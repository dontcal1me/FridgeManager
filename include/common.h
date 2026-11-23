/* common.h
 * 담당: 윤종하
 * 설명: 모든 모듈이 함께 쓰는 상수, 열거형, 구조체 (README 11장과 같다)
 *       확정 후에는 윤종하만 수정한다.
 */
#ifndef COMMON_H
#define COMMON_H

#define NAME_LEN     30
#define MAX_ING      10
#define ALERT_DAYS   2      /* 임박 기준: 2일 이내 */

#define FRIDGE_FILE  "data/fridge.txt"
#define RECIPE_FILE  "data/recipes.txt"
#define WASTE_FILE   "data/waste.txt"

typedef enum {
    RC_OK = 0,
    RC_NOT_FOUND,     /* 대상 없음 */
    RC_INVALID,       /* 잘못된 입력값 */
    RC_NOT_ENOUGH,    /* 재료 부족 */
    RC_NO_MEMORY,     /* 메모리 할당 실패 */
    RC_FILE_ERROR     /* 파일 열기·쓰기 실패 */
} ResultCode;

typedef enum { FRIDGE = 0, FREEZER = 1 } Storage;   /* 냉장, 냉동 */

typedef struct {
    int year, month, day;
} Date;

typedef struct {
    char    name[NAME_LEN];
    Date    expire;
    int     quantity;
    Storage storage;
} Food;

typedef struct {
    Food *items;      /* 동적 배열 */
    int   count;
    int   capacity;   /* 가득 차면 realloc으로 2배 확장 */
} Fridge;

typedef struct {
    char name[NAME_LEN];
    char ingredients[MAX_ING][NAME_LEN];
    int  amounts[MAX_ING];    /* 재료별 필요 수량 */
    int  ingCount;
    int  isCustom;            /* 0: 기본 레시피, 1: 사용자 추가 */
} Recipe;

typedef struct {
    Recipe *items;
    int     count;
    int     capacity;
} RecipeBook;

typedef struct {
    int  recipeIdx;           /* RecipeBook 안의 인덱스 */
    int  matched;             /* 보유한 재료 수 */
    int  score;               /* 추천 점수 */
    int  canCook;             /* 1이면 바로 요리 가능 */
    int  missingCount;
    char missing[MAX_ING][NAME_LEN];   /* 부족한 재료 이름 */
    int  urgentCount;         /* 사용하면 좋은 임박 재료 수 (+5점씩) */
    char urgent[MAX_ING][NAME_LEN];    /* 임박 재료 이름 (화면의 "임박 재료: ..."에 사용) */
} Recommendation;

typedef struct {
    char    name[NAME_LEN];
    int     quantity;
    Storage storage;
    Date    expire;
    Date    discarded;        /* 폐기한 날짜 */
} WasteRecord;

typedef struct {
    WasteRecord *items;
    int          count;
    int          capacity;
} WasteList;

typedef struct {
    int  totalRecords;        /* 폐기 횟수 */
    int  totalQuantity;       /* 폐기한 재료 총 수량 */
    int  fridgeCount;         /* 냉장 재료 폐기 횟수 */
    int  freezerCount;        /* 냉동 재료 폐기 횟수 */
    int  thisMonth;           /* 이번 달 폐기 횟수 */
    char topName[NAME_LEN];   /* 가장 많이 버린 재료 */
    int  topCount;
} WasteStats;

typedef struct {              /* 화면 모듈이 들고 다니는 전체 데이터 */
    Fridge     fridge;
    RecipeBook recipes;
    WasteList  waste;
} AppData;

#endif
