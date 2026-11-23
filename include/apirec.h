/* apirec.h
 * 담당: 윤종하
 * 설명: API 요리 추천 (도전 목표). 내 냉장고의 재료 이름으로 식품안전나라 레시피를 찾고,
 *       재료의 양·개수는 보지 않고 "이름이 재료 정보에 나오는가"만 대조해 점수를 매긴다.
 *       이 모듈은 화면 입출력을 하지 않는다.
 */
#ifndef APIREC_H
#define APIREC_H

#include "common.h"
#include "guide_api.h"

#define MAX_ING_REF   30     /* 대조할 내 재료 이름 최대 개수 */
#define MAX_QUERY     8      /* 한 번의 추천에서 API를 부르는 최대 횟수 (재료 이름 또는 같은 뜻의 이름 하나당 1회) */
#define MAX_API_REC   30     /* 추천 결과 최대 개수 */
#define MAX_HAVE      12     /* 요리마다 기록하는 "내가 가진 재료" 이름 최대 개수 */

typedef struct {
    char name[NAME_LEN];     /* 재료 이름 */
    int  urgent;             /* 1이면 임박 재료 (기한이 지나지 않았고 ALERT_DAYS 이내) */
} IngredientRef;

typedef struct {
    ApiRecipe recipe;                      /* 요리 이름, 재료 정보 원문, 조리 순서 */
    int  matched;                          /* 이름이 재료 정보에 나온 내 재료의 개수 */
    int  urgentMatched;                    /* 그중 임박 재료의 개수 */
    int  score;                            /* matched × 10 + urgentMatched × 5 */
    int  haveCount;                        /* have[]에 담긴 개수 (최대 MAX_HAVE) */
    char have[MAX_HAVE][NAME_LEN];         /* 이름이 나온 내 재료 */
    int  haveUrgent[MAX_HAVE];             /* have[i]가 임박 재료면 1 */
} ApiRecommendation;

/* 냉장고에서 기한이 지나지 않은 재료의 이름을 중복 없이 모은다. 임박 재료가 앞에 온다. 모은 개수를 반환 */
int collectIngredients(const Fridge *f, IngredientRef *out, int max);

/* ings[0..n-1]의 재료 이름으로 API를 불러 요리를 찾고, 이름을 대조해 점수순으로 out에 채운다.
 * 인터넷을 쓰므로 시간이 걸린다 (별도 스레드에서 호출할 것). Fridge를 직접 읽지 않는다.
 * 결과가 있으면 API_OK와 *outCount, 없으면 API_NOT_FOUND, 오류면 해당 ApiResult */
ApiResult apiRecommend(const IngredientRef *ings, int n, ApiRecommendation *out, int maxOut, int *outCount);

/* 재료 정보 글(text) 안에 재료 이름(name)이나 같은 뜻의 이름이 나오는지. 나오면 1 (시험용으로 열어 둠) */
int ingredientInText(const char *text, const char *name);

#endif
