/* fileio.h
 * 담당: 윤종하
 * 설명: 식재료·레시피·폐기 기록 파일 저장·불러오기
 *       loadXxx: 파일이 없으면 RC_OK (컨테이너는 비워 둔다), 형식이 틀린 줄은 건너뛴다.
 *       saveXxx: 파일을 열 수 없으면 RC_FILE_ERROR.
 */
#ifndef FILEIO_H
#define FILEIO_H

#include "common.h"

ResultCode loadFridge(const char *path, Fridge *f);
ResultCode saveFridge(const char *path, const Fridge *f);
ResultCode loadRecipes(const char *path, RecipeBook *rb);
ResultCode saveRecipes(const char *path, const RecipeBook *rb);
ResultCode loadWaste(const char *path, WasteList *w);
ResultCode saveWaste(const char *path, const WasteList *w);

#endif
