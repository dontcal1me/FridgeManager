/* util.h
 * 담당: 장태현
 * 설명: 잘못된 입력에도 멈추지 않는 콘솔 입력 함수
 *       함수 선언은 README 6.2장과 같다. 윤종하가 확정했으므로 바꾸지 않는다.
 */
#ifndef UTIL_H
#define UTIL_H

#include "common.h"

void clearInputBuffer(void);                              /* 입력 버퍼에 남은 줄 비우기 */
int  inputInt(const char *msg, int min, int max);         /* min~max 정수가 나올 때까지 반복 입력 */
int  inputDate(const char *msg, Date *d);                 /* YYYY-MM-DD 입력. 성공 1, "0" 입력(취소) 0 */
void inputWord(const char *msg, char *buf, int bufLen);   /* 공백 없는 단어 입력 (이름용) */
int  inputYesNo(const char *msg);                         /* y/Y면 1, n/N이면 0 */
void waitForKey(void);                                    /* 아무 키나 누를 때까지 대기 (_getch) */

#endif
