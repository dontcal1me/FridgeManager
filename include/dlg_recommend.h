/* dlg_recommend.h
 * 담당: 윤종하
 * 설명: 요리 추천 대화상자 (G4) + 조리법 자동 불러오기 (도전 목표, 식품안전나라 API)
 */
#ifndef DLG_RECOMMEND_H
#define DLG_RECOMMEND_H

#include "gui_common.h"

/* 추천 목록을 보여 주고, 요리하기(재료 자동 차감)·레시피 추가·조리법 불러오기를 처리한다 */
void recommendDialog(HWND owner);

#endif
