/* stats.c
 * 담당: 박지연
 * 설명: 폐기 기록·일괄 폐기·통계 (F6)
 */
#include <stdlib.h>
#include <string.h>

#include "stats.h"

/* TODO(박지연): 구현 전 뼈대. 컴파일만 되는 빈 껍데기입니다. */

void initWasteList(WasteList *w) { (void)w; }
void freeWasteList(WasteList *w) { (void)w; }
ResultCode appendWaste(WasteList *w, const WasteRecord *rec) { (void)w; (void)rec; return RC_OK; }
ResultCode addWaste(WasteList *w, const Food *item, Date today) { (void)w; (void)item; (void)today; return RC_OK; }
int discardExpired(Fridge *f, WasteList *w) { (void)f; (void)w; return 0; }
void computeWasteStats(const WasteList *w, Date today, WasteStats *out) { (void)w; (void)today; if (out) memset(out, 0, sizeof(*out)); }