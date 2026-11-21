/* stats.h
 * 담당: 박지연
 * 설명: 폐기 기록, 일괄 폐기, 통계 (F6)
 *       함수 선언은 README 6.5장과 같다. 윤종하가 확정했으므로 바꾸지 않는다.
 */
#ifndef STATS_H
#define STATS_H

#include "common.h"

void       initWasteList(WasteList *w);                                  /* capacity 4로 동적 배열 생성 */
void       freeWasteList(WasteList *w);
ResultCode appendWaste(WasteList *w, const WasteRecord *rec);            /* 기록을 그대로 추가 (파일 불러오기용) */
ResultCode addWaste(WasteList *w, const Food *item, Date today);         /* F6: 재료를 폐기 기록으로 추가 */
int        discardExpired(Fridge *f, WasteList *w);                      /* F6: 기한 지난 재료 일괄 폐기, 폐기 개수 반환 */
void       computeWasteStats(const WasteList *w, Date today, WasteStats *out);   /* F6: 통계 계산 */

#endif
