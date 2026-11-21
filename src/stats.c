/* stats.c
 * 담당: 박지연
 * 설명: 폐기 기록, 일괄 폐기, 통계 (F6)
 *
 * [뼈대] 컴파일만 되는 빈 껍데기입니다. 담당자가 README 6.5장을 보고 내용을 채웁니다.
 *        함수 이름과 매개변수, 반환값은 stats.h 그대로 두세요.
 */
#include <string.h>
#include "stats.h"

void initWasteList(WasteList *w)
{
    w->items = 0;
    w->count = 0;
    w->capacity = 0;
}

void freeWasteList(WasteList *w)
{
    (void)w;
}

ResultCode appendWaste(WasteList *w, const WasteRecord *rec)
{
    (void)w; (void)rec;
    return RC_OK;
}

ResultCode addWaste(WasteList *w, const Food *item, Date today)
{
    (void)w; (void)item; (void)today;
    return RC_OK;
}

int discardExpired(Fridge *f, WasteList *w)
{
    (void)f; (void)w;
    return 0;
}

void computeWasteStats(const WasteList *w, Date today, WasteStats *out)
{
    (void)w; (void)today;
    memset(out, 0, sizeof(*out));
}
