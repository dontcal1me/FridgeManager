/* test.h
 * 담당: 윤종하
 * 설명: 모듈 단독 테스트 도우미.
 *       실행: 프로젝트 속성 → 디버깅 → 명령 인수에 "--test food" 처럼 입력하고 Ctrl+F5
 *       (util, expiry, recipe, food, stats, fileio 중 하나)
 *       테스트 파일은 로직 모듈이 아니므로 printf를 써도 된다.
 */
#ifndef TEST_H
#define TEST_H

#include <stdio.h>

#define CHECK(cond) printf("%s  %s\n", (cond) ? "[PASS]" : "[FAIL]", #cond)

int  runTest(const char *name);   /* main.c에 있음. 이름에 맞는 testXxx()를 호출 */

void testUtil(void);     /* test_util.c    : 장태현 */
void testExpiry(void);   /* test_expiry.c  : 김기범 */
void testRecipe(void);   /* test_recipe.c  : 최민솔 */
void testFood(void);     /* test_food.c    : 박지연 */
void testStats(void);    /* test_stats.c   : 박지연 */
void testFileio(void);   /* test_fileio.c  : 윤종하 */

#endif
