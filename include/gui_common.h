/* gui_common.h
 * 담당: 윤종하
 * 설명: GUI의 모든 파일이 함께 쓰는 전역 상태와 도우미 함수
 *       (문자 변환, 메시지 창, 컨트롤 만들기, 표(ListView) 도우미, 메인 목록 갱신)
 */
#ifndef GUI_COMMON_H
#define GUI_COMMON_H

#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#include <windows.h>
#include <commctrl.h>
#include "common.h"

#define DATA_CP    949                        /* 데이터 파일의 문자 코드 (한국어 ANSI) */
#define APP_TITLE  L"냉장고 관리"

/* 전역 상태 (정의는 gui_common.c) */
extern AppData *g_app;                        /* 프로그램 전체 데이터 */
extern HWND     g_main, g_list, g_status;     /* 메인 창, 식재료 표, 상태줄 */
extern HFONT    g_font;                       /* 모든 컨트롤이 쓰는 글꼴 */
extern int      g_dpi;                        /* 화면 배율 (96 = 100%) */

/* 96dpi 기준 픽셀 값을 현재 화면 배율로 바꾼다 */
int  S(int v);

/* 문자 변환·문자열 도우미 */
void ansiToWide(const char *src, wchar_t *dst, int dstLen);       /* ANSI(949) → 유니코드 */
int  wideToAnsi(const wchar_t *src, char *dst, int dstLen);       /* 유니코드 → ANSI(949). 넘치거나 변환 불가면 0 */
void trim(wchar_t *s);                                            /* 앞뒤 공백 제거 */
int  hasSpace(const wchar_t *s);                                  /* 공백이 있으면 1 */
void catf(wchar_t *buf, size_t cap, const wchar_t *fmt, ...);     /* buf 끝에 서식 문자열 덧붙이기 */
void dateStr(Date d, wchar_t *buf, int n);                        /* "YYYY-MM-DD" 문자열 만들기 */
int  parseDate(const wchar_t *s, Date *d);                        /* "YYYY-MM-DD" 읽기. 올바른 날짜면 1 */
void ddayText(int days, wchar_t *buf, int n);                     /* "D-3 (임박)" 같은 문자열 */

/* 메시지 창·컨트롤 도우미 */
void msg(HWND owner, const wchar_t *text, UINT icon);             /* 확인 버튼만 있는 안내 창 */
int  ask(HWND owner, const wchar_t *text);                        /* 예/아니오 질문 창. 예면 1 */
void bad(HWND dlg, int id, const wchar_t *text);                  /* 안내 후 해당 입력칸으로 커서 이동 */
HWND mk(HWND parent, const wchar_t *cls, const wchar_t *text, DWORD style, int id,
        int x, int y, int w, int h);                              /* 컨트롤 만들기 (좌표는 96dpi 기준) */
int  readName(HWND dlg, int id, const wchar_t *label, char *out); /* 이름 입력칸 읽기·검사 */

/* 표(ListView) 도우미 */
void lvCol(HWND lv, int i, const wchar_t *t, int w, int fmt);
int  lvAdd(HWND lv, int row, const wchar_t *t);
void lvSet(HWND lv, int row, int col, const wchar_t *t);
void lvSelect(HWND lv, int row);

/* 메인 창 */
void setNote(const wchar_t *fmt, ...);                            /* 상태줄에 보여 줄 마지막 작업 */
void refreshMain(void);                                           /* 임박순 정렬 후 표와 상태줄 갱신 */

#endif
