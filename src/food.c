/* food.c
 * 담당: 박지연
 * 설명: 식재료 등록·수정·삭제·수량 변경·검색 (F1, F5)
 *
 * [뼈대] 컴파일만 되는 빈 껍데기입니다. 담당자가 README 6.5장을 보고 내용을 채웁니다.
 *        함수 이름과 매개변수, 반환값은 food.h 그대로 두세요.
 */
#include "food.h"

void initFridge(Fridge *f)
{
    f->items = 0;
    f->count = 0;
    f->capacity = 0;
}

void freeFridge(Fridge *f)
{
    (void)f;
}

ResultCode addFood(Fridge *f, const Food *item)
{
    (void)f; (void)item;
    return RC_OK;
}

ResultCode updateFood(Fridge *f, int idx, const Food *item)
{
    (void)f; (void)idx; (void)item;
    return RC_OK;
}

ResultCode removeFood(Fridge *f, int idx)
{
    (void)f; (void)idx;
    return RC_OK;
}

ResultCode useFood(Fridge *f, int idx, int amount)
{
    (void)f; (void)idx; (void)amount;
    return RC_OK;
}

int findFood(const Fridge *f, const char *name)
{
    (void)f; (void)name;
    return -1;
}
