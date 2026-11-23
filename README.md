# 냉장고 식재료 관리 프로그램 (Fridge Manager)

> 냉장고 속 식재료의 유통기한을 관리하고, 먼저 먹어야 할 재료를 알려 주며, 남은 재료로 만들 수 있는 요리를 추천하는 C언어 프로그램
> **GUI 버전(필수)**으로 완성하고, **도전 목표**로 식품안전나라 API에서 **조리 방법을 자동으로 불러옵니다.**

- 과목: 비판적사고 (홍서빈 교수님)
- 팀명: TEAM 1 (윤종하, 장태현, 김기범, 최민솔, 박지연)
- 개발 언어: C
- 개발 환경: Visual Studio / Windows 10 이상
- GUI: Win32 API (Windows 기본 제공, 추가 라이브러리 설치 없음)
- 저장소: https://github.com/dontcal1me/FridgeManager

> **방향 변경 (2026-11-23):** 콘솔 버전을 제거하고 **GUI 버전을 기본(필수) 버전**으로 정했습니다. 도전 목표는 **API로 조리 방법을 자동으로 불러오는 기능**입니다.
> 역할은 **GUI 버전 기준으로 다시 나눴습니다** (2.1장, 6장). 장태현은 가장 작고 쉬운 `dlg_simple` 한 쌍을 맡습니다. 15.1장은 콘솔 시절 화면 예시로, 설계 당시 참고용입니다.
> 제거한 콘솔 파일은 `FridgeManager_콘솔버전_백업` 폴더에 보관했습니다.

이 문서는 팀원들이 **각자 맡은 파일을 따로 개발하고, 나중에 문제없이 합칠 수 있도록** 정리한 작업 안내서입니다.
작업을 시작하기 전에 2장부터 5장까지는 꼭 읽고, 6장에서 **내 이름이 붙은 절**을 찾아 그대로 따라 하세요.

| 나는 | 내 담당 | 먼저 볼 곳 | 가장 먼저 할 일 |
|---|---|---|---|
| 윤종하 (팀장) | `common.h`, `fileio`, `guide`, `guide_api`, `apirec`, GUI 공통·메인 창(`main_gui`, `ui_gui`, `gui_common`, `gui_dialog`), `dlg_recommend` | [6.1](#61-윤종하-팀장) | 11/23까지 뼈대 코드 업로드 |
| 장태현 | `dlg_simple` (가장 작은 파일 1쌍) + 최종 발표 | [6.2](#62-장태현-dlg_simple) | 11/29까지 `dlg_simple` PR |
| 김기범 | `expiry`, `dlg_food` + 흐름도 | [6.3](#63-김기범-expiry-dlg_food) | 11/25까지 `getToday`, `isValidDate`, `daysLeft` PR |
| 최민솔 (부팀장) | `recipe`, `dlg_recipe`, `recipes.txt` | [6.4](#64-최민솔-recipe-dlg_recipe) | 11/25까지 `initRecipeBook`, `addRecipe`, `freeRecipeBook` PR |
| 박지연 | `food`, `stats` + 테스트 | [6.5](#65-박지연-food-stats) | 11/25까지 `initFridge`, `addFood`, `freeFridge`와 `initWasteList`, `appendWaste`, `addWaste`, `freeWasteList` PR |

---

## 목차

1. [프로젝트 소개](#1-프로젝트-소개)
2. [팀원별 담당과 마감](#2-팀원별-담당과-마감)
3. [처음 한 번만 하는 준비](#3-처음-한-번만-하는-준비)
4. [모두가 지켜야 할 약속](#4-모두가-지켜야-할-약속)
5. [매일 작업하는 순서 (Git)](#5-매일-작업하는-순서-git)
6. [팀원별 작업 안내](#6-팀원별-작업-안내)
7. [프로젝트 목표](#7-프로젝트-목표)
8. [주요 기능](#8-주요-기능)
9. [설계 원칙: 로직과 화면 분리](#9-설계-원칙-로직과-화면-분리)
10. [모듈 구조](#10-모듈-구조)
11. [핵심 자료구조](#11-핵심-자료구조)
12. [데이터 파일 형식](#12-데이터-파일-형식)
13. [개발 환경 설정](#13-개발-환경-설정)
14. [빌드 및 실행 방법](#14-빌드-및-실행-방법)
15. [실행 화면 예시](#15-실행-화면-예시)
16. [자주 막히는 문제](#16-자주-막히는-문제)
17. [완료 체크리스트](#17-완료-체크리스트)
18. [참고 자료](#18-참고-자료)

---

## 1. 프로젝트 소개

냉장고에 무엇이 있는지, 언제까지 먹어야 하는지 잊어버려서 음식을 버리는 일이 자주 생깁니다.
이 프로그램은 식재료를 등록해 두면 **오늘 날짜 기준으로 남은 일수를 자동 계산**하고,
**유통기한이 임박한 재료를 먼저 알려 주며**, **그 재료로 만들 수 있는 요리를 추천**합니다.
또한 버린 식재료를 기록하고 통계로 보여 주어, 사용자가 자신의 소비 습관을 데이터로 돌아볼 수 있도록 돕습니다.

---

## 2. 팀원별 담당과 마감

### 2.1 역할 분담

모두가 **자기 담당 파일을 직접 개발·확인하고, 커밋·푸시하고, Pull Request를 올립니다.** 이 프로그램은 **GUI 버전 하나**이며, 파일 양은 사람마다 다르게 나눴습니다. 발표를 맡은 장태현은 **가장 작고 쉬운 파일 1쌍(`dlg_simple`)만** 맡습니다.

| 이름 | 역할 | 담당 파일 (`include/`의 `.h`와 `src/`의 `.c` 쌍) | 개발 외 업무 | 브랜치 |
|---|---|---|---|---|
| 윤종하 | 팀장 · 통합 · 일정 · 문서 | `common.h`, `fileio`, `guide`, `guide_api`(도전), `apirec`(도전), `main_gui.c`, `ui_gui`(메인 창), `gui_common`, `gui_dialog`, `dlg_recommend`(요리 추천 창 + API 추천·조리법 자동 불러오기) | GitHub 관리·병합, 회의록·WBS, 제안·결과보고서, README, 발표 PPT | `feature/skeleton`, `feature/gui`, `feature/fileio`, `feature/guide` |
| 장태현 | 구현(가장 작은 파일) · 발표 | `dlg_simple` (수량 입력 창, 글 보기 창) | **최종 발표·데모 시연**, 예상 질문과 답변 정리 | `feature/dlg-simple` |
| 김기범 | 구현 | `expiry` (F2, F4), `dlg_food` (식재료 등록·수정 창) | 프로그램 흐름도 | `feature/expiry`, `feature/dlg-food` |
| 최민솔 | 부팀장 · 구현 | `recipe` (F3, 자동 차감), `dlg_recipe` (레시피 추가 창), `data/recipes.txt` (20개 이상) | PR 검토 대행, 데모용 샘플 데이터 준비 | `feature/recipe`, `feature/dlg-recipe` |
| 박지연 | 구현 · 테스트 | `food` (F1, F5), `stats` (F6) | 테스트 시나리오·결과서 | `feature/food`, `feature/stats` |

담당 파일의 규모(대략): 장태현 약 90줄 / 김기범 약 190줄 / 박지연 약 200줄 / 최민솔 약 300줄. 나머지(공통 도우미, 메인 창, 파일 입출력, API 연동, 요리 추천 창)는 팀장이 맡아 통합합니다.

**자기 담당 파일만 수정합니다.** 다른 사람 파일을 고쳐야 하면 먼저 단톡방에 말하고, 그 사람이 고치거나 허락을 받은 뒤 고칩니다.

| 공유 파일 | 규칙 |
|---|---|
| `common.h`, 모든 `.h` 함수 선언 | 확정 후에는 **윤종하만** 수정. 바꾸고 싶으면 단톡방에 먼저 제안 |
| `FridgeManager.slnx`, `FridgeGUI.vcxproj`, `FridgeGUI.vcxproj.filters` | 윤종하만 수정. 파일 추가가 필요하면 요청 |
| `main_gui.c`, `ui_gui`, `gui_common`, `gui_dialog`, `dlg_recommend` | 윤종하만 수정 |

### 2.2 전체 일정 (WBS)

| 구분 | 기간 | 주요 작업 | 산출물 | 담당 |
|---|---|---|---|---|
| 1. 착수 및 계획 | 11/09 ~ 11/15 | 주제 선정, 역할 확정, 제안보고서, GitHub 개설 | 제안보고서 | 윤종하 |
| 2. 분석 및 설계 | 11/16 ~ 11/22 | `common.h`·헤더 확정, 기능 명세, 흐름도 | 기능 명세서, 흐름도 | 전원 |
| 3. 구현 | 11/23 ~ 12/06 | 모듈별 코딩, 주간 코드 리뷰·병합 | 소스 코드 v0.x | 전원 |
| 4. 통합 및 테스트 | 12/07 ~ 12/13 | 통합·디버깅, **v1.0 태그** | 테스트 결과서, v1.0 | 윤종하 |
| 5. 납품 및 발표 | 12/14 ~ 발표일 | 결과보고서, 발표자료, 데모 시연 | 결과보고서, 발표자료 | 윤종하 |

### 2.3 주차별 계획 (마감)

| 주차 | 윤종하 | 장태현 | 김기범 | 최민솔 | 박지연 |
|---|---|---|---|---|---|
| **설계 주**<br>11/16~11/22 | 제안 발표(11/16), README·`common.h` 초안 공유, 회의록·WBS | 개발 환경 준비, 6.2 읽기 | 개발 환경 준비, 프로그램 흐름도 작성 | 개발 환경 준비, 기본 레시피 20개 목록 초안 | 개발 환경 준비, 6.5 읽기 |
| **구현 1주차**<br>11/23~11/29 | **11/23(월) 뼈대 코드 업로드** (모든 `.h` 완성, `.c`는 빈 껍데기, 빈 메인 창이 뜨고 빌드 통과), `gui_common`·`gui_dialog` | **11/29(일)** `dlg_simple` PR | **11/25(수)** `getToday`·`isValidDate`·`daysLeft` PR<br>**11/29(일)** `expiry.c` 전체 PR | **11/25(수)** `initRecipeBook`·`addRecipe`·`freeRecipeBook` PR, `recipes.txt` 작성 | **11/25(수)** `initFridge`·`addFood`·`freeFridge`, `initWasteList`·`appendWaste`·`addWaste`·`freeWasteList` PR<br>**11/29(일)** `food.c` 전체 PR |
| **구현 2주차**<br>11/30~12/06 | PR 검토·병합, `fileio`, 메인 창(`ui_gui`), `dlg_recommend`, `guide`·`guide_api`, 연결 확인 | 발표 시연 순서 정리 | **12/03(목)** `dlg_food` PR, 흐름도 최종 | **12/03(목)** `recommend`·`cookRecipe` PR, `dlg_recipe` PR, `recipes.txt` 20개 완성 | **12/03(목)** `stats.c` 전체 PR, 확인 시나리오 작성<br>**12/06(일)** 전원 병합 완료, GUI 실행 가능 |
| **통합 주**<br>12/07~12/13 | 통합·디버깅, **12/13(일) v1.0 태그** | 시연 리허설, 예상 질문 정리 | 버그 수정, 자기 코드 설명 연습 | 버그 수정, 데모 샘플 데이터 | 통합 테스트 실행, 버그를 Issues에 등록, 테스트 결과서 |
| **납품 주**<br>12/14~ | 결과보고서 취합, 발표 PPT, API 조리법 자동 불러오기 시연 준비 | **최종 발표·데모 시연** | 보고서에 자기 모듈 설명 전달 | 보고서에 자기 모듈 설명 전달 | 테스트 결과서 최종, 보고서에 자기 모듈 설명 전달 |

팀원이 맡은 로직 모듈(`food`, `expiry`, `recipe`, `stats`)은 화면과 분리되어 있어서, 화면 파일이 바뀌어도 **그대로** 쓰입니다.

### 2.4 왜 11/25에 먼저 올리나요?

다른 사람의 함수를 부르는 모듈이 있기 때문입니다. 아래 함수는 **가장 먼저** 올려서 다른 팀원이 막히지 않게 합니다.

| 먼저 올릴 함수 | 담당 | 이 함수를 쓰는 사람 |
|---|---|---|
| `initFridge`, `addFood`, `freeFridge` | 박지연 | 윤종하(`loadFridge`, 메인 창), 김기범(`sortByExpiry`가 냉장고 구조체 사용), 최민솔(`recommend`, `cookRecipe`) |
| `initWasteList`, `appendWaste`, `addWaste`, `freeWasteList` | 박지연 | 윤종하(`loadWaste`, 메인 창) |
| `getToday`, `daysLeft`, `isValidDate` | 김기범 | 김기범(`dlg_food` 날짜 검사), 최민솔(`recommend`), 박지연(`discardExpired`), 윤종하(메인 창) |
| `initRecipeBook`, `addRecipe`, `freeRecipeBook` | 최민솔 | 최민솔(`dlg_recipe`), 윤종하(`loadRecipes`, 메인 창, `dlg_recommend`) |

급하게 필요한데 아직 안 올라왔으면 단톡방에 말하세요. 기다리는 동안은 내 쪽 함수 안에서 그 함수를 호출하는 줄만 주석 처리하고 나머지부터 만듭니다.

---

## 3. 처음 한 번만 하는 준비

### 3.1 설치
- **Visual Studio** (Community): 설치 화면의 워크로드에서 **"C++를 사용한 데스크톱 개발"** 선택
- **Git**

### 3.2 저장소 받기

팀장이 보낸 GitHub 초대 메일에서 **Accept invitation**을 누른 뒤, 터미널에서 실행합니다.

```bash
git clone https://github.com/dontcal1me/FridgeManager.git
cd FridgeManager
```

### 3.3 Git 사용자 설정 (컴퓨터마다 한 번)

```bash
git config --global user.name "본인 이름"
git config --global user.email "GitHub에 가입한 이메일"
```

### 3.4 열고 실행해 보기

1. `FridgeManager.slnx`를 Visual Studio로 엽니다.
2. `Ctrl + F5` (디버깅하지 않고 시작)로 실행합니다.
3. 팀장이 뼈대 코드를 올리기 전에는 빈 파일이라 빌드되지 않을 수 있습니다. 뼈대가 올라온 뒤 `git pull` 하고 오류 없이 실행되면 준비 완료입니다.

`v145 플랫폼 도구 집합을 찾을 수 없다`는 오류가 나면 16장을 보세요.

### 3.5 내 컴퓨터에서 확인할 설정

| 항목 | 확인 방법 |
|---|---|
| 작업 디렉터리 | 프로젝트 속성 → 디버깅 → 작업 디렉터리가 `$(SolutionDir)`인지 (팀장이 프로젝트에 설정해 둠) |
| 보안 경고 해제 | 프로젝트 속성 → C/C++ → 전처리기에 `_CRT_SECURE_NO_WARNINGS`가 있는지 (팀장이 설정) |
| 소스 인코딩 | 새 `.c`/`.h` 파일은 **UTF-8(서명 있음)**. 파일 → 다른 이름으로 저장 → 인코딩하여 저장 |
| 확장자 | 소스는 반드시 `.c`. `.cpp`로 만들면 C++로 컴파일됩니다 |

---

## 4. 모두가 지켜야 할 약속

따로 만든 코드가 합쳐지려면 아래 약속을 **모두 똑같이** 지켜야 합니다. 바꾸고 싶으면 혼자 바꾸지 말고 팀장과 먼저 상의합니다.

### 4.1 로직과 화면을 섞지 않는다

| 계층 | 파일 | 해도 되는 것 | 하면 안 되는 것 |
|---|---|---|---|
| 로직 | `food`, `expiry`, `recipe`, `stats`, `fileio`, `guide` | 계산, 구조체 수정, 파일 읽기·쓰기 | 창·대화상자·`MessageBox` 등 Win32 화면 API, `printf`, `scanf` |
| 화면 | `main_gui`, `ui_gui`, `gui_*`, `dlg_*` | 창·입력칸·버튼 처리, 로직 함수 호출 | 구조체 내부(`f->items[i].quantity = ...`)를 직접 고치기 |

(`guide_api`는 인터넷에서 조리법을 받아 오는 모듈이라 통신용 WinHTTP를 쓰지만, 화면 입출력은 하지 않습니다.)

값은 **매개변수**로 받고, 결과는 **반환값이나 포인터**로 돌려줍니다. 어떤 메시지를 보여 줄지는 화면이 정합니다. (파일 읽기·쓰기를 위한 `fopen`은 `fileio`에서 사용해도 됩니다.)

### 4.2 데이터 표현 약속

| 항목 | 약속 |
|---|---|
| 식재료 위치 | `Fridge.items` 배열의 인덱스 `idx` (0부터). 메인 창 표의 행 번호(0부터)가 곧 `idx` |
| 날짜 | `Date` 구조체 `{year, month, day}`. 문자열로는 `YYYY-MM-DD` |
| D-day | `daysLeft = 유통기한 - 오늘`. **양수 = 남음, 0 = 오늘까지, 음수 = 기한 지남** |
| 기한 지남 | `daysLeft < 0` |
| 임박 | `0 <= daysLeft <= ALERT_DAYS` (ALERT_DAYS = 2, 오늘 포함) |
| 보관 방법 | `FRIDGE = 0` (냉장), `FREEZER = 1` (냉동) |
| 이름 | **공백 없는 문자열**. `NAME_LEN` 30 → 한글은 **최대 14자** (한글 1자 = 2바이트) |
| 성공·실패 | `ResultCode` 반환. 6장에 적힌 경우에 어떤 코드를 반환하는지 그대로 지킵니다 |
| 잘못된 인자 | 포인터가 `NULL`이거나 인덱스가 범위 밖이면 죽지 말고 오류 코드 반환 |
| 동적 배열 | `initXxx` → 사용 → `freeXxx`. 가득 차면 `realloc`으로 **2배** 확장, 처음 `capacity`는 4 |

### 4.3 공통 상수는 `common.h`에서만

경로, 길이 같은 값은 각자 파일에 직접 쓰지 말고 `common.h`의 상수를 씁니다. (11장의 `common.h`에 아래 경로 상수가 포함됩니다.)

```c
#define FRIDGE_FILE  "data/fridge.txt"
#define RECIPE_FILE  "data/recipes.txt"
#define WASTE_FILE   "data/waste.txt"
```

### 4.4 함수 이름과 반환값

6장에 적힌 **함수 이름, 매개변수, 반환값**은 그대로 지킵니다. 다른 팀원이 그 이름으로 부르고 있기 때문에, 이름 하나만 달라도 합칠 때 빌드가 깨집니다. 내부 구현 방법은 자유입니다.

### 4.5 코딩 규칙

- 함수 이름은 `addFood`처럼 camelCase, 상수는 `MAX_ING`처럼 대문자, 구조체는 `Food`처럼 대문자로 시작
- 구조체를 함수에 넘길 때는 포인터를 쓰고, 안에서 수정하지 않으면 `const`를 붙이기
- `malloc`/`realloc` 뒤에는 반드시 `NULL` 검사, 프로그램 끝나기 전에 `free`
- `realloc`은 `f->items = realloc(f->items, ...)`처럼 바로 대입하지 말고 **임시 변수**에 받아서 검사
- 모든 함수 바로 위에 한 줄 이상 한글 설명 주석
- `printf`로 디버깅한 코드는 PR 올리기 전에 지우기
- 헤더 파일에는 `#ifndef`/`#define`/`#endif` 가드
- 파일 맨 위에 담당자 표시

```c
/* food.c
 * 담당: 박지연
 * 설명: 식재료 등록·수정·삭제·수량 변경·검색 (F1, F5)
 */
```

`main_gui.c`, `ui_gui.c`, `gui_common.c`, `gui_dialog.c`, `dlg_recommend.c`는 윤종하만 수정합니다.

### 4.6 인코딩 (한글 깨짐 방지)

| 파일 종류 | 저장 인코딩 |
|---|---|
| 소스 (`.c`, `.h`) | **UTF-8 (서명 있음)** |
| 데이터 (`data/*.txt`) | **ANSI (EUC-KR)**. 메모장 → 다른 이름으로 저장 → 인코딩 ANSI |
| `README.md` | UTF-8 |

프로젝트 속성에 `/utf-8` 옵션은 **추가하지 않습니다.** (화면의 한글 문자열이 깨질 수 있습니다.) 자세한 내용은 13.3장.

---

## 5. 매일 작업하는 순서 (Git)

### 5.1 작업 시작할 때

```bash
git checkout main              # main 브랜치로 이동
git pull origin main           # 다른 팀원이 올린 최신 코드 받기
git checkout -b feature/food   # 내 브랜치 만들기 (처음 한 번, 이름은 2.1의 브랜치 칸)
```

이미 만든 브랜치로 다시 작업할 때는 최신 내용을 내 브랜치에 합칩니다.

```bash
git checkout feature/food
git merge main
```

### 5.2 작업 중간중간 저장 (commit)

기능 하나가 끝날 때마다 저장합니다. 하루에 한 번 이상이 좋습니다.

```bash
git add src/food.c
git commit -m "[feat] addFood 동적 배열 확장 구현"
```

`git add .`처럼 전부 추가하지 말고 **내가 고친 파일 이름**을 적습니다.

커밋 메시지 형식: `[태그] 작업 내용`

| 태그 | 언제 |
|---|---|
| `[feat]` | 새 기능 |
| `[fix]` | 버그 수정 |
| `[docs]` | 문서 수정 |
| `[refactor]` | 동작은 같고 코드 정리 |
| `[test]` | 테스트 코드 |

### 5.3 GitHub에 올리기 (push)

```bash
git push origin feature/food
```

### 5.4 Pull Request 만들기

1. GitHub 저장소 페이지에 들어가면 노란 띠에 **Compare & pull request** 버튼이 뜹니다.
2. 제목은 커밋 메시지처럼 쓰고, 내용에 **무엇을 만들었는지, 어떻게 테스트했는지** 적습니다.
3. Reviewers에 **윤종하**를 지정합니다. 윤종하의 PR은 **최민솔**을 지정합니다.
4. 수정 요청이 오면 같은 브랜치에서 고쳐서 다시 push 하면 PR에 자동으로 반영됩니다.
5. 승인되면 팀장이 `main`에 병합합니다.

### 5.5 절대 하지 않을 것

- **`main` 브랜치에 직접 push 하지 않기**
- 다른 사람 담당 파일을 말없이 고치지 않기
- `git push --force` 쓰지 않기
- 빌드가 안 되거나 실행 중 죽는 코드를 PR로 올리지 않기 (최소한 내 담당 기능을 6장의 "혼자 확인하기" 순서대로 실행해 보고 올리기)
- `.vcxproj`, `.slnx` 파일을 임의로 고쳐서 커밋하지 않기

### 5.6 충돌(conflict)이 났을 때

`git merge main` 후 충돌 메시지가 나오면 Visual Studio나 VS Code에서 해당 파일을 엽니다. `<<<<<<<`, `=======`, `>>>>>>>` 표시가 있는 부분에서 맞는 쪽을 고른 뒤 표시를 모두 지우고, 저장하고 다시 커밋합니다. 잘 모르겠으면 아무것도 지우지 말고 팀장에게 바로 물어봅니다.

---

## 6. 팀원별 작업 안내

각 절에는 **만들어야 할 것(정확한 이름과 반환값)**, **구현 요점**, **완료 기준**, **혼자 확인하는 방법**이 있습니다. 담당 파일은 `include/`(헤더)와 `src/`(소스)에 있습니다.

### 6.0 시작하기 전에

#### 파일과 담당 (GUI 버전)

| 계층 | 파일 (`.h` + `.c`) | 하는 일 | 담당 |
|---|---|---|---|
| 로직 | `food` | 식재료 등록·수정·삭제·수량 변경·검색 (F1, F5) | 박지연 |
| 로직 | `stats` | 폐기 기록·일괄 폐기·통계 (F6) | 박지연 |
| 로직 | `expiry` | 날짜 계산·D-day·임박순 정렬·임박 개수 (F2, F4) | 김기범 |
| 로직 | `recipe` | 요리 추천·재료 자동 차감·레시피 추가 (F3, F5) | 최민솔 |
| 로직 | `fileio` | 데이터 파일 저장·불러오기 | 윤종하 |
| 로직 | guide, guide_api | 조리 순서 저장 / 인터넷(API)에서 불러오기 (도전) | 윤종하 |
| 로직 | pirec | 재료 **이름**으로 API 요리 추천, 양은 비교하지 않음 (도전) | 윤종하 |
| 화면 | `main_gui`, `ui_gui` | 진입점, 메인 창(식재료 표·기능 버튼), 시작 알림 | 윤종하 |
| 화면 | `gui_common`, `gui_dialog` | 공통 도우미, 대화상자 틀 | 윤종하 |
| 화면 | `dlg_simple` | 수량 입력 창, 글 보기(폐기 통계) 창 | **장태현** |
| 화면 | `dlg_food` | 식재료 등록·수정 창 | 김기범 |
| 화면 | `dlg_recipe` | 레시피 추가 창 | 최민솔 |
| 화면 | `dlg_recommend` | 요리 추천 창 + 조리법 자동 불러오기 | 윤종하 |

#### 의존 순서

```
common.h (윤종하, 전원 확정)
   │
   ├── food (박지연) ──────┐
   ├── expiry (김기범) ────┤
   ├── stats (박지연) ─────┼──> fileio (윤종하: addFood, addRecipe, appendWaste 사용)
   │                       │      └──> recipe (최민솔: expiry, food 사용)
   │
   ├── gui_common, gui_dialog (윤종하: 화면 공통 도우미·대화상자 틀)
   │      ├── dlg_simple (장태현: 다른 팀원 파일을 부르지 않음)
   │      ├── dlg_food (김기범: expiry의 날짜 검사 사용)
   │      ├── dlg_recipe (최민솔: recipe의 addRecipe 사용)
   │      └── dlg_recommend (윤종하: recipe, guide, guide_api 사용)
   │
   └── main_gui / ui_gui (윤종하: 위의 모든 모듈과 창을 연결)
```

`food`, `expiry`, `stats`, `dlg_simple`은 서로 독립적이라 **동시에** 개발할 수 있습니다. 2.4장의 "먼저 올릴 함수"만 11/25까지 올리면 나머지는 막힘 없이 진행됩니다.

#### 혼자 확인하는 방법

프로그램이 **GUI 하나**이므로 별도의 테스트 실행 방식(`--test`)은 쓰지 않습니다. 대신 각 절의 **"혼자 확인하기"** 순서대로 프로그램을 직접 실행해서 내 담당 기능을 확인합니다.

1. 솔루션 탐색기에서 `FridgeGUI`를 시작 프로젝트로 설정하고 `Ctrl + F5`로 실행합니다.
2. 내 담당 화면·기능을 시나리오대로 눌러 봅니다. **일부러 잘못된 값**도 넣어 봅니다.
3. 무엇을 눌러서 어떤 결과가 나왔는지를 **PR 본문**에 적습니다.
4. 데이터 파일(`data/*.txt`)이 바뀌었으면 커밋하지 말고 원래대로 되돌립니다.

> 콘솔 버전에 있던 `--test` 단위 테스트 코드(`test_*.c`)는 제거했습니다. 필요하면 `FridgeManager_콘솔버전_백업` 폴더에서 볼 수 있습니다.

---

### 6.1 윤종하 (팀장)

**담당:** `common.h`, `fileio`, `guide`, `guide_api`(도전), `apirec`(도전), `main_gui.c`, `ui_gui`, `gui_common`, `gui_dialog`, `dlg_recommend`, 프로젝트 설정
**브랜치:** `feature/skeleton`, `feature/gui`, `feature/fileio`, `feature/guide`

#### 11/23(월)까지: 뼈대 코드 업로드 (가장 중요)

팀원 전원이 이것을 받아서 시작합니다. 아래를 모두 갖춘 상태로 `main`에 올립니다.

- [ ] `common.h`: 11장 그대로 + 4.3장의 파일 경로 상수
- [ ] 로직 모듈의 `.h`(`food`, `expiry`, `recipe`, `stats`, `fileio`, `guide`): 6.3~6.5장에 적힌 함수 선언을 **그대로** 복사
- [ ] 각 로직 `.c`: 함수마다 컴파일만 되는 빈 껍데기 (반환값만 `return RC_OK;`, `return 0;` 등)
- [ ] `gui_common`, `gui_dialog`: 동작하는 공통 도우미와 대화상자 틀
- [ ] `ui_gui`, `main_gui`: 메인 창이 뜨고 버튼이 있는 뼈대 (각 버튼은 아직 "준비 중" 안내)
- [ ] `dlg_simple`, `dlg_food`, `dlg_recipe`, `dlg_recommend`의 `.h`와 빈 함수 (`return 0;`)
- [ ] `FridgeGUI.vcxproj`와 `.filters`: 위 파일 등록, `include`를 추가 포함 디렉터리로 지정, `_CRT_SECURE_NO_WARNINGS` 정의, 디버깅 작업 디렉터리를 `$(SolutionDir)`로 설정 (13.2장)
- [ ] `data/` 폴더: `fridge.txt`, `waste.txt` (샘플 몇 줄), `recipes.txt` (빈 파일. 최민솔이 채움), `steps.txt`. **ANSI로 저장**
- [ ] 빌드와 실행이 오류 없이 되는지 확인한 뒤 단톡방에 공지

#### main_gui.c

```c
int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE hPrev, PWSTR cmdLine, int nShow)
{
    AppData app;

    initFridge(&app.fridge);   initRecipeBook(&app.recipes);   initWasteList(&app.waste);
    loadFridge(FRIDGE_FILE, &app.fridge);
    loadRecipes(RECIPE_FILE, &app.recipes);
    loadWaste(WASTE_FILE, &app.waste);

    guiShowStartupAlert(&app.fridge);        /* F4: 시작 알림 팝업 */
    guiRun(hInst, &app, nShow);              /* 창을 닫을 때까지 */

    saveFridge(FRIDGE_FILE, &app.fridge);
    saveRecipes(RECIPE_FILE, &app.recipes);
    saveWaste(WASTE_FILE, &app.waste);

    freeFridge(&app.fridge);   freeRecipeBook(&app.recipes);   freeWasteList(&app.waste);
    return 0;
}
```

`data/` 폴더를 못 찾으면 실행 파일 위치에서 위쪽 폴더로 올라가며 찾습니다. (`.exe`를 직접 실행해도 동작)

#### 메인 창 (ui_gui.c)

식재료 표(임박순 정렬, 기한 지남 = 빨강, 임박 = 주황)와 오른쪽 기능 버튼으로 구성됩니다. 각 버튼이 부르는 담당자 파일은 아래와 같습니다.

| 버튼 | 동작 | 부르는 것 |
|---|---|---|
| 식재료 등록 | 등록 창 → 같은 재료(이름·유통기한·보관 방법 동일)면 수량을 합치고 안내 | `dlg_food`(김기범) → `addFood`(박지연) |
| 수정 | 선택한 행을 수정 창으로 수정, 다른 재료와 같아지면 합침 | `dlg_food` → `updateFood` |
| 수량 변경 | 숫자 입력 창 → 줄이면 `useFood`, 늘리면 `updateFood` | `dlg_simple`(장태현) |
| 삭제 | 삭제 확인 → "폐기로 기록할까요?" → 삭제 | `addWaste`, `removeFood`(박지연) |
| 요리 추천 | 추천 창 (요리하기·레시피 추가·조리법 불러오기) | `dlg_recommend` |
| 레시피 추가 | 레시피 추가 창 | `dlg_recipe`(최민솔) |
| 기한 지난 재료 폐기 | 확인 후 일괄 폐기 | `discardExpired`(박지연) |
| 폐기 통계 | 통계 창 | `computeWasteStats`(박지연), `textDialog`(장태현) |

#### 화면 공통 (gui_common, gui_dialog)

- `gui_common.h`: 문자 변환(`ansiToWide`, `wideToAnsi`), 메시지 창(`msg`, `ask`, `bad`), 컨트롤 만들기(`mk`), 표 도우미(`lvAdd` 등), 목록 갱신(`refreshMain`)
- `gui_dialog.h`: 모든 대화상자가 쓰는 틀(`DlgBase`, `runDialog`). 팀원은 이 틀에 맞춰 자기 `dlg_*` 파일을 만듭니다.

#### 조리법 자동 불러오기 (guide, guide_api, dlg_recommend) — 도전 목표

```c
/* guide.h : 조리 순서 저장·조회 (data/steps.txt) */
int        getRecipeSteps(const char *recipeName, char steps[][STEP_LEN], int max);
ResultCode addRecipeSteps(const char *recipeName, char steps[][STEP_LEN], int count);
ResultCode replaceRecipeSteps(const char *recipeName, char steps[][STEP_LEN], int count);

/* guide_api.h : 식품안전나라 API (인증키: data/api_key.txt 또는 환경변수) */
int       hasApiKey(void);
ApiResult fetchRecipeSteps(const char *recipeName, char steps[][STEP_LEN], int max,
                           int *outCount, char *matchedName, int matchedLen);
```

요리 추천 창에서 요리를 고르면 조리 순서가 없을 때 자동으로 `fetchRecipeSteps`를 호출합니다. 호출은 별도 스레드에서 하고, 받은 결과는 `replaceRecipeSteps`로 저장합니다. 설정은 14.1장입니다.

#### API 요리 추천 (apirec) — 도전 목표

요리 추천을 **인터넷 레시피(API)로** 합니다. 재료의 양·개수는 보지 않고, **내 재료 이름이 요리의 재료 정보에 나오는지만** 대조합니다.

```c
/* apirec.h */
int       collectIngredients(const Fridge *f, IngredientRef *out, int max);   /* 기한이 지나지 않은 재료 이름 (임박 먼저) */
ApiResult apiRecommend(const IngredientRef *ings, int n,
                       ApiRecommendation *out, int maxOut, int *outCount);    /* API로 찾고 이름 대조, 점수순 */

/* guide_api.h */
ApiResult fetchRecipesByIngredient(const char *ingredient, ApiRecipe *out, int max, int *outCount);
```

| 단계 | 내용 |
|---|---|
| 1. 질의 | 내 재료 이름마다 API에 "이 재료가 들어간 요리"를 묻는다 (임박 재료 먼저, 최대 8회). `계란`은 `달걀`로도 묻는다 |
| 2. 합치기 | 받은 요리를 모아 같은 이름은 하나로 합친다 |
| 3. 이름 대조 | 요리의 **재료 정보 글**에 내 재료 이름이 나오는지 센다 (양·개수는 보지 않음, `달걀`=`계란`) |
| 4. 점수 | 나온 재료 수 × 10 + 그중 임박 재료 수 × 5. 점수 높은 순, 최대 30개 |

- 요리 이름·재료 정보·조리 순서가 **한 번의 응답**에 같이 오므로 조리법을 따로 부르지 않습니다.
- 인터넷 호출은 별도 스레드에서 하고, 냉장고는 창에서 이름만 복사해 넘깁니다. (스레드는 냉장고를 직접 읽지 않음)
- 인증키가 있으면 요리 추천 창이 **인터넷 추천**으로 열리고, 없거나 실패하면 저장된 레시피(`recipe`)로 추천합니다. `[저장 레시피로 보기]` / `[인터넷 추천 보기]` 버튼으로 오갈 수 있습니다.
- 인터넷 추천은 양을 비교하지 않으므로 **재료 자동 차감(`요리하기`)은 저장된 레시피 추천에서만** 쓸 수 있습니다.

#### fileio (파일 저장·불러오기)

##### fileio.h

```c
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
```

| 함수 | 동작과 반환값 |
|---|---|
| `loadXxx` | 컨테이너는 **호출하는 쪽이 이미 `initXxx` 해 둔 상태**. 파일이 **없으면 `RC_OK`** 를 반환하고 컨테이너는 비워 둔다 (프로그램을 처음 켜는 경우). 형식이 잘못된 줄은 **건너뛴다**. 읽은 항목은 `addFood`, `addRecipe`, `appendWaste`로 추가한다 |
| `saveXxx` | 파일을 새로 만들어(`"w"`) 전부 쓴다. 파일을 열 수 없으면 `RC_FILE_ERROR` |

파일 형식은 12장과 같습니다. 줄 하나 읽는 예:

```c
/* fridge.txt 한 줄: 이름 유통기한 수량 보관 */
Food item;
while (fscanf(fp, "%29s %d-%d-%d %d %d",
              item.name, &item.expire.year, &item.expire.month, &item.expire.day,
              &item.quantity, (int *)&item.storage) == 6) {
    addFood(f, &item);
}
```

`recipes.txt`는 한 줄의 길이가 다르므로 `이름`, `사용자추가여부`, `재료개수`를 먼저 읽고, 재료 개수만큼 `이름 수량` 쌍을 반복해서 읽습니다. 재료 개수가 `MAX_ING`보다 크면 그 줄은 건너뜁니다. `(int *)&item.storage`가 어렵다면 `int s;`로 읽은 뒤 `item.storage = (Storage)s;`로 바꿔도 됩니다.

##### fileio 완료 기준
- 저장한 뒤 다시 불러오면 항목 수와 값이 저장 전과 똑같다 (재료, 레시피, 폐기 기록 모두)
- 데이터 파일이 없어도 프로그램이 정상 시작한다
- 한글 이름이 저장·불러오기 후에도 깨지지 않는다 (파일이 **ANSI**여야 함)

#### 6.1 완료 기준
- 메인 창이 뜨고, 모든 버튼이 오류 없이 동작하고 돌아온다
- 창을 닫으면 세 파일이 저장되고 메모리가 해제된 뒤 종료된다
- 모든 PR을 4.x장 약속 기준으로 검토한다: 로직에 화면 코드 없음, 담당자 표시, "혼자 확인하기" 결과가 PR에 적혀 있음, `NULL` 검사

#### 개발 외 업무
- GitHub 저장소 관리, 팀원 Collaborator 초대, PR 검토·`main` 병합, 버전 태그(v1.0)
- 주간 회의 진행, 회의록, WBS 일정 관리
- 제안보고서·결과보고서 취합 (각자 맡은 파일 설명을 받아서 정리), README 관리, 발표 PPT

---

### 6.2 장태현 (dlg_simple)

**담당:** `include/dlg_simple.h`, `src/dlg_simple.c`
**브랜치:** `feature/dlg-simple`

장태현은 **최종 발표**를 맡고 있어서 **가장 작고 쉬운 파일 1쌍**만 맡습니다. 입력칸 하나짜리 작은 창 두 개로 이루어져 있고, **다른 팀원의 파일을 부르지 않아서** 기다릴 필요가 없습니다.

#### dlg_simple.h

```c
#ifndef DLG_SIMPLE_H
#define DLG_SIMPLE_H
#include "gui_common.h"

int  promptNumber(HWND owner, const wchar_t *title, const wchar_t *label,
                  int initial, int min, int max, int *out);   /* 숫자 한 개 입력 */
void textDialog(HWND owner, const wchar_t *title, const wchar_t *text);   /* 여러 줄 글 보기 */

#endif
```

| 함수 | 쓰이는 곳 | 동작과 반환값 |
|---|---|---|
| `promptNumber` | 메인 창 [수량 변경] | 숫자 입력칸 하나와 확인·취소 버튼. 처음에 `initial`을 보여 주고 `min`~`max` 숫자만 받는다. 확인하면 `*out`에 값을 넣고 **1**, 취소하거나 닫으면 **0** |
| `textDialog` | 메인 창 [폐기 통계] | 글을 보여 주는 창. 닫기 버튼 하나. 반환값 없음 |

#### 구현 요점

- 대화상자는 `gui_dialog.h`의 틀을 씁니다. 구조체 **맨 앞에 `DlgBase`**를 두고, `onCreate`(컨트롤 만들기)와 `onCommand`(버튼 처리)를 연결한 뒤 `runDialog`로 띄웁니다.
- 컨트롤은 `gui_common.h`의 `mk()`로 만듭니다. 입력이 잘못되면 `bad()`로 안내하고 입력칸으로 커서를 돌려 줍니다. **이때 창은 닫지 않습니다.**
- 숫자는 `GetDlgItemInt`로 읽고 범위를 검사합니다. 입력칸에 `ES_NUMBER` 스타일을 주면 숫자만 칠 수 있습니다.
- Enter는 확인(`IDOK`), Esc는 취소(`IDCANCEL`)로 동작해야 합니다.

```c
/* 구조체 맨 앞에 DlgBase를 둔다 */
typedef struct { DlgBase b; const wchar_t *label; int initial, min, max; int value; int ok; } NumDlg;

static void numCommand(HWND h, DlgBase *base, int id, int code)
{
    NumDlg *d = (NumDlg *)base;
    if (id == IDCANCEL) { d->b.done = 1; return; }          /* 창 닫기 */
    if (id == IDOK) {
        ...   /* 입력값을 읽고 범위 검사: 틀리면 bad(...) 후 return, 맞으면 d->ok = 1; d->b.done = 1; */
    }
}
```

#### 완료 기준
- `promptNumber`에 빈칸, 범위 밖 숫자를 넣으면 안내만 나오고 창이 닫히지 않는다
- 확인하면 1을 반환하고 `*out`에 입력값이 들어간다. 취소·Esc·닫기(X)는 0을 반환하고 `*out`은 바뀌지 않는다
- 창이 열려 있는 동안 뒤의 메인 창은 눌러지지 않는다 (`runDialog`가 처리)
- `textDialog`는 닫기 버튼과 Enter·Esc로 닫힌다

#### 혼자 확인하기

1. 프로그램을 실행하고 표에서 재료 하나를 선택한 뒤 **수량 변경**을 누른다
2. 입력칸을 비우고 확인 → 안내가 나오고 창이 유지되는지
3. `0`, `1000`을 넣고 확인 → 1~999 범위 안내가 나오는지
4. `5`를 넣고 확인 → 표의 수량이 5로 바뀌는지. 다시 열어서 **취소**하면 바뀌지 않는지
5. **폐기 통계** 버튼을 눌러 통계 창이 열리고 닫기·Esc로 닫히는지

#### 개발 외 업무: 최종 발표와 데모 시연
- 시연 순서 정리: 시작 알림 → 목록 → 등록 → 요리 추천 → 요리하기 → 폐기 통계
- 예상 질문과 답변 정리 (예: "왜 로직과 화면을 나눴나요?", "추천 점수는 어떻게 정했나요?", "조리법은 어떻게 자동으로 불러오나요?")
- 리허설 1회 이상
- 산출물: 시연 시나리오, 예상 질문 정리

---

### 6.3 김기범 (expiry, dlg_food)

**담당:** `expiry.c/h`, `dlg_food.c/h`
**브랜치:** `feature/expiry`, `feature/dlg-food`

`expiry`를 먼저 만들고(11/25 먼저 올릴 함수, 11/29 전체), 그다음 `dlg_food`를 만듭니다(12/03).

#### expiry

##### expiry.h

```c
#ifndef EXPIRY_H
#define EXPIRY_H
#include "common.h"

Date getToday(void);                                      /* 오늘 날짜 */
int  isValidDate(Date d);                                 /* 올바른 날짜면 1, 아니면 0 (윤년, 월별 일수) */
int  daysBetween(Date from, Date to);                     /* to - from (일). to가 뒤면 양수 */
int  daysLeft(Date expire);                               /* expire - 오늘. 지났으면 음수 */
void sortByExpiry(Fridge *f);                             /* F2: 임박순(유통기한 빠른 순) 정렬 */
void countAlerts(const Fridge *f, int *expired, int *soon);   /* F4: 기한 지난 개수, 임박 개수 */
void formatDday(int days, char *buf, int bufLen);         /* "D-3", "D-DAY", "D+2" 문자열 만들기 */

#endif
```

| 함수 | 동작과 반환값 |
|---|---|
| `getToday` | `time()`, `localtime_s()`로 오늘 날짜를 `Date`로 반환 |
| `isValidDate` | 연도 2000~2100, 월 1~12, 일 1~그 달의 일수. **윤년**(4로 나누어떨어지고 100으로 안 나누어떨어짐, 또는 400으로 나누어떨어짐)이면 2월은 29일 |
| `daysBetween` | `Date` 두 개를 `struct tm`으로 바꿔 `mktime()`과 `difftime()`으로 계산. **`tm_hour = 12`로 맞추고 반올림**해야 서머타임 같은 시간대 문제가 없다 |
| `daysLeft` | `daysBetween(getToday(), expire)` |
| `sortByExpiry` | `qsort` 사용. 비교 함수는 `Date`를 연→월→일 순으로 비교, 같으면 `strcmp`로 이름 비교 |
| `countAlerts` | `*expired` = `daysLeft < 0`인 항목 수, `*soon` = `0 <= daysLeft <= ALERT_DAYS`인 항목 수 (**둘이 겹치지 않는다**). 항목 개수(종류 수)를 센다 |
| `formatDday` | `days > 0` → `D-3`, `days == 0` → `D-DAY`, `days < 0` → `D+2` (`snprintf` 사용). "(지남)" 같은 설명은 화면이 붙인다 |

구현 요점

```c
int daysBetween(Date from, Date to)
{
    struct tm a = { 0 }, b = { 0 };
    double sec;

    a.tm_year = from.year - 1900;  a.tm_mon = from.month - 1;  a.tm_mday = from.day;  a.tm_hour = 12;
    b.tm_year = to.year - 1900;    b.tm_mon = to.month - 1;     b.tm_mday = to.day;    b.tm_hour = 12;

    sec = difftime(mktime(&b), mktime(&a));
    return (int)(sec / 86400.0 + (sec >= 0 ? 0.5 : -0.5));   /* 반올림 */
}
```

```c
/* qsort 비교 함수 모양 (정렬 기준을 직접 채우세요) */
static int compareFood(const void *p, const void *q)
{
    const Food *a = (const Food *)p;
    const Food *b = (const Food *)q;
    ...
}
```

##### expiry 완료 기준
- `daysBetween({2026,11,9}, {2026,11,12}) == 3`, 월말·연말을 넘겨도 정확하다 (`{2026,2,28}` → `{2026,3,1}`은 1일, 윤년 `{2028,2,28}` → `{2028,3,1}`은 2일)
- `isValidDate({2026,2,29}) == 0`, `isValidDate({2028,2,29}) == 1`, 월 13이나 일 0은 0
- 정렬 후 `items[0]`이 가장 유통기한이 빠른 재료다
- 재료가 하나도 없어도 `countAlerts`와 `sortByExpiry`가 죽지 않는다

#### dlg_food (식재료 등록·수정 창)

##### dlg_food.h

```c
#ifndef DLG_FOOD_H
#define DLG_FOOD_H
#include "gui_common.h"

/* 입력을 받아 food에 채운다. isEdit=1이면 food의 현재 값을 채워서 보여 준다.
 * 등록/수정을 눌렀으면 1, 취소면 0 */
int foodDialog(HWND owner, Food *food, int isEdit);

#endif
```

| 입력칸 | 검사 |
|---|---|
| 이름 | 비어 있으면 안 되고 **공백 불가**, 한글 **14자 이내** (`readName` 사용) |
| 유통기한 | `YYYY-MM-DD`. `parseDate`(`gui_common`)가 `isValidDate`(김기범)로 검사. 새로 등록할 때는 오늘 날짜를 처음에 채워 둔다 |
| 수량 | 1~999 숫자 |
| 보관 | 냉장 / 냉동 선택 (기본 냉장) |

구현 요점
- 대화상자 틀(`gui_dialog.h`)을 씁니다. 6.2장의 `dlg_simple`과 같은 방식(`DlgBase` → `onCreate`/`onCommand` → `runDialog`)입니다.
- 검사에 실패하면 `bad()`로 안내하고 해당 입력칸으로 커서를 돌려 줍니다. 창은 닫지 않습니다.
- `isEdit == 1`이면 열 때 `food`의 현재 값을 각 칸에 채우고, 버튼 글자는 "수정"입니다.
- 이 창은 **입력만 받습니다.** `addFood`·`updateFood` 호출은 메인 창(`ui_gui`)이 합니다.

##### dlg_food 완료 기준
- `2026-02-30`, `2026-13-01`, `abc`, 빈칸을 넣으면 안내만 나오고 창이 유지된다
- 이름에 공백이 있거나 한글 14자를 넘으면 거절한다
- 수량 `0`, `1000`, 글자를 거절한다
- 수정 창을 열면 선택한 재료의 값이 채워져 있고, 취소하면 재료가 바뀌지 않는다

#### 혼자 확인하기

1. **expiry:** 유통기한이 지난 재료, 오늘까지인 재료, 3일 뒤인 재료를 등록한다. 표가 **유통기한 빠른 순**으로 정렬되고, 남은 기간이 `D+n (지남)`, `D-DAY (임박)`, `D-3`으로 나오는지, 기한 지남은 빨강·임박은 주황인지 확인한다
2. **expiry:** 프로그램을 껐다 켜면 시작 알림 팝업의 "지난 재료"·"임박 재료" 개수가 위 재료 수와 맞는지 확인한다
3. **expiry:** 윤년 `2028-02-29`는 등록되고, `2026-02-29`는 거절되는지 확인한다
4. **dlg_food:** 위 "완료 기준"의 잘못된 값을 하나씩 넣어 본다
5. 같은 이름·같은 유통기한·같은 보관 방법으로 두 번 등록하면 한 줄로 합쳐지는지 확인한다 (합치는 일은 `addFood`, 안내는 메인 창)

#### 개발 외 업무
- 프로그램 흐름도 작성 (발표 PPT의 "3.3 프로그램 흐름도" 기준, 제안·결과보고서에 들어갈 그림)

---

### 6.4 최민솔 (recipe, dlg_recipe)

**담당:** `recipe.c/h`, `dlg_recipe.c/h`, `data/recipes.txt`
**브랜치:** `feature/recipe`, `feature/dlg-recipe`

`recipe`를 먼저 만들고(11/25 먼저 올릴 함수, 12/03 전체), 이어서 `dlg_recipe`를 만듭니다(12/03).

#### recipe

##### recipe.h

```c
#ifndef RECIPE_H
#define RECIPE_H
#include "common.h"

void       initRecipeBook(RecipeBook *rb);                /* capacity 8로 동적 배열 생성 */
void       freeRecipeBook(RecipeBook *rb);
ResultCode addRecipe(RecipeBook *rb, const Recipe *rc);   /* F3: 레시피 추가 */
int        recommend(const Fridge *f, const RecipeBook *rb,
                     Recommendation *out, int max);       /* F3: 점수순 추천. 채운 개수 반환 */
ResultCode cookRecipe(Fridge *f, const Recipe *rc);       /* F5: 재료 자동 차감 */

#endif
```

| 함수 | 동작과 반환값 |
|---|---|
| `addRecipe` | 이름이 비었거나, `ingCount`가 1~`MAX_ING`이 아니거나, 수량이 0 이하이거나, **같은 이름의 레시피가 이미 있으면** `RC_INVALID`. 그 외에는 `*rc`를 그대로 복사해 추가 (`isCustom`은 호출한 쪽이 정함). 가득 차면 `realloc` 2배 |
| `recommend` | 아래 알고리즘으로 `out[0..n-1]`을 채우고 `n`을 반환. `max`는 `out` 배열 크기 |
| `cookRecipe` | **먼저 모든 재료가 충분한지 확인**하고, 하나라도 부족하면 아무것도 차감하지 않고 `RC_NOT_ENOUGH` 반환. 충분하면 재료마다 차감하고 `RC_OK` |

**용어:** 재료를 "보유"한다는 것은 **기한이 지나지 않은** 같은 이름의 재료(여러 줄일 수 있음) 수량의 합이 레시피의 필요 수량 이상이라는 뜻입니다. 기한이 지난 재료는 쓸 수 없다고 봅니다. (`recipe.c` 안에 `static int countUsable(const Fridge *f, const char *name)` 같은 도우미 함수를 만들어 쓰세요.)

#### recommend 알고리즘

1. 레시피마다 재료를 하나씩 보며 `matched`(보유한 재료 수)를 센다. 보유하지 않은 재료는 `missing[]`에 이름을 넣고 `missingCount`를 올린다.
2. 보유한 재료 중 **임박 재료**(`0 <= daysLeft <= ALERT_DAYS`인 줄이 하나라도 있음)는 `urgent[]`에 이름을 넣고 `urgentCount`를 올린다.
3. `matched * 2 < ingCount`이거나 `matched == 0`이면 **제외** (필요 재료의 절반 이상을 가진 요리만 추천)
4. 점수 = `matched × 10` + `urgentCount × 5` + (`matched == ingCount`이면 `canCook = 1`, +20)
5. `out`에 `recipeIdx`, `matched`, `score`, `canCook`, `missing`, `urgent`를 채운다. `max`개가 차면 멈춘다.
6. 점수 내림차순으로 정렬 (`qsort`). 점수가 같으면 `canCook`인 쪽, 그다음 레시피 이름 순.

#### cookRecipe 알고리즘

1. 모든 재료에 대해 `countUsable(f, 이름) >= 필요 수량`인지 확인한다. 하나라도 아니면 `RC_NOT_ENOUGH`.
2. 재료마다 필요 수량이 0이 될 때까지 반복: 기한이 지나지 않은 같은 이름의 줄 중 **유통기한이 가장 빠른 줄**의 인덱스를 찾아 `useFood(f, idx, min(그 줄 수량, 남은 필요량))`를 호출한다.
3. `useFood`가 줄을 삭제하면 인덱스가 밀리므로, **차감할 때마다 인덱스를 처음부터 다시 찾는다.**

#### data/recipes.txt

12장의 형식으로 **기본 레시피 20개 이상**을 작성합니다. ANSI로 저장하고, 재료 이름은 팀 전체가 똑같이 쓰도록 흔한 이름으로 통일합니다. (`계란`, `대파`, `양파`, `두부`, `김치`, `돼지고기`, `밥` 처럼. `달걀`과 `계란`을 섞어 쓰지 않기.) 사용자가 등록하는 재료와 이름이 같아야 매칭됩니다. 12/07 전에 작성이 끝나야 통합 테스트를 할 수 있습니다.

##### recipe 완료 기준
- 재료가 충분한 레시피는 `canCook == 1`이고 점수가 가장 높다
- 재료가 절반 미만인 레시피는 추천에 나오지 않는다
- `cookRecipe`는 재료가 하나라도 부족하면 **아무것도 차감하지 않는다**
- 같은 재료가 유통기한이 다른 두 줄이면 **빠른 줄부터** 차감한다
- 레시피 20개 이상, 기본 레시피는 `isCustom == 0`

#### dlg_recipe (레시피 추가 창)

##### dlg_recipe.h

```c
#ifndef DLG_RECIPE_H
#define DLG_RECIPE_H
#include "gui_common.h"

/* 레시피를 추가했으면 1. addedName에 추가한 요리 이름이 들어간다 (필요 없으면 NULL) */
int recipeDialog(HWND owner, wchar_t *addedName, int nameLen);

#endif
```

| 입력칸 | 검사 |
|---|---|
| 요리 이름 | 비어 있으면 안 되고 공백 불가, 한글 14자 이내 |
| 재료 (최대 `MAX_ING` = 10개) | 이름(공백 불가, 14자 이내)과 필요 수량(1~99)을 **짝으로** 입력. 안 쓰는 칸은 비워 둔다. 하나도 없으면 거절 |
| 조리 순서 (선택) | 한 줄에 한 단계, 최대 20단계, 한 단계는 한글 127자 이내 |

구현 요점
- 입력값을 **모두 검사한 뒤에** `Recipe`를 채워서 `addRecipe`(**`isCustom = 1`**)를 호출합니다. 같은 이름이 이미 있으면 `RC_INVALID`이므로 "이미 있는 이름"이라고 안내합니다.
- 조리 순서를 입력했으면 `addRecipe`가 성공한 뒤 `addRecipeSteps`(`guide.h`)로 저장합니다.
- 대화상자 틀(`gui_dialog.h`)은 6.2장과 같은 방식으로 씁니다.

##### dlg_recipe 완료 기준
- 이름만 쓰고 재료를 하나도 안 넣으면 거절한다. 재료 이름만 쓰고 수량을 비우면(또는 반대) 그 칸으로 커서가 이동한다
- 이미 있는 요리 이름을 넣으면 거절하고, 새 이름이면 추가된다
- 추가한 레시피는 `isCustom == 1`이고, 프로그램을 껐다 켜도 남아 있다
- 조리 순서를 입력한 레시피는 요리 추천 창에서 그 순서가 보인다

#### 혼자 확인하기

1. **recipe:** 재료를 등록하고 **요리 추천** 창을 열어, 재료가 충분한 요리는 "바로 가능", 절반 이상이면 "부족: ○○"으로 나오는지 확인한다
2. **recipe:** "바로 가능"한 요리를 골라 **요리하기** → 재료 수량이 레시피대로 줄고, 0이 된 재료는 표에서 사라지는지 확인한다
3. **recipe:** 같은 재료가 유통기한이 다른 두 줄이면 **유통기한이 빠른 줄부터** 줄어드는지 확인한다
4. **recipe:** 재료가 부족한 요리는 요리하기 버튼이 눌리지 않고, 수량이 줄지 않는지 확인한다
5. **dlg_recipe:** 위 "완료 기준"을 하나씩 확인한다. 같은 이름을 두 번 추가해 본다
6. **recipes.txt:** 레시피가 20개 이상이고 재료 이름이 통일되어 있는지(`계란`/`달걀` 섞임 없음) 확인한다

#### 개발 외 업무
- 부팀장: 윤종하가 없을 때 PR 검토·병합 대행, 윤종하의 PR 검토
- 시연용 샘플 데이터(`data/fridge.txt`) 준비

---

### 6.5 박지연 (food, stats)

**담당:** `food.c/h`, `stats.c/h`
**브랜치:** `feature/food`, `feature/stats`

`food`를 먼저 만들고(11/25 먼저 올릴 함수, 11/29 전체), 그다음 `stats`를 만듭니다(12/03).

#### food

##### food.h

```c
#ifndef FOOD_H
#define FOOD_H
#include "common.h"

void       initFridge(Fridge *f);                                 /* capacity 4로 동적 배열 생성 */
void       freeFridge(Fridge *f);                                 /* 메모리 해제, items = NULL, count = 0 */
ResultCode addFood(Fridge *f, const Food *item);                  /* F1 */
ResultCode updateFood(Fridge *f, int idx, const Food *item);      /* F5 */
ResultCode removeFood(Fridge *f, int idx);                        /* F5 */
ResultCode useFood(Fridge *f, int idx, int amount);               /* F5: 수량 차감 */
int        findFood(const Fridge *f, const char *name);           /* 이름으로 검색, 없으면 -1 */

#endif
```

| 함수 | 동작과 반환값 |
|---|---|
| `initFridge` | `items = malloc(4개)`, `count = 0`, `capacity = 4`. 할당 실패하면 `items = NULL`, `capacity = 0` |
| `freeFridge` | `free(items)` 후 `items = NULL`, `count = capacity = 0` |
| `addFood` | 이름이 비었거나 `quantity <= 0`, `storage`가 0/1이 아니면 `RC_INVALID`. **이름·유통기한·보관 방법이 모두 같은 재료가 이미 있으면 새로 추가하지 않고 수량만 더한다.** 가득 찼으면 `realloc` 2배, 실패하면 `RC_NO_MEMORY` (기존 데이터는 그대로) |
| `updateFood` | `idx`가 범위 밖이면 `RC_NOT_FOUND`, `item`이 잘못됐으면 `RC_INVALID`. 해당 칸을 `*item`으로 덮어쓴다 |
| `removeFood` | `idx`가 범위 밖이면 `RC_NOT_FOUND`. 뒤의 항목을 한 칸씩 앞으로 당기고 `count`를 줄인다 (`memmove`) |
| `useFood` | `idx` 범위 밖이면 `RC_NOT_FOUND`, `amount <= 0`이면 `RC_INVALID`, `amount`가 보유 수량보다 크면 `RC_NOT_ENOUGH`. 차감 후 수량이 0이면 `removeFood`로 삭제 |
| `findFood` | `strcmp`로 이름이 같은 **첫 번째** 항목의 인덱스. 없으면 -1 |

날짜가 올바른지(윤년, 월별 일수)는 `food`가 아니라 **입력 단계(`dlg_food`의 날짜 입력)에서** 검사합니다. 그래서 `food.c`는 `expiry.h`를 부르지 않습니다.

구현 요점: 동적 배열 확장은 이렇게 합니다.

```c
if (f->count == f->capacity) {
    int newCap = (f->capacity == 0) ? 4 : f->capacity * 2;
    Food *tmp = (Food *)realloc(f->items, sizeof(Food) * newCap);
    if (tmp == NULL)
        return RC_NO_MEMORY;       /* f->items는 그대로 살아 있다 */
    f->items = tmp;
    f->capacity = newCap;
}
f->items[f->count++] = *item;
```

#### 완료 기준 (food)
- `addFood`를 20번 호출해도(capacity 4 → 8 → 16 → 32) 오류 없이 모두 저장된다
- 같은 이름·같은 유통기한·같은 보관 방법의 재료를 두 번 등록하면 항목은 1개, 수량은 합쳐진다
- 범위 밖 `idx`, 수량 초과 차감에 프로그램이 죽지 않고 지정된 `ResultCode`가 나온다

#### stats

##### stats.h

```c
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
```

| 함수 | 동작과 반환값 |
|---|---|
| `appendWaste` | `rec`를 배열 끝에 추가. 가득 차면 `realloc` 2배, 실패 시 `RC_NO_MEMORY` |
| `addWaste` | `item`으로 `WasteRecord`(이름, 수량, 보관, 유통기한, `discarded = today`)를 만들어 `appendWaste` 호출. `item`이 `NULL`이면 `RC_INVALID` |
| `discardExpired` | 냉장고를 **뒤에서부터** 훑으며 `daysLeft < 0`인 항목을 `addWaste` 후 `removeFood`. 폐기한 개수를 반환. (뒤에서부터 돌아야 삭제해도 인덱스가 밀리지 않는다) |
| `computeWasteStats` | `*out`을 0으로 초기화한 뒤 채운다. `totalRecords` = 기록 수, `totalQuantity` = 수량 합, `fridgeCount`/`freezerCount` = 보관 방법별 기록 수, `thisMonth` = 폐기일이 `today`와 같은 연·월인 기록 수, `topName`/`topCount` = **가장 많이 폐기된 재료 이름과 횟수**(같으면 먼저 나온 것). 기록이 없으면 모두 0, `topName`은 빈 문자열 |

**새로 추가된 점:** `appendWaste`(파일에서 불러올 때 폐기일을 그대로 넣기 위해)와 `computeWasteStats`의 `today` 매개변수(이번 달을 알기 위해)는 11장 자료구조를 만든 뒤 추가된 것입니다. 이 README의 선언이 **확정안**입니다.

#### 완료 기준 (stats)
- 일괄 폐기 후 냉장고에서 기한 지난 재료가 모두 사라지고 폐기 기록은 같은 개수만큼 늘어난다
- 폐기 기록이 0개일 때 `computeWasteStats`가 모두 0으로 채우고 죽지 않는다
- 폐기 횟수가 같은 재료가 여럿이면 먼저 나온 재료가 `topName`이다

#### 혼자 확인하기

1. **food:** 재료를 등록·수정·삭제하고 수량을 바꾼다. 표와 상태줄의 개수가 맞는지 확인한다
2. **food:** 같은 이름·같은 유통기한·같은 보관 방법의 재료를 두 번 등록하면 한 줄로 합쳐지고 수량이 더해지는지 확인한다. 유통기한이나 보관 방법이 다르면 따로 남는지 확인한다
3. **food:** 재료를 20개 넘게 등록해도 오류 없이 모두 표시되는지 확인한다 (동적 배열 확장)
4. **food:** 수량 변경에서 현재보다 줄이면 `useFood`, 늘리면 `updateFood`로 동작하는지 확인한다
5. **stats:** 재료를 삭제하면서 "폐기로 기록"을 선택하고, **폐기 통계**의 횟수·수량이 늘어나는지 확인한다
6. **stats:** 기한 지난 재료를 여러 개 만들고 **기한 지난 재료 폐기**를 누른다. 표에서 모두 사라지고 폐기 기록은 같은 개수만큼 늘어나는지 확인한다
7. **stats:** 폐기 기록이 하나도 없을 때 통계 창이 "아직 폐기 기록이 없습니다"로 나오고 죽지 않는지 확인한다

#### 개발 외 업무
- 확인 시나리오 작성: 정상 입력, 잘못된 날짜(2월 30일), 빈 목록, 수량 0, 파일 없음, 이름에 공백, 한글 14자 초과 등
- 통합 테스트(12/07~) 실행, 발견한 버그를 **GitHub Issues**에 등록 (제목: `[버그] 어떤 화면 - 증상`, 내용: 재현 순서)
- 테스트 결과서 작성

---

### 6.6 GUI 화면과 API 조리법 (윤종하)

윤종하가 진행합니다. **로직 모듈은 고치지 않고** 그대로 호출합니다. 팀원이 맡을 일은 없으며, 화면 예시는 15.2장, 한글 변환은 13.3장입니다.

---

## 7. 프로젝트 목표

### 필수 목표 (GUI 버전)
F1~F6 기능을 Windows 창 화면(Win32 API)으로 모두 완성합니다. 로직 모듈은 화면과 분리되어 있어 **수정 없이** 그대로 사용합니다.

| 단계 | 내용 | 상태 |
|:---:|---|:---:|
| G1 | 프로그램 시작 시 유통기한 알림을 Windows 팝업 창(`MessageBoxW`)으로 표시 | 완료 |
| G2 | 메인 창에 식재료 목록 표(ListView)와 기능 버튼 배치, 임박순 정렬 표시 | 완료 |
| G3 | 식재료 등록·수정 입력 창, 삭제·수량 변경 버튼. 같은 재료(이름·유통기한·보관 방법 동일)는 수량을 합쳐서 표시 | 완료 |
| G4 | 요리 추천 창(추천 목록 + "요리하기" 버튼으로 자동 차감), 레시피 추가 창 | 완료 |
| G5 | 폐기 통계 창, 기한 지난 재료는 빨간색, 임박 재료는 주황색으로 강조 | 완료 |

- 데이터 파일 저장·불러오기로 프로그램을 껐다 켜도 정보가 유지됩니다.

### 도전 목표 (API로 조리 방법 자동 불러오기)
식품안전나라 "조리식품의 레시피 DB"(COOKRCP01) OpenAPI에서 **요리의 조리 순서를 자동으로 가져옵니다.**

| 단계 | 내용 | 담당 파일 |
|:---:|---|---|
| A1 | 인증키 설정: 환경변수 `FOODSAFETY_API_KEY` 또는 `data/api_key.txt`(첫 줄). 키는 코드에 쓰지 않고 `.gitignore`로 올라가지 않게 함 | `guide_api` |
| A2 | 요리 이름으로 API를 호출해 조리 순서(MANUAL01~20)를 받아 옴. 응답이 UTF-8이든 ANSI든 처리 | `guide_api` |
| A3 | 받은 조리 순서를 `data/steps.txt`에 저장해, 한 번 받은 요리는 인터넷 없이도 표시 | `guide` |
| A4 | 요리 추천 창에서 요리를 고르면 **조리 순서가 없을 때 자동으로** 불러옴. 창이 멈추지 않도록 백그라운드 스레드에서 호출하고, 실패 사유를 화면에 안내. `[조리법 불러오기]` 버튼으로 다시 받거나 갱신 | `dlg_recommend` |
| A5 | **요리 추천을 API로**: 내 재료 이름으로 식품안전나라 레시피를 찾아 추천. 재료의 **양·개수는 보지 않고 이름이 재료 정보에 나오는지만** 대조하며, 임박 재료는 가산점. 인증키가 없으면 저장된 레시피로 추천 | `apirec`, `guide_api`, `dlg_recommend` |

> 인증키가 없으면 자동 불러오기는 동작하지 않고, 저장된 조리 순서만 표시합니다. 키 발급·설정 방법은 14장을 참고합니다.

---

## 8. 주요 기능

| 번호 | 기능 | 설명 | 로직 모듈 | 우선순위 |
|:---:|---|---|---|:---:|
| F1 | 식재료 등록 | 이름, 유통기한(YYYY-MM-DD), 수량, 보관 방법(냉장/냉동) 입력 | `food` | 필수 |
| F2 | 유통기한 임박순 자동 정렬 | 오늘 날짜를 자동으로 가져와 D-day 계산 후 남은 일수가 적은 순으로 정렬 | `expiry` | 필수 |
| F3 | 요리 추천 | 미리 등록된 레시피와 보유 재료를 비교해 추천, 사용자 정의 레시피 추가 | `recipe` | 필수 |
| F4 | 시작 시 임박 알림 | 실행 직후 "유통기한 지난 재료 N개, 2일 이내 재료 M개" 경고 표시 | `expiry` | 필수 |
| F5 | 식재료 수정·삭제·수량 관리 | 정보 수정, 삭제, 수량 변경 / 요리 선택 시 사용한 재료 자동 차감 | `food`, `recipe` | 필수 |
| F6 | 폐기 기록 통계 | 폐기한 재료를 기록하고 총 폐기 수, 보관 방법별 비율, 가장 많이 버린 재료 출력 | `stats` | 필수 |
| G1~G5 | GUI 화면 | 위 기능을 Windows 창 화면으로 제공 | `ui_gui` | 필수 |
| A1~A5 | API 조리법·요리 추천 | 요리를 고르면 조리 순서를 자동으로 가져와 저장하고, 내 재료 이름으로 인터넷 레시피를 추천 | `guide_api`, `guide`, `apirec`, `dlg_recommend` | 도전 |

### 기능 상세

**F2. D-day 계산**
- `time.h`의 `time()`, `localtime_s()`로 오늘 날짜를 가져옵니다.
- 유통기한 날짜를 `struct tm`으로 바꾼 뒤 `mktime()`과 `difftime()`으로 남은 일수를 계산합니다.
- 표기 예: `D-3`, `D-DAY`, `D+2 (기한 지남)`

**F3. 요리 추천 점수**
- 보유한 재료 1개당 +10점
- 유통기한 2일 이내 재료를 사용하면 +5점
- 필요한 재료를 모두 가지고 있으면 +20점
- 점수 순으로 정렬하고, 필요 재료의 절반 이상을 가진 요리만 추천하며, 부족한 재료도 함께 보여 줍니다.

**F4. 시작 알림**
- 실행하면 메인 창이 뜨기 전에 Windows 경고 팝업 창(G1)으로 "지난 재료 N개, 2일 이내 재료 M개"와 해당 재료 이름·D-day를 보여 줍니다.

**F5. 자동 차감**
- 추천 목록에서 요리를 선택하면 확인을 받은 뒤 레시피에 적힌 수량만큼 재료를 차감합니다.
- 수량이 0이 된 재료는 목록에서 자동 삭제됩니다.

**F6. 폐기 기록**
- 식재료를 삭제할 때 폐기로 기록할지 선택합니다.
- "기한 지난 재료 일괄 폐기" 기능으로 기한이 지난 재료를 한꺼번에 폐기 처리합니다.

---

## 9. 설계 원칙: 로직과 화면 분리

화면(GUI)이 바뀌어도 계산·저장 코드를 그대로 쓰고, 담당자끼리 서로 방해하지 않도록 프로그램을 **로직 계층**과 **화면 계층**으로 나눕니다.

```
┌──────────────────────────────────────────────────┐
│  화면 계층 (GUI)                                  │
│  main_gui.c  ui_gui.c  gui_common.c  gui_dialog.c │
│  dlg_simple.c  dlg_food.c  dlg_recipe.c           │
│  dlg_recommend.c            (Win32 API)           │
└────────────────────────┬─────────────────────────┘
                         │  로직 함수만 호출
                         ▼
┌──────────────────────────────────────────────────┐
│  로직 계층                                        │
│  food.c  expiry.c  recipe.c  stats.c  fileio.c   │
│  guide.c  guide_api.c  apirec.c                  │
│  ※ 창·대화상자·printf·scanf 사용 금지             │
└──────────────────────────────────────────────────┘
```

### 규칙
1. **로직 모듈에서는 화면 입출력을 하지 않습니다.** 값은 매개변수로 받고, 결과는 반환값이나 포인터로 돌려줍니다.
2. 성공·실패는 `ResultCode`로 반환하고, 어떤 메시지를 보여 줄지는 화면 모듈이 결정합니다.
3. 화면 모듈은 로직 모듈의 함수만 호출하며, 구조체 내부를 직접 고치지 않습니다.

```c
/* 잘못된 예: 화면과 로직이 섞여 있어 다른 사람이 따로 만들 수 없다 */
void addFood(Fridge *f) {
    printf("이름: ");
    scanf("%s", ...);
}

/* 좋은 예: 로직만 담당하므로 어떤 화면에서든 그대로 호출 */
ResultCode addFood(Fridge *f, const Food *item);
```

---

## 10. 모듈 구조

솔루션 안에 **GUI 프로젝트(`FridgeGUI`) 하나**를 두고, 로직 모듈과 화면 파일을 모두 이 프로젝트가 사용합니다.

```
fridge-manager/
├── README.md
├── .gitignore
├── FridgeManager.slnx         # 솔루션 파일 (이 파일을 열어서 작업)
├── FridgeGUI/
│   └── FridgeGUI.vcxproj     # GUI 프로젝트 (필수)
├── include/                  # 헤더 파일(.h)만 모아 둔 폴더
│   ├── common.h                   # 공통 구조체, 열거형, 상수, ResultCode
│   │   ── 로직 계층 ──
│   ├── food.h  expiry.h  recipe.h  stats.h  fileio.h
│   ├── guide.h  guide_api.h  apirec.h
│   │   ── 화면 계층 (GUI) ──
│   ├── ui_gui.h  gui_common.h  gui_dialog.h
│   └── dlg_food.h  dlg_simple.h  dlg_recipe.h  dlg_recommend.h
├── src/                      # 소스 파일(.c)만 모아 둔 폴더
│   │   ── 로직 계층 (화면과 무관) ──
│   ├── food.c                     # F1 등록, F5 수정·삭제·수량 변경·검색
│   ├── expiry.c                   # F2 날짜 계산·D-day·정렬, F4 임박 개수 계산
│   ├── recipe.c                   # F3 추천 점수 계산·레시피 추가, F5 자동 차감
│   ├── stats.c                    # F6 폐기 기록 추가·일괄 폐기·통계 계산
│   ├── fileio.c                   # 모든 데이터 파일 저장·불러오기
│   ├── guide.c                    # 요리별 조리 순서 조회·저장 (data/steps.txt)
│   ├── guide_api.c                # 도전: 식품안전나라 API로 조리 순서·요리 가져오기
│   ├── apirec.c                   # 도전: 재료 이름으로 API 요리 추천 (양은 비교하지 않음)
│   │   ── 화면 계층 (GUI) ──
│   ├── main_gui.c                 # GUI 진입점 (wWinMain): 데이터 불러오기 → 알림 → 창 → 저장
│   ├── ui_gui.c                   # 메인 창(식재료 표·기능 버튼), 시작 알림 (G1, G2)
│   ├── gui_common.c               # 공통 도우미: 문자 변환, 메시지 창, 컨트롤·표 도우미, 목록 갱신
│   ├── gui_dialog.c               # 모달 대화상자 틀 (DlgBase, runDialog)
│   ├── dlg_food.c                 # 식재료 등록·수정 창 (G3)
│   ├── dlg_simple.c               # 숫자 입력(수량 변경) 창, 글 보기(폐기 통계) 창 (G3, G5)
│   ├── dlg_recipe.c               # 레시피 추가 창 (G4)
│   └── dlg_recommend.c            # 요리 추천 창 + 조리법 자동 불러오기 (G4, A4)
└── data/
    ├── fridge.txt            # 식재료 목록
    ├── recipes.txt           # 레시피 목록 (기본 + 사용자 추가)
    ├── steps.txt             # 요리별 조리 순서 (API로 받은 것 포함)
    ├── waste.txt             # 폐기 기록
    └── api_key.txt           # API 인증키 (직접 만듦, .gitignore로 제외됨)
```

| 프로젝트 | 포함하는 파일 |
|---|---|
| `FridgeGUI` | 화면: `main_gui.c`, `ui_gui.c`, `gui_common.c`, `gui_dialog.c`, `dlg_*.c` (각각 `.h`와 쌍) + 로직 모듈(`food`, `expiry`, `recipe`, `stats`, `fileio`, `guide`, `guide_api`, `apirec`) |

> 콘솔 버전(`main.c`, `ui_console`, `util`, `test_*.c`)은 제거했습니다. 로직 모듈은 화면과 분리되어 있어 수정 없이 GUI에서 그대로 쓰입니다.
> 헤더는 `include/`, 소스는 `src/`에 나뉘어 있고, `FridgeGUI.vcxproj`의 "추가 포함 디렉터리"(`$(ProjectDir)..\include`)로 헤더를 찾습니다. 새 파일을 추가할 때도 `.h`는 `include/`, `.c`는 `src/`에 둡니다.

### 모듈별 주요 함수 (요약)

> 정확한 선언과 동작은 **6장**의 각 팀원 절을 기준으로 합니다. 아래 표는 한눈에 보기 위한 요약입니다.

**로직 계층**

| 모듈 | 함수 | 역할 |
|---|---|---|
| `food` | `void initFridge(Fridge *f)` / `void freeFridge(Fridge *f)` | 동적 배열 초기화·해제 |
| | `ResultCode addFood(Fridge *f, const Food *item)` | 식재료 추가 (같은 이름·유통기한이면 수량 합치기) |
| | `ResultCode updateFood(Fridge *f, int idx, const Food *item)` | 식재료 정보 수정 |
| | `ResultCode removeFood(Fridge *f, int idx)` | 식재료 삭제 |
| | `ResultCode useFood(Fridge *f, int idx, int amount)` | 수량 차감, 0이면 삭제 |
| | `int findFood(const Fridge *f, const char *name)` | 이름으로 검색, 없으면 -1 |
| `expiry` | `Date getToday(void)` | 오늘 날짜 반환 |
| | `int daysLeft(Date expire)` | 남은 일수 계산 (지났으면 음수) |
| | `int isValidDate(Date d)` | 날짜 유효성 검사 (윤년, 월별 일수) |
| | `void sortByExpiry(Fridge *f)` | 임박순 정렬 (`qsort`) |
| | `void countAlerts(const Fridge *f, int *expired, int *soon)` | 기한 지난 개수, 2일 이내 개수 계산 |
| `recipe` | `int recommend(const Fridge *f, const RecipeBook *rb, Recommendation *out, int max)` | 추천 결과를 점수순으로 채우고 개수 반환 |
| | `ResultCode cookRecipe(Fridge *f, const Recipe *rc)` | 레시피 재료 자동 차감 (부족하면 `RC_NOT_ENOUGH`) |
| | `ResultCode addRecipe(RecipeBook *rb, const Recipe *rc)` | 사용자 레시피 추가 |
| `stats` | `ResultCode addWaste(WasteList *w, const Food *item, Date today)` | 폐기 기록 추가 |
| | `int discardExpired(Fridge *f, WasteList *w)` | 기한 지난 재료 일괄 폐기, 폐기 개수 반환 |
| | `void computeWasteStats(const WasteList *w, Date today, WasteStats *out)` | 통계 계산 (`today`로 이번 달 판단) |
| `fileio` | `ResultCode loadFridge / saveFridge` | `fridge.txt` 읽기·쓰기 |
| | `ResultCode loadRecipes / saveRecipes` | `recipes.txt` 읽기·쓰기 |
| | `ResultCode loadWaste / saveWaste` | `waste.txt` 읽기·쓰기 |

**화면 계층**

| 모듈 | 함수 | 역할 |
|---|---|---|
| `ui_gui` | `void guiShowStartupAlert(const Fridge *f)` | G1: 시작 경고 팝업 |
| | `int guiRun(HINSTANCE hInst, AppData *app, int nShow)` | G2: 메인 창 생성과 메시지 루프 |
| `gui_common` | `void refreshMain(void)` | 임박순 정렬 후 표·상태줄 갱신 |
| | `ansiToWide / wideToAnsi / mk / msg / ask / bad ...` | 문자 변환, 컨트롤 만들기, 메시지 창 |
| `gui_dialog` | `void runDialog(HWND owner, const wchar_t *title, int cw, int ch, DlgBase *b)` | 모든 대화상자의 공통 틀 |
| `dlg_simple` | `int promptNumber(...)` / `void textDialog(...)` | 수량 입력 창 / 글 보기(폐기 통계) 창 |
| `dlg_food` | `int foodDialog(HWND owner, Food *food, int isEdit)` | G3: 식재료 등록·수정 창 |
| `dlg_recipe` | `int recipeDialog(HWND owner, wchar_t *addedName, int nameLen)` | G4: 레시피 추가 창 |
| `dlg_recommend` | `void recommendDialog(HWND owner)` | G4·A4: 요리 추천 창, 조리법 자동 불러오기 |

> 주의: `common.h`와 로직 모듈의 함수 선언(`.h`)은 구현을 시작하기 전에 팀 전체가 확정합니다. 확정 후 변경할 때는 반드시 팀원에게 공유합니다.

---

## 11. 핵심 자료구조

```c
/* common.h */
#ifndef COMMON_H
#define COMMON_H

#define NAME_LEN     30
#define MAX_ING      10
#define ALERT_DAYS   2      /* 임박 기준: 2일 이내 */

#define FRIDGE_FILE  "data/fridge.txt"
#define RECIPE_FILE  "data/recipes.txt"
#define WASTE_FILE   "data/waste.txt"

typedef enum {
    RC_OK = 0,
    RC_NOT_FOUND,     /* 대상 없음 */
    RC_INVALID,       /* 잘못된 입력값 */
    RC_NOT_ENOUGH,    /* 재료 부족 */
    RC_NO_MEMORY,     /* 메모리 할당 실패 */
    RC_FILE_ERROR     /* 파일 열기·쓰기 실패 */
} ResultCode;

typedef enum { FRIDGE = 0, FREEZER = 1 } Storage;   /* 냉장, 냉동 */

typedef struct {
    int year, month, day;
} Date;

typedef struct {
    char    name[NAME_LEN];
    Date    expire;
    int     quantity;
    Storage storage;
} Food;

typedef struct {
    Food *items;      /* 동적 배열 */
    int   count;
    int   capacity;   /* 가득 차면 realloc으로 2배 확장 */
} Fridge;

typedef struct {
    char name[NAME_LEN];
    char ingredients[MAX_ING][NAME_LEN];
    int  amounts[MAX_ING];    /* 재료별 필요 수량 */
    int  ingCount;
    int  isCustom;            /* 0: 기본 레시피, 1: 사용자 추가 */
} Recipe;

typedef struct {
    Recipe *items;
    int     count;
    int     capacity;
} RecipeBook;

typedef struct {
    int  recipeIdx;           /* RecipeBook 안의 인덱스 */
    int  matched;             /* 보유한 재료 수 */
    int  score;               /* 추천 점수 */
    int  canCook;             /* 1이면 바로 요리 가능 */
    int  missingCount;
    char missing[MAX_ING][NAME_LEN];   /* 부족한 재료 이름 */
    int  urgentCount;         /* 사용하면 좋은 임박 재료 수 (+5점씩) */
    char urgent[MAX_ING][NAME_LEN];    /* 임박 재료 이름 (화면의 "임박 재료: ..."에 사용) */
} Recommendation;

typedef struct {
    char    name[NAME_LEN];
    int     quantity;
    Storage storage;
    Date    expire;
    Date    discarded;        /* 폐기한 날짜 */
} WasteRecord;

typedef struct {
    WasteRecord *items;
    int          count;
    int          capacity;
} WasteList;

typedef struct {
    int  totalRecords;        /* 폐기 횟수 */
    int  totalQuantity;       /* 폐기한 재료 총 수량 */
    int  fridgeCount;         /* 냉장 재료 폐기 횟수 */
    int  freezerCount;        /* 냉동 재료 폐기 횟수 */
    int  thisMonth;           /* 이번 달 폐기 횟수 */
    char topName[NAME_LEN];   /* 가장 많이 버린 재료 */
    int  topCount;
} WasteStats;

typedef struct {              /* 화면 모듈이 들고 다니는 전체 데이터 */
    Fridge     fridge;
    RecipeBook recipes;
    WasteList  waste;
} AppData;

#endif
```

### 사용하는 C언어 핵심 개념

| 개념 | 활용 위치 |
|---|---|
| 제어문 | 메인 메뉴 반복(`while`, `switch`), 재료 매칭 반복문, D-day 조건 분기, GUI 메시지 분기 |
| 함수 분리(모듈화) | 로직 계층·화면 계층 분리, 기능별 `.c` / `.h` 파일 분리 |
| 배열과 문자열 | 레시피 재료 목록, `strcmp`로 재료 이름 비교, `strstr`로 검색 |
| 포인터 | 구조체를 포인터로 전달해 수정, 결과를 포인터 매개변수로 반환, 콜백 함수 포인터(`qsort`, `WndProc`) |
| 구조체 | `Food`, `Recipe`, `Recommendation`, `WasteRecord`, `WasteStats`, `AppData` |
| 파일 입출력 | `fridge.txt`, `recipes.txt`, `waste.txt` 저장·불러오기 |
| 동적 메모리 할당 | `malloc` / `realloc` / `free`로 목록 크기 자동 확장 |
| 기타 | `time.h` 날짜 계산, `enum` 상태값, `qsort` 정렬, Win32 API(GUI) |

---

## 12. 데이터 파일 형식

- 인코딩: **ANSI(EUC-KR, 코드 페이지 949)**로 저장합니다. (13.3 한글 처리 참고)
- 공백으로 구분된 텍스트 파일이며, 재료 이름에는 공백을 넣지 않습니다. (예: `돼지 고기`는 안 되고 `돼지고기`로 입력)

**`data/fridge.txt`**: `이름 유통기한 수량 보관(0=냉장, 1=냉동)`
```
계란 2026-11-12 6 0
대파 2026-11-10 1 0
돼지고기 2026-12-20 2 1
두부 2026-11-11 1 0
```

**`data/recipes.txt`**: `요리이름 사용자추가여부 재료개수 재료1 수량1 재료2 수량2 ...`
```
계란말이 0 2 계란 3 대파 1
계란볶음밥 0 3 계란 2 밥 1 대파 1
김치찌개 0 4 김치 1 돼지고기 1 두부 1 대파 1
된장찌개 0 4 된장 1 두부 1 애호박 1 양파 1
```

**`data/waste.txt`**: `이름 수량 보관 유통기한 폐기일`
```
우유 1 0 2026-11-05 2026-11-08
상추 1 0 2026-11-03 2026-11-08
```

---

## 13. 개발 환경 설정

### 13.1 설치 (팀원 전원)

1. **Visual Studio Community** 설치 (팀장 프로젝트의 플랫폼 도구 집합이 `v145`라 가능하면 같은 최신 버전을 설치. 오류가 나면 16장 참고)
2. 설치 화면의 워크로드에서 **"C++를 사용한 데스크톱 개발"** 선택
   (C 컴파일러와 Win32 API 개발 도구가 함께 설치됩니다.)

### 13.2 솔루션과 프로젝트 만들기 (팀장이 1회 진행 후 GitHub에 올림)

> **현재 상태:** `FridgeManager.slnx`와 `FridgeGUI` 프로젝트(`include/`, `src/`의 파일 포함)는 이미 만들어져 있습니다. 팀원은 이 절을 따라 할 필요가 없습니다. 새 파일은 팀장에게 요청합니다.

**GUI 프로젝트 (FridgeGUI)**
1. 솔루션 우클릭 → 추가 → 새 프로젝트 → **빈 프로젝트** → 이름 `FridgeGUI`
2. `src/`의 `.c` 파일(`main_gui.c`, `ui_gui.c`, `gui_*.c`, `dlg_*.c`, 로직 모듈)과 `include/`의 `.h` 파일을 기존 항목으로 추가
3. 프로젝트 속성 → C/C++ → 일반 → **추가 포함 디렉터리**에 `$(ProjectDir)..\include` 추가
4. 프로젝트 속성 → 링커 → 시스템 → **하위 시스템: Windows (/SUBSYSTEM:WINDOWS)**
5. 프로젝트 속성 → 고급 → **문자 집합: 유니코드 문자 집합 사용**
6. 공용 컨트롤(ListView)과 인터넷(WinHTTP) 라이브러리는 소스 상단의 `#pragma comment(lib, ...)`로 연결합니다. (`gui_common.c`, `guide_api.c`)
7. 파일 확장자는 반드시 **`.c`**로 합니다. (`.cpp`이면 C++로 컴파일됩니다.)

**공통 설정** (구성: **모든 구성**, 플랫폼: **모든 플랫폼**으로 선택 후 설정)
- C/C++ → 전처리기 → 전처리기 정의에 `_CRT_SECURE_NO_WARNINGS` 추가
  (`scanf`, `fopen` 사용 시 나오는 보안 경고 C4996 해제)
- 디버깅 → 작업 디렉터리를 **`$(SolutionDir)`**로 변경
  (실행할 때 솔루션 폴더의 `data/`를 찾을 수 있도록 함)

### 13.3 한글 처리

한국어 Windows의 기본 문자 코드는 **코드 페이지 949(EUC-KR)**입니다. 데이터 파일은 이 코드(ANSI)로 저장하고, 화면(Win32)은 유니코드를 쓰므로 표시·입력할 때 변환합니다.

| 파일 종류 | 저장 인코딩 | 이유 |
|---|---|---|
| 소스 파일 (`.c`, `.h`) | **유니코드(서명 있는 UTF-8)** | GitHub에서도 한글 주석이 깨지지 않음. Visual Studio가 문자열을 실행 시 949로 자동 변환 |
| 데이터 파일 (`.txt`) | **ANSI (EUC-KR)** | 로직의 문자열이 949라서 `strcmp` 비교가 정확함. 화면에 보일 때만 유니코드로 변환 |
| `README.md` | UTF-8 | GitHub 표시용 |

- 소스 파일 인코딩 변경: 파일 → 다른 이름으로 저장 → 저장 버튼 옆 화살표 → **인코딩하여 저장** → "유니코드(서명 있음)(UTF-8) - 코드 페이지 65001"
- 데이터 파일 인코딩 변경: 메모장에서 열기 → 다른 이름으로 저장 → 인코딩 **ANSI**
- 프로젝트 속성에 `/utf-8` 옵션은 **추가하지 않습니다.** (추가하면 소스의 한글 문자열이 깨질 수 있습니다.)

**GUI에서의 한글**: Win32 API는 유니코드(`wchar_t`) 함수(`...W`)를 사용합니다. 데이터 파일에서 읽은 문자열은 화면에 표시하기 전에 변환합니다.
```c
/* gui_common.c: ANSI(949) → 유니코드 변환 (로직 데이터를 화면에 표시할 때) */
void ansiToWide(const char *src, wchar_t *dst, int dstLen) {
    MultiByteToWideChar(CP_ACP, 0, src, -1, dst, dstLen);
}

/* gui_common.c: 유니코드 → ANSI(949) 변환 (입력창 값을 로직에 넘길 때) */
void wideToAnsi(const wchar_t *src, char *dst, int dstLen) {
    WideCharToMultiByte(CP_ACP, 0, src, -1, dst, dstLen, NULL, NULL);
}
```

### 13.4 `.gitignore`

Visual Studio가 만드는 빌드 결과물과 개인 설정 파일은 GitHub에 올리지 않습니다.
```
.vs/
x64/
Debug/
Release/
*.user
*.suo
*.obj
*.pdb
*.ilk
*.exe
```
> `.slnx`과 `.vcxproj`, `.vcxproj.filters` 파일은 반드시 커밋해야 팀원이 같은 프로젝트 구성을 받을 수 있습니다.

---

## 14. 빌드 및 실행 방법

1. GitHub 저장소를 클론한 뒤 `FridgeManager.slnx`를 Visual Studio로 엽니다.
2. 솔루션 탐색기에서 `FridgeGUI`를 우클릭 → **시작 프로젝트로 설정**
3. 구성을 **Debug / x64**로 두고 `Ctrl + F5` (디버깅하지 않고 시작) 또는 `F5` (디버깅 시작)

> 실행 시 솔루션 폴더 안에 `data/` 폴더가 있어야 합니다. `.exe`를 직접 실행해도 위쪽 폴더에서 `data/`를 찾아 이동합니다.

### 14.1 조리법 자동 불러오기 설정 (API 인증키)

1. 식품안전나라(foodsafetykorea.go.kr) 오픈API에서 **조리식품의 레시피 DB(COOKRCP01)** 이용 신청 후 인증키를 발급받습니다.
2. `data/api_key.example.txt`를 복사해 `data/api_key.txt`를 만들고, 인증키를 **첫 줄에 영문·숫자만** 적습니다. (또는 환경변수 `FOODSAFETY_API_KEY`) 파일에는 키 한 줄만 넣고, 설명 줄이나 따옴표, 공백은 넣지 마세요.
3. 프로그램을 실행해 **요리 추천** 창을 열면 **인터넷 추천**으로 열립니다. 내 재료 이름으로 식품안전나라 레시피를 찾아 보여 주고(양은 비교하지 않음), 저장된 레시피 추천에서 조리 순서가 없는 요리를 고르면 잠시 뒤 자동으로 불러와 `data/steps.txt`에 저장합니다.
4. `조리법 불러오기` 버튼으로 다시 받거나 이미 저장된 조리 순서를 인터넷 것으로 바꿀 수 있습니다.

> `data/api_key.txt`는 `.gitignore`에 들어 있어 GitHub에 올라가지 않습니다. **인증키를 코드나 README에 적지 마세요.**

---

## 15. 실행 화면 예시

### 15.1 콘솔 버전 (제거됨, 설계 당시 참고용)

**프로그램 시작 시 알림**
```
╔══════════════════════════════════════╗
║           [ 유통기한 알림 ]           ║
╠══════════════════════════════════════╣
║  오늘 날짜: 2026-11-09               ║
║                                      ║
║  유통기한 지난 재료: 1개             ║
║   - 우유 (D+1)                       ║
║  2일 이내 임박 재료: 3개             ║
║   - 대파 (D-1)                       ║
║   - 두부 (D-2)                       ║
║   - 계란 (D-2)                       ║
╚══════════════════════════════════════╝
      아무 키나 누르면 메뉴로 이동합니다
```

**메인 메뉴**
```
============ 냉장고 관리 ============
 1. 식재료 등록
 2. 식재료 목록 (임박순)
 3. 식재료 수정 / 삭제 / 수량 변경
 4. 오늘 뭐 먹지? (요리 추천)
 5. 레시피 추가
 6. 기한 지난 재료 일괄 폐기
 7. 폐기 기록 통계
 0. 저장 후 종료
=====================================
선택 >
```

**식재료 목록 (임박순)**
```
번호  이름        수량  보관   유통기한     남은 기간
-------------------------------------------------
 1   우유         1   냉장   2026-11-08   D+1 (지남)
 2   대파         1   냉장   2026-11-10   D-1
 3   두부         1   냉장   2026-11-11   D-2
 4   계란         6   냉장   2026-11-11   D-2
 5   돼지고기     2   냉동   2026-12-20   D-41
```

**요리 추천**
```
===== 오늘의 추천 요리 =====
1. 계란말이     [재료 2/2] [바로 가능]  (임박 재료: 계란, 대파)
2. 계란볶음밥   [재료 2/3]  부족: 밥
3. 김치찌개     [재료 3/4]  부족: 김치

만들 요리 번호를 입력하세요 (0: 돌아가기) > 1
계란 3개, 대파 1개를 사용합니다. 만들까요? (y/n) > y
[완료] 재료가 차감되었습니다. (계란 6 → 3, 대파 1 → 0, 대파 삭제)
```

**폐기 기록 통계**
```
=========== 폐기 기록 통계 ===========
총 폐기 횟수       : 7회 (재료 9개)
냉장 / 냉동 비율   : 냉장 86% / 냉동 14%
가장 많이 버린 재료: 우유 (3회)
이번 달 폐기 횟수  : 4회
--------------------------------------
[팁] 냉장 재료 폐기가 많아요. 장보기 양을 조금 줄여 보세요!
```

### 15.2 GUI 버전

**G1. 시작 알림 팝업**
```
┌─ 유통기한 알림 ────────────────────┐
│                                    │
│  (!)  유통기한 지난 재료: 1개       │
│       2일 이내 임박 재료: 3개       │
│                                    │
│                         [ 확인 ]   │
└────────────────────────────────────┘
```
```c
/* G1 구현 예시 (ui_gui.c) */
void guiShowStartupAlert(const Fridge *f) {
    int expired, soon;
    wchar_t msg[200];
    countAlerts(f, &expired, &soon);          /* 로직 함수 재사용 */
    if (expired == 0 && soon == 0) return;
    swprintf(msg, 200, L"유통기한 지난 재료: %d개\n2일 이내 임박 재료: %d개", expired, soon);
    MessageBoxW(NULL, msg, L"유통기한 알림", MB_OK | MB_ICONWARNING);
}
```

**G2~G3. 메인 창**
```
┌─ 냉장고 관리 ─────────────────────────────────────────────┐
│ ┌──────────────────────────────────────────┐ ┌──────────┐ │
│ │ 이름     │ 수량 │ 보관 │ 유통기한   │ 남은 │ │ 식재료   │ │
│ │──────────┼──────┼──────┼────────────┼──────│ │  등록    │ │
│ │ 우유     │  1   │ 냉장 │ 2026-11-08 │ D+1  │ ├──────────┤ │
│ │ 대파     │  1   │ 냉장 │ 2026-11-10 │ D-1  │ │ 수정     │ │
│ │ 두부     │  1   │ 냉장 │ 2026-11-11 │ D-2  │ ├──────────┤ │
│ │ 계란     │  6   │ 냉장 │ 2026-11-11 │ D-2  │ │ 삭제     │ │
│ │ 돼지고기 │  2   │ 냉동 │ 2026-12-20 │ D-41 │ ├──────────┤ │
│ │          │      │      │            │      │ │ 요리 추천│ │
│ │          │      │      │            │      │ ├──────────┤ │
│ │          │      │      │            │      │ │ 폐기 통계│ │
│ └──────────────────────────────────────────┘ └──────────┘ │
│  전체 5개 · 기한 지남 1개 · 임박 3개                       │
└────────────────────────────────────────────────────────────┘
```

**G3. 등록 대화상자**
```
┌─ 식재료 등록 ───────────────────┐
│  이름      [ 계란             ] │
│  유통기한  [ 2026-11-20       ] │
│  수량      [ 6   ]              │
│  보관      (●) 냉장  ( ) 냉동   │
│                                 │
│              [ 등록 ] [ 취소 ]  │
└─────────────────────────────────┘
```

**G4. 요리 추천 창**
```
┌─ 오늘 뭐 먹지? ────────────────────────────────┐
│  요리         │ 재료  │ 상태                     │
│───────────────┼───────┼──────────────────────────│
│  계란말이     │ 2/2   │ 바로 가능 (임박 재료)    │
│  계란볶음밥   │ 2/3   │ 부족: 밥                 │
│  김치찌개     │ 3/4   │ 부족: 김치               │
│                                                 │
│        [ 요리하기 ]  [ 레시피 추가 ]  [ 닫기 ]   │
└─────────────────────────────────────────────────┘
```

---

## 16. 자주 막히는 문제

| 증상 | 원인 | 해결 |
|---|---|---|
| `v145 플랫폼 도구 집합을 찾을 수 없습니다` | 팀장의 Visual Studio가 더 최신 버전 | 팀장에게 알리기. 급하면 프로젝트 속성 → 일반 → 플랫폼 도구 집합을 설치된 것(`v143` 등)으로 바꿔 실행하되, **커밋 전에 `git restore FridgeGUI/FridgeGUI.vcxproj`로 되돌리기** |
| 한글이 `?`나 깨진 글자로 나옴 | 소스 인코딩 문제 | 소스는 UTF-8(서명 있음), 데이터는 ANSI. `/utf-8` 옵션은 추가하지 않기 (4.6, 13.3) |
| `error C4996: 'scanf': This function may be unsafe` | 보안 경고가 오류로 처리됨 | 프로젝트 속성 → C/C++ → 전처리기에 `_CRT_SECURE_NO_WARNINGS` (팀장이 설정. 안 되어 있으면 알리기) |
| 프로그램이 `data/fridge.txt`를 못 찾음 | 작업 디렉터리가 다름 | 속성 → 디버깅 → 작업 디렉터리 `$(SolutionDir)`. `.exe`를 직접 더블클릭하면 안 됨 |
| `LNK2019: 확인할 수 없는 외부 기호` | 함수 선언만 있고 정의가 없음, 또는 `.c`가 프로젝트에 없음 | 함수 이름·인자가 `.h`와 `.c`에서 똑같은지 확인. 파일 추가는 팀장에게 요청 |
| `LNK2005: 이미 정의되었습니다` | 헤더에 함수 본문을 넣음 | 본문은 `.c`에, `.h`에는 선언만 |
| 새 파일이 C++로 컴파일됨 (`error C2440` 등) | 확장자가 `.cpp` | `.c`로 만들기 |
| 실행하자마자 창이 닫힘 | `F5`로 실행 | `Ctrl + F5` 사용 |
| `fatal error C1083: 포함 파일을 열 수 없습니다 (…h)` | 헤더를 `include/`가 아닌 곳에 만들었거나 포함 경로가 빠짐 | `.h`는 `include/`에 두고, 프로젝트 속성의 추가 포함 디렉터리에 `$(ProjectDir)..\include`가 있는지 확인 |
| 실행 중 갑자기 종료 (`Access violation`) | `NULL` 포인터, 배열 범위 밖 접근 | 인자 `NULL` 검사, `idx < count` 검사. 디버거(`F5`)로 멈춘 줄 확인 |
| 재료를 많이 넣으면 죽음 | `realloc` 결과를 잘못 처리 | 임시 변수에 받고 `NULL` 검사 후 대입 (4.5) |
| 이름 검색이 안 됨 | 이름 끝에 개행(`\n`)이 붙어 있음 | `fgets`로 받은 문자열의 `\n` 제거 |
| `git push`가 거절됨 | 내 브랜치가 오래됨 | `git pull origin 내브랜치` 후 다시 push |
| `.vcxproj`에서 충돌이 남 | 누군가 프로젝트 파일을 수정함 | 직접 해결하지 말고 팀장에게 알리기 |
| `x64/`, `.vs/` 같은 폴더가 `git status`에 나옴 | `.gitignore`에 없는 빌드 결과물 | 커밋하지 말고 팀장에게 알리기. `git add 파일이름`으로 내가 고친 파일만 추가 |

---

## 17. 완료 체크리스트

### 기능 (GUI v1.0)
- [ ] F1 식재료 등록, 같은 재료 수량 합치기 (박지연 `food`, 김기범 `dlg_food`)
- [ ] F2 D-day 계산과 임박순 정렬 (김기범 `expiry`)
- [ ] F3 요리 추천과 레시피 추가, 기본 레시피 20개 이상 (최민솔 `recipe`, `dlg_recipe`)
- [ ] F4 시작 시 임박 알림 (김기범 `expiry`, 윤종하 `ui_gui`)
- [ ] F5 수정·삭제·수량 변경, 요리 시 자동 차감 (박지연 `food`, 최민솔 `recipe`, 장태현 `dlg_simple`)
- [ ] F6 폐기 기록, 일괄 폐기, 통계 (박지연 `stats`, 장태현 `dlg_simple`)
- [ ] 파일 저장·불러오기, 껐다 켜도 데이터 유지 (윤종하 `fileio`)
- [ ] 메인 창과 전체 연결 (윤종하 `ui_gui`, `main_gui`)

### 코드 규칙
- [ ] 로직 모듈(`food`, `expiry`, `recipe`, `stats`, `fileio`, `guide`)에 창·대화상자·`printf`·`scanf`가 없다
- [ ] `malloc`/`realloc` 뒤 `NULL` 검사를 했고 종료 전 `free` 한다
- [ ] 모든 함수에 한글 설명 주석, 파일 맨 위에 담당자 표시가 있다
- [ ] 각자 6장의 "혼자 확인하기"를 실행해 보고 결과를 PR에 적었다
- [ ] 소스는 UTF-8(서명 있음), 데이터는 ANSI다

### 협업 (모든 팀원)
- [ ] 모든 팀원이 PR로 기여한 기록이 GitHub에 있다
- [ ] 다른 팀원의 PR을 1개 이상 리뷰했다
- [ ] 모든 팀원이 자기 담당 코드를 그 자리에서 설명하고 수정할 수 있다
- [ ] `main`은 항상 빌드·실행이 된다

### 제출물
- [ ] 제안보고서 (윤종하), 11/15
- [ ] 흐름도 (김기범)
- [ ] 테스트 시나리오·결과서 (박지연)
- [ ] 회의록, WBS (윤종하)
- [ ] 결과보고서 (윤종하가 취합, 각자 자기 모듈 설명 제출)
- [ ] 발표 PPT (윤종하), 최종 발표·데모 시연 (장태현)
- [ ] 12/13 `v1.0` 태그
- [ ] (도전) API로 조리 방법 자동 불러오기 (인증키 설정 후 요리 추천 창에서 확인)

---

## 18. 참고 자료
- 수업 교재: `교재명 / 저자 / 출판사`
- C 표준 라이브러리 `time.h`: https://en.cppreference.com/w/c/chrono
- `qsort`: https://en.cppreference.com/w/c/algorithm/qsort
- Visual Studio에서 C 프로그램 만들기 (Microsoft Learn): https://learn.microsoft.com/ko-kr/cpp/build/walkthrough-compile-a-c-program-on-the-command-line
- Win32 API 시작하기 (Microsoft Learn): https://learn.microsoft.com/ko-kr/windows/win32/learnwin32/learn-to-program-for-windows
- ListView 컨트롤 (Microsoft Learn): https://learn.microsoft.com/ko-kr/windows/win32/controls/list-view-controls-overview
