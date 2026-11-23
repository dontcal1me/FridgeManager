/* expiry.c
 * 담당: 김기범
 * 설명: 날짜 계산·D-day·임박순 정렬·임박 개수 (F2, F4)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "expiry.h"

/* TODO(김기범): 구현 전 뼈대. 컴파일만 되는 빈 껍데기입니다. */

Date getToday(void) { Date d = { 2026, 1, 1 }; return d; }
int isValidDate(Date d) { (void)d; return 0; }
int daysBetween(Date from, Date to) { (void)from; (void)to; return 0; }
int daysLeft(Date expire) { (void)expire; return 0; }
void sortByExpiry(Fridge *f) { (void)f; }
void countAlerts(const Fridge *f, int *expired, int *soon) { (void)f; if (expired) *expired = 0; if (soon) *soon = 0; }
void formatDday(int days, char *buf, int bufLen) { (void)days; if (buf && bufLen > 0) buf[0] = '\0'; }