/* guide_api.h
 * 담당: 윤종하
 * 설명: 식품안전나라 "조리식품의 레시피 DB"(COOKRCP01) OpenAPI에서 조리 순서만 가져온다.
 *       인증키는 환경변수 FOODSAFETY_API_KEY 또는 data/api_key.txt (첫 줄)에서 읽는다.
 *       키는 코드에 쓰지 않는다. (data/api_key.txt는 .gitignore에 들어 있다)
 *       이 모듈은 화면 입출력을 하지 않는다. 결과는 ApiResult와 배열로 돌려준다.
 */
#ifndef GUIDE_API_H
#define GUIDE_API_H

#include "guide.h"

#define API_KEY_FILE  "data/api_key.txt"
#define API_KEY_ENV   "FOODSAFETY_API_KEY"

typedef enum {
    API_OK = 0,
    API_NO_KEY,         /* 인증키가 없거나 형식이 이상함 */
    API_AUTH_ERROR,     /* 서버가 키를 거절함 (INFO-100/110/120/600) */
    API_LIMIT,          /* 호출 제한 또는 사용 중 (INFO-320/500, ERROR-503 등) */
    API_NET_ERROR,      /* 인터넷 연결·응답 실패 */
    API_NOT_FOUND,      /* 검색 결과 없음 */
    API_PARSE_ERROR     /* 응답에서 조리 순서를 읽지 못함 */
} ApiResult;

/* 인증키를 쓸 수 있으면 1 */
int hasApiKey(void);

/* 요리 이름으로 조리 순서를 인터넷에서 가져온다.
 * steps에 최대 max단계를 채우고 *outCount에 단계 수를, matchedName에 검색된 요리 이름을 돌려준다.
 * 이름이 정확히 같은 결과를 우선하고, 없으면 첫 번째 결과를 쓴다. */
ApiResult fetchRecipeSteps(const char *recipeName, char steps[][STEP_LEN], int max,
                           int *outCount, char *matchedName, int matchedLen);

/* 서버 응답 본문(JSON)에서 조리 순서를 뽑는다. 네트워크 없이 시험할 수 있도록 따로 열어 둔다.
 * 응답이 UTF-8이든 ANSI(949)이든 steps에는 ANSI(949) 문자열을 채운다. */
ApiResult parseRecipeResponse(const char *body, const char *recipeName, char steps[][STEP_LEN], int max,
                              int *outCount, char *matchedName, int matchedLen);

/* ---- 재료 이름으로 요리 찾기 (API 추천용) ---- */
#define API_NAME_LEN   64        /* 요리 이름 최대 바이트 */
#define API_PARTS_LEN  2048      /* 재료 정보(RCP_PARTS_DTLS) 최대 바이트 */
#ifndef API_PAGE_SIZE
#define API_PAGE_SIZE  20        /* 재료 하나당 받아 오는 요리 수 */
#endif

typedef struct {
    char name[API_NAME_LEN];              /* 요리 이름 (ANSI 949) */
    char parts[API_PARTS_LEN];            /* 재료 정보 원문: 재료 이름과 양이 섞인 글 (ANSI 949) */
    char steps[MAX_STEPS][STEP_LEN];      /* 조리 순서 */
    int  stepCount;
} ApiRecipe;

/* 재료 이름이 들어간 요리를 인터넷에서 찾아 out에 최대 max개 채운다. 요리 이름·재료 정보·조리 순서가 한 번에 온다 */
ApiResult fetchRecipesByIngredient(const char *ingredient, ApiRecipe *out, int max, int *outCount);

/* 서버 응답 본문(JSON)에서 요리 여러 개를 뽑는다. 네트워크 없이 시험할 수 있도록 따로 열어 둔다 */
ApiResult parseRecipeRows(const char *body, ApiRecipe *out, int max, int *outCount);
#endif
