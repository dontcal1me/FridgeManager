/* guide.h
 * 담당: 윤종하
 * 설명: 요리별 조리 순서(레시피 가이드) 조회와 저장. data/steps.txt를 사용한다.
 *       파일 한 줄 형식: 요리이름|1단계 설명|2단계 설명|...   (ANSI, 단계 설명에는 공백 가능)
 */
#ifndef GUIDE_H
#define GUIDE_H

#include "common.h"

#define GUIDE_FILE  "data/steps.txt"
#define MAX_STEPS   20      /* 요리 하나의 최대 단계 수 (식품안전나라 API의 MANUAL01~20과 같다) */
#define STEP_LEN    256     /* 한 단계 설명의 최대 바이트 (한글 127자) */

/* 요리 이름으로 조리 순서를 찾아 steps에 복사하고 단계 수를 반환한다. 없으면 0 (파일은 처음 부를 때 한 번 읽는다) */
int        getRecipeSteps(const char *recipeName, char steps[][STEP_LEN], int max);

/* 조리 순서를 메모리와 파일 끝에 추가한다. 같은 이름이 이미 있으면 RC_INVALID, 파일을 못 쓰면 RC_FILE_ERROR */
ResultCode addRecipeSteps(const char *recipeName, char steps[][STEP_LEN], int count);

/* 조리 순서를 바꾼다. 같은 이름이 없으면 새로 추가한다. 파일 전체를 임시 파일에 쓴 뒤 교체한다 (인터넷에서 받은 조리법 저장용) */
ResultCode replaceRecipeSteps(const char *recipeName, char steps[][STEP_LEN], int count);

#endif
