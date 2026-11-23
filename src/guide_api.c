/* guide_api.c
 * 담당: 윤종하
 * 설명: 식품안전나라 COOKRCP01 API에서 조리 순서(MANUAL01~20)를 가져온다. (화면 입출력 없음)
 *
 * 공식 명세에서 확인한 것: 요청 주소 형식, 요리 이름 RCP_NM, 조리 순서 MANUAL01~MANUAL20.
 * 명세에 없는 것: JSON 최상위 구조, 문자 인코딩. 그래서 구조에 기대지 않고 "키":"값" 을 직접 찾고,
 *                 UTF-8과 ANSI(949) 응답을 모두 처리한다.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <windows.h>
#include <winhttp.h>
#include "guide_api.h"

#pragma comment(lib, "winhttp.lib")

#define API_HOST     L"openapi.foodsafetykorea.go.kr"
#define API_MAX_BODY (2 * 1024 * 1024)      /* 응답 본문 최대 크기 */

/* ---------- 문자열 도우미 ---------- */

/* 코드포인트를 UTF-8로 out에 쓰고 쓴 바이트 수를 반환한다 */
static int encodeUtf8(unsigned long cp, char *out)
{
    if (cp < 0x80)    { out[0] = (char)cp; return 1; }
    if (cp < 0x800)   { out[0] = (char)(0xC0 | (cp >> 6)); out[1] = (char)(0x80 | (cp & 0x3F)); return 2; }
    if (cp < 0x10000) { out[0] = (char)(0xE0 | (cp >> 12)); out[1] = (char)(0x80 | ((cp >> 6) & 0x3F));
                        out[2] = (char)(0x80 | (cp & 0x3F)); return 3; }
    out[0] = (char)(0xF0 | (cp >> 18)); out[1] = (char)(0x80 | ((cp >> 12) & 0x3F));
    out[2] = (char)(0x80 | ((cp >> 6) & 0x3F)); out[3] = (char)(0x80 | (cp & 0x3F));
    return 4;
}

/* UTF-8 문자열을 ANSI(949)로 바꾼다. 올바른 UTF-8이 아니면 이미 ANSI라고 보고 그대로 복사한다 */
static void toAnsi(const char *src, char *dst, int dstLen)
{
    wchar_t wide[2048];
    int n = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, src, -1, wide, 2048);
    if (n > 0 && WideCharToMultiByte(CP_ACP, 0, wide, -1, dst, dstLen, NULL, NULL) > 0)
        return;
    strncpy(dst, src, dstLen - 1);
    dst[dstLen - 1] = '\0';
}

/* ANSI(949) 문자열을 UTF-8로 바꾼다 (요리 이름을 주소에 넣기 위함) */
static int ansiToUtf8(const char *src, char *dst, int dstLen)
{
    wchar_t wide[256];
    if (MultiByteToWideChar(CP_ACP, 0, src, -1, wide, 256) <= 0) return 0;
    return WideCharToMultiByte(CP_UTF8, 0, wide, -1, dst, dstLen, NULL, NULL) > 0;
}

/* 문자열 끝의 공백을 지우고 앞의 공백도 건너뛴 시작 위치를 반환한다 */
static char *trim(char *s)
{
    size_t n;
    while (*s == ' ' || *s == '\t') s++;
    n = strlen(s);
    while (n > 0 && (s[n - 1] == ' ' || s[n - 1] == '\t' || s[n - 1] == '\r' || s[n - 1] == '\n'))
        s[--n] = '\0';
    return s;
}

/* ---------- JSON 읽기 (필요한 만큼만) ---------- */

/* p가 가리키는 따옴표 뒤의 JSON 문자열을 UTF-8로 풀어 out에 쓴다. 닫는 따옴표 다음 위치를 반환 (실패 시 NULL) */
static const char *readJsonString(const char *p, char *out, int outLen)
{
    int n = 0;

    while (*p && *p != '"') {
        char buf[4];
        int len = 0;

        if (*p == '\\') {
            p++;
            switch (*p) {
            case 'n': case 'r': case 't': buf[0] = ' '; len = 1; p++; break;
            case 'b': case 'f': p++; continue;
            case 'u': {
                unsigned long cp = 0;
                int i;
                for (i = 1; i <= 4; i++) {
                    char c = p[i];
                    if (!isxdigit((unsigned char)c)) return NULL;
                    cp = cp * 16 + (unsigned long)(isdigit((unsigned char)c) ? c - '0' : (tolower(c) - 'a' + 10));
                }
                p += 5;
                if (cp >= 0xD800 && cp <= 0xDBFF && p[0] == '\\' && p[1] == 'u') {    /* 서로게이트 쌍 */
                    unsigned long lo = 0;
                    for (i = 2; i <= 5; i++) {
                        char c = p[i];
                        if (!isxdigit((unsigned char)c)) return NULL;
                        lo = lo * 16 + (unsigned long)(isdigit((unsigned char)c) ? c - '0' : (tolower(c) - 'a' + 10));
                    }
                    cp = 0x10000 + ((cp - 0xD800) << 10) + (lo - 0xDC00);
                    p += 6;
                }
                {
                    char u[4];
                    len = encodeUtf8(cp, u);
                    memcpy(buf, u, len);
                }
                break;
            }
            case '\0': return NULL;
            default: buf[0] = *p; len = 1; p++; break;      /* \" \\ \/ 등 */
            }
        } else {
            buf[0] = *p; len = 1; p++;
        }
        if (n + len < outLen) {
            memcpy(out + n, buf, len);
            n += len;
        }
    }
    if (*p != '"') return NULL;
    out[n] = '\0';
    return p + 1;
}

/* [from, to) 범위에서 "key":"값" 의 값을 UTF-8로 읽는다. 있고 문자열이면 1 */
static int getJsonString(const char *from, const char *to, const char *key, char *out, int outLen)
{
    char pat[48];
    const char *p = from;
    size_t plen;

    snprintf(pat, sizeof(pat), "\"%s\"", key);
    plen = strlen(pat);
    while (p < to && (p = strstr(p, pat)) != NULL && p < to) {
        const char *q = p + plen;
        while (q < to && (*q == ' ' || *q == '\t' || *q == '\r' || *q == '\n')) q++;
        if (q < to && *q == ':') {
            q++;
            while (q < to && (*q == ' ' || *q == '\t' || *q == '\r' || *q == '\n')) q++;
            if (q < to && *q == '"')
                return readJsonString(q + 1, out, outLen) != NULL;
            return 0;                                         /* null, 숫자 등 */
        }
        p += plen;
    }
    return 0;
}

/* 한 단계 설명을 다듬는다: 앞의 "1. " 번호와 끝에 붙는 이미지 표시 글자(a, b ...)를 지운다 */
static void cleanStep(char *s)
{
    char *p = s;
    size_t n;

    while (*p == ' ') p++;
    if (isdigit((unsigned char)*p)) {                          /* "1. ", "12) " 같은 번호 */
        char *q = p;
        while (isdigit((unsigned char)*q)) q++;
        if ((*q == '.' || *q == ')') && q - p <= 2) {
            q++;
            while (*q == ' ') q++;
            p = q;
        }
    }
    if (p != s) memmove(s, p, strlen(p) + 1);

    n = strlen(s);
    while (n > 0 && (s[n - 1] == ' ' || s[n - 1] == '\r' || s[n - 1] == '\n')) s[--n] = '\0';
    if (n >= 2 && islower((unsigned char)s[n - 1]) &&
        (s[n - 2] == '.' || (unsigned char)s[n - 2] >= 0x80))   /* "...했다.a" 처럼 문장 끝에 붙은 한 글자 */
        s[n - 1] = '\0';
}

/* dst(STEP_LEN)에 한글이 중간에 잘리지 않게 복사한다 */
static void copyStep(char *dst, const char *src)
{
    int i = 0, cut = 0;
    while (src[i] != '\0') {
        int step = ((unsigned char)src[i] >= 0x80) ? 2 : 1;
        if (cut + step > STEP_LEN - 1) break;
        cut += step;
        i += step;
    }
    memcpy(dst, src, cut);
    dst[cut] = '\0';
}

/* ---------- 응답 해석 ---------- */

/* 응답에 들어 있는 서버 결과 코드로 오류 종류를 판단한다 */
static ApiResult classifyError(const char *body)
{
    if (strstr(body, "INFO-100") || strstr(body, "INFO-110") ||
        strstr(body, "INFO-120") || strstr(body, "INFO-600"))
        return API_AUTH_ERROR;
    if (strstr(body, "INFO-320") || strstr(body, "INFO-500") || strstr(body, "ERROR-503") ||
        strstr(body, "INFO-330") || strstr(body, "INFO-310"))
        return API_LIMIT;
    if (strstr(body, "INFO-200"))
        return API_NOT_FOUND;
    return API_PARSE_ERROR;
}

/* 서버 응답 본문(JSON)에서 조리 순서를 뽑는다 */
ApiResult parseRecipeResponse(const char *body, const char *recipeName, char steps[][STEP_LEN], int max,
                              int *outCount, char *matchedName, int matchedLen)
{
    const char *p, *best = NULL, *bestEnd = NULL;
    char nameAnsi[NAME_LEN * 2], name[512];
    int count = 0, i;

    if (outCount) *outCount = 0;
    if (matchedName && matchedLen > 0) matchedName[0] = '\0';
    if (body == NULL || recipeName == NULL || steps == NULL || max <= 0) return API_PARSE_ERROR;

    /* "RCP_NM" 이 들어 있는 행(바깥 { } 한 쌍)을 모두 훑어 이름이 같은 행을 찾는다. 없으면 첫 행 */
    p = body;
    while ((p = strstr(p, "\"RCP_NM\"")) != NULL) {
        const char *s = p, *e;
        while (s > body && *s != '{') s--;                     /* 행은 평평한 객체라 가장 가까운 { } 가 행이다 */
        e = strchr(p, '}');
        if (*s != '{' || e == NULL) { p += 8; continue; }

        if (getJsonString(s, e, "RCP_NM", name, sizeof(name))) {
            toAnsi(name, nameAnsi, sizeof(nameAnsi));
            if (best == NULL) { best = s; bestEnd = e; }
            if (strcmp(nameAnsi, recipeName) == 0) { best = s; bestEnd = e; break; }
        }
        p = e + 1;
    }
    if (best == NULL)
        return classifyError(body);

    if (matchedName && matchedLen > 0 && getJsonString(best, bestEnd, "RCP_NM", name, sizeof(name))) {
        toAnsi(name, nameAnsi, sizeof(nameAnsi));
        strncpy(matchedName, nameAnsi, matchedLen - 1);
        matchedName[matchedLen - 1] = '\0';
    }

    for (i = 1; i <= 20 && count < max; i++) {
        char key[16], raw[1024], ansi[1024], *t;
        snprintf(key, sizeof(key), "MANUAL%02d", i);
        if (!getJsonString(best, bestEnd, key, raw, sizeof(raw))) continue;
        toAnsi(raw, ansi, sizeof(ansi));
        cleanStep(ansi);
        t = trim(ansi);
        if (t[0] == '\0') continue;
        copyStep(steps[count], t);
        count++;
    }
    if (count == 0) return API_PARSE_ERROR;
    if (outCount) *outCount = count;
    return API_OK;
}

/* ---------- 인증키 ---------- */

/* 인증키를 읽는다. 영문·숫자만 허용해서 주소가 엉뚱하게 바뀌는 일을 막는다. 쓸 수 있으면 1 */
static int readApiKey(char *key, int keyLen)
{
    const char *env = getenv(API_KEY_ENV);
    char line[256];
    int i;

    key[0] = '\0';
    if (env != NULL && env[0] != '\0') {
        strncpy(line, env, sizeof(line) - 1);
        line[sizeof(line) - 1] = '\0';
    } else {
        FILE *fp = fopen(API_KEY_FILE, "r");
        if (fp == NULL) return 0;
        if (fgets(line, sizeof(line), fp) == NULL) { fclose(fp); return 0; }
        fclose(fp);
    }
    {
        char *t = trim(line);
        size_t n = strlen(t);
        if (n == 0 || (int)n >= keyLen) return 0;
        for (i = 0; i < (int)n; i++)
            if (!isalnum((unsigned char)t[i])) return 0;
        memcpy(key, t, n + 1);
    }
    return 1;
}

/* 인증키를 쓸 수 있으면 1 */
int hasApiKey(void)
{
    char key[128];
    return readApiKey(key, sizeof(key));
}

/* ---------- 통신 ---------- */

/* 주소에 넣기 위해 UTF-8 바이트를 %XX 로 바꾼다 */
static void percentEncode(const char *src, char *dst, int dstLen)
{
    static const char hex[] = "0123456789ABCDEF";
    int n = 0;
    for (; *src; src++) {
        unsigned char c = (unsigned char)*src;
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            if (n + 1 < dstLen) dst[n++] = (char)c;
        } else if (n + 3 < dstLen) {
            dst[n++] = '%'; dst[n++] = hex[c >> 4]; dst[n++] = hex[c & 15];
        }
    }
    dst[n] = '\0';
}

/* GET 요청을 보내고 응답 본문을 malloc한 문자열로 돌려준다. 실패하면 NULL */
static char *httpGet(const char *pathUtf8)
{
    HINTERNET ses = NULL, con = NULL, req = NULL;
    wchar_t wpath[1024];
    char *body = NULL;
    size_t used = 0;
    DWORD got = 0;
    int ok = 0;

    if (MultiByteToWideChar(CP_UTF8, 0, pathUtf8, -1, wpath, 1024) <= 0) return NULL;

    ses = WinHttpOpen(L"FridgeManager/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                      WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (ses == NULL) goto done;
    WinHttpSetTimeouts(ses, 5000, 5000, 8000, 8000);           /* 이름 확인·연결·전송·수신 (ms) */

    con = WinHttpConnect(ses, API_HOST, INTERNET_DEFAULT_HTTP_PORT, 0);
    if (con == NULL) goto done;
    req = WinHttpOpenRequest(con, L"GET", wpath, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
    if (req == NULL) goto done;
    if (!WinHttpSendRequest(req, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0)) goto done;
    if (!WinHttpReceiveResponse(req, NULL)) goto done;

    body = (char *)malloc(65536);
    if (body == NULL) goto done;
    {
        size_t cap = 65536;
        for (;;) {
            if (used + 8192 + 1 > cap) {
                char *tmp;
                if (cap >= API_MAX_BODY) goto done;            /* 비정상적으로 큰 응답은 거절 */
                tmp = (char *)realloc(body, cap * 2);
                if (tmp == NULL) goto done;
                body = tmp;
                cap *= 2;
            }
            if (!WinHttpReadData(req, body + used, 8192, &got)) goto done;
            if (got == 0) break;
            used += got;
        }
    }
    body[used] = '\0';
    ok = 1;

done:
    if (req) WinHttpCloseHandle(req);
    if (con) WinHttpCloseHandle(con);
    if (ses) WinHttpCloseHandle(ses);
    if (!ok) { free(body); return NULL; }
    return body;
}

/* 요리 이름으로 조리 순서를 인터넷에서 가져온다 */
ApiResult fetchRecipeSteps(const char *recipeName, char steps[][STEP_LEN], int max,
                           int *outCount, char *matchedName, int matchedLen)
{
    char key[128], utf8[256], enc[600], path[900];
    char *body;
    ApiResult r;

    if (outCount) *outCount = 0;
    if (recipeName == NULL || recipeName[0] == '\0') return API_NOT_FOUND;
    if (!readApiKey(key, sizeof(key))) return API_NO_KEY;
    if (!ansiToUtf8(recipeName, utf8, sizeof(utf8))) return API_NOT_FOUND;

    percentEncode(utf8, enc, sizeof(enc));
    snprintf(path, sizeof(path), "/api/%s/COOKRCP01/json/1/5/RCP_NM=%s", key, enc);

    body = httpGet(path);
    if (body == NULL) return API_NET_ERROR;
    r = parseRecipeResponse(body, recipeName, steps, max, outCount, matchedName, matchedLen);
    free(body);
    return r;
}

/* ---------- 재료 이름으로 요리 찾기 ---------- */

/* src를 dst(dstLen)에 한글이 중간에 잘리지 않게 복사한다 */
static void copyTrunc(char *dst, int dstLen, const char *src)
{
    int i = 0, cut = 0;
    while (src[i] != '\0') {
        int step = ((unsigned char)src[i] >= 0x80 && src[i + 1] != '\0') ? 2 : 1;
        if (cut + step > dstLen - 1) break;
        cut += step;
        i += step;
    }
    memcpy(dst, src, cut);
    dst[cut] = '\0';
}

/* 서버 응답에서 요리 행을 모두 뽑는다 (요리 이름, 재료 정보, 조리 순서) */
ApiResult parseRecipeRows(const char *body, ApiRecipe *out, int max, int *outCount)
{
    const char *p;
    char raw[5000], ansi[4096];
    int n = 0, i;

    if (outCount) *outCount = 0;
    if (body == NULL || out == NULL || max <= 0) return API_PARSE_ERROR;

    p = body;
    while (n < max && (p = strstr(p, "\"RCP_NM\"")) != NULL) {
        const char *s = p, *e;
        ApiRecipe *r = &out[n];

        while (s > body && *s != '{') s--;                     /* 행은 평평한 객체라 가장 가까운 { } 가 행이다 */
        e = strchr(p, '}');
        if (*s != '{' || e == NULL) { p += 8; continue; }

        memset(r, 0, sizeof(*r));
        if (!getJsonString(s, e, "RCP_NM", raw, sizeof(raw))) { p = e + 1; continue; }
        toAnsi(raw, ansi, sizeof(ansi));
        copyTrunc(r->name, API_NAME_LEN, trim(ansi));
        if (r->name[0] == '\0') { p = e + 1; continue; }

        if (getJsonString(s, e, "RCP_PARTS_DTLS", raw, sizeof(raw))) {
            toAnsi(raw, ansi, sizeof(ansi));
            copyTrunc(r->parts, API_PARTS_LEN, trim(ansi));
        }
        for (i = 1; i <= 20 && r->stepCount < MAX_STEPS; i++) {
            char key[16], stepRaw[1024], stepAnsi[1024], *t;
            snprintf(key, sizeof(key), "MANUAL%02d", i);
            if (!getJsonString(s, e, key, stepRaw, sizeof(stepRaw))) continue;
            toAnsi(stepRaw, stepAnsi, sizeof(stepAnsi));
            cleanStep(stepAnsi);
            t = trim(stepAnsi);
            if (t[0] == '\0') continue;
            copyStep(r->steps[r->stepCount], t);
            r->stepCount++;
        }
        n++;
        p = e + 1;
    }
    if (n == 0) return classifyError(body);
    if (outCount) *outCount = n;
    return API_OK;
}

/* 재료 이름이 들어간 요리를 인터넷에서 가져온다 (재료 정보 검색: RCP_PARTS_DTLS) */
ApiResult fetchRecipesByIngredient(const char *ingredient, ApiRecipe *out, int max, int *outCount)
{
    char key[128], utf8[256], enc[600], path[900];
    char *body;
    ApiResult r;

    if (outCount) *outCount = 0;
    if (ingredient == NULL || ingredient[0] == '\0') return API_NOT_FOUND;
    if (!readApiKey(key, sizeof(key))) return API_NO_KEY;
    if (!ansiToUtf8(ingredient, utf8, sizeof(utf8))) return API_NOT_FOUND;

    percentEncode(utf8, enc, sizeof(enc));
    snprintf(path, sizeof(path), "/api/%s/COOKRCP01/json/1/%d/RCP_PARTS_DTLS=%s", key, API_PAGE_SIZE, enc);

    body = httpGet(path);
    if (body == NULL) return API_NET_ERROR;
    r = parseRecipeRows(body, out, max, outCount);
    free(body);
    return r;
}