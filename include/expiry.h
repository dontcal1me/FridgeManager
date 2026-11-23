/* expiry.h
 * 담당: 김기범
 * 설명: 날짜 계산, D-day, 임박순 정렬, 임박 개수 (F2, F4)
 *       함수 선언은 README 6.3장과 같다. 윤종하가 확정했으므로 바꾸지 않는다.
 */
#ifndef EXPIRY_H
#define EXPIRY_H

#include "common.h"

Date getToday(void);                                          /* 오늘 날짜 */
int  isValidDate(Date d);                                     /* 올바른 날짜면 1, 아니면 0 (윤년, 월별 일수) */
int  daysBetween(Date from, Date to);                         /* to - from (일). to가 뒤면 양수 */
int  daysLeft(Date expire);                                   /* expire - 오늘. 지났으면 음수 */
void sortByExpiry(Fridge *f);                                 /* F2: 임박순(유통기한 빠른 순) 정렬 */
void countAlerts(const Fridge *f, int *expired, int *soon);   /* F4: 기한 지난 개수, 임박 개수 */
void formatDday(int days, char *buf, int bufLen);             /* "D-3", "D-DAY", "D+2" 문자열 만들기 */

#endif
