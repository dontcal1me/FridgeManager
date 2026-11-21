/* util.c
 * 담당: 장태현
 * 설명: 잘못된 입력에도 멈추지 않는 콘솔 입력 함수
 *
 * [뼈대] 컴파일만 되는 빈 껍데기입니다. 담당자가 README 6.2장을 보고 내용을 채웁니다.
 *        함수 이름과 매개변수, 반환값은 util.h 그대로 두세요.
 *        (inputInt만 뼈대 메뉴가 돌아가도록 README의 예시 코드를 넣어 두었습니다.)
 */
#include <stdio.h>
#include <conio.h>
#include "util.h"

void clearInputBuffer(void)
{
}

int inputInt(const char *msg, int min, int max)
{
    char line[64];
    int value;
    while (1) {
        printf("%s", msg);
        if (fgets(line, sizeof(line), stdin) == NULL)
            return min;                                  /* 입력 끝(EOF) 보호 */
        if (sscanf(line, "%d", &value) == 1 && value >= min && value <= max)
            return value;
        printf("%d ~ %d 사이의 숫자를 입력하세요.\n", min, max);
    }
}

int inputDate(const char *msg, Date *d)
{
    (void)msg; (void)d;
    return 0;
}

void inputWord(const char *msg, char *buf, int bufLen)
{
    (void)msg;
    if (bufLen > 0) buf[0] = '\0';
}

int inputYesNo(const char *msg)
{
    (void)msg;
    return 0;
}

void waitForKey(void)
{
    (void)_getch();
}
