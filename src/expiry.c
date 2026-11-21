/* expiry.c
 * 담당: 김기범
 * 설명: 날짜 계산, D-day, 임박순 정렬, 임박 개수 (F2, F4)
 *
 * [뼈대] 컴파일만 되는 빈 껍데기입니다. 담당자가 README 6.3장을 보고 내용을 채웁니다.
 *        함수 이름과 매개변수, 반환값은 expiry.h 그대로 두세요.
 */
#include "expiry.h"

Date getToday(void)
{
    Date d = { 2000, 1, 1 };
    return d;
}

int isValidDate(Date d)
{
    (void)d;
    return 1;
}

int daysBetween(Date from, Date to)
{
    (void)from; (void)to;
    return 0;
}

int daysLeft(Date expire)
{
    (void)expire;
    return 0;
}

void sortByExpiry(Fridge *f)
{
    (void)f;
}

void countAlerts(const Fridge *f, int *expired, int *soon)
{
    (void)f;
    *expired = 0;
    *soon = 0;
}

void formatDday(int days, char *buf, int bufLen)
{
    (void)days;
    if (bufLen > 0) buf[0] = '\0';
}
