# 냉장고 식재료 관리 프로그램 (Fridge Manager)

> 냉장고 속 식재료의 유통기한을 관리하고, 먼저 먹어야 할 재료를 알려 주며, 남은 재료로 만들 수 있는 요리를 추천하는 C언어 프로그램
> **콘솔 버전(필수)**을 먼저 완성하고, 같은 로직을 재사용해 **GUI 버전(도전)**으로 확장합니다.

- 과목: 비판적사고 (홍서빈 교수님)
- 팀명: TEAM 1 (윤종하, 장태현, 김기범, 최민솔, 박지연)
- 개발 언어: C
- 개발 환경: Visual Studio / Windows 10 이상
- GUI: Win32 API (Windows 기본 제공, 추가 라이브러리 설치 없음)
- 저장소: https://github.com/dontcal1me/FridgeManager

이 문서는 팀원들이 **각자 맡은 파일을 따로 개발하고, 나중에 문제없이 합칠 수 있도록** 정리한 작업 안내서입니다.
작업을 시작하기 전에 2장부터 5장까지는 꼭 읽고, 6장에서 **내 이름이 붙은 절**을 찾아 그대로 따라 하세요.

| 나는 | 내 담당 | 먼저 볼 곳 | 가장 먼저 할 일 |
|---|---|---|---|
| 윤종하 (팀장) | `common.h`, `main.c`, `ui_console`, `fileio` | [6.1](#61-윤종하-팀장) | 11/23까지 뼈대 코드 업로드 |
| 장태현 | `util` + 최종 발표 | [6.2](#62-장태현-util) | 11/29까지 `util.c` PR |
| 김기범 | `expiry` + 흐름도 | [6.3](#63-김기범-expiry) | 11/25까지 `getToday`, `isValidDate`, `daysLeft` PR |
| 최민솔 (부팀장) | `recipe`, `recipes.txt` | [6.4](#64-최민솔-recipe) | 11/25까지 `initRecipeBook`, `addRecipe`, `freeRecipeBook` PR |
| 박지연 | `food`, `stats` + 테스트 | [6.5](#65-박지연-food-stats) | 11/25까지 `initFridge`, `addFood`, `freeFridge`와 `initWasteList`, `appendWaste`, `addWaste`, `freeWasteList` PR |

---|---|---|
| 윤종하 (팀장) | [6.1](#61-윤종하-팀장) | 11/23까지 뼈대 코드 업로드 |
| 장태현 | [6.2](#62-장태현-food-util) | 11/25까지 `initFridge`, `addFood`, `freeFridge` PR |
| 김기범 | [6.3](#63-김기범-expiry) | 11/25까지 `getToday`, `isValidDate`, `daysLeft` PR |
| 최민솔 (부팀장) | [6.4](#64-최민솔-recipe) | 11/25까지 `initRecipeBook`, `addRecipe`, `freeRecipeBook` PR |
| 박지연 | [6.5](#65-박지연-stats-fileio) | 11/25까지 `initWasteList`, `appendWaste`, `addWaste`, `freeWasteList` PR |

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

모두가 **자기 담당 파일을 직접 코딩하고, 커밋·푸시하고, Pull Request를 올립니다.** 파일 양은 사람마다 다르게 나눴습니다. 발표를 맡은 장태현은 가장 작은 모듈 1개를 맡습니다.

| 이름 | 역할 | 담당 파일 | 개발 외 업무 | 브랜치 |
|---|---|---|---|---|
| 윤종하 | 팀장 · 통합 · 일정 · 문서 | `common.h`, `main.c`, `ui_console.c/h` (모든 화면), `fileio.c/h` | GitHub 관리·병합, 회의록·WBS, 제안·결과보고서, README, 발표 PPT | `feature/skeleton`, `feature/main`, `feature/fileio` |
| 장태현 | 구현(작은 모듈) · 발표 | `util.c/h` | **최종 발표·데모 시연**, 예상 질문과 답변 정리 | `feature/util` |
| 김기범 | 구현 | `expiry.c/h` (F2, F4) | 프로그램 흐름도 | `feature/expiry` |
| 최민솔 | 부팀장 · 구현 | `recipe.c/h` (F3, 자동 차감), `data/recipes.txt` (20개 이상) | PR 검토 대행, 데모용 샘플 데이터 준비 | `feature/recipe` |
| 박지연 | 구현 · 테스트 | `food.c/h` (F1, F5), `stats.c/h` (F6) | 테스트 시나리오·결과서 | `feature/food`, `feature/stats` |

**자기 담당 파일만 수정합니다.** 다른 사람 파일을 고쳐야 하면 먼저 단톡방에 말하고, 그 사람이 고치거나 허락을 받은 뒤 고칩니다.

| 공유 파일 | 규칙 |
|---|---|
| `common.h`, 모든 `.h` 함수 선언 | 확정 후에는 **윤종하만** 수정. 바꾸고 싶으면 단톡방에 먼저 제안 |
| `FridgeManager.slnx`, `FridgeConsole.vcxproj` | 윤종하만 수정. 파일 추가가 필요하면 요청 |
| `main.c`, `ui_console.c` | 윤종하만 수정 |

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
| **구현 1주차**<br>11/23~11/29 | **11/23(월) 뼈대 코드 업로드** (모든 `.h` 완성, `.c`는 빈 껍데기, 빌드 통과), `main.c`·메뉴 | **11/29(일)** `util.c` 전체 PR | **11/25(수)** `getToday`·`isValidDate`·`daysLeft` PR<br>**11/29(일)** `expiry.c` 전체 PR | **11/25(수)** `initRecipeBook`·`addRecipe`·`freeRecipeBook` PR, `recipes.txt` 작성 | **11/25(수)** `initFridge`·`addFood`·`freeFridge`, `initWasteList`·`appendWaste`·`addWaste`·`freeWasteList` PR<br>**11/29(일)** `food.c` 전체 PR |
| **구현 2주차**<br>11/30~12/06 | PR 검토·병합, `fileio.c`와 모든 화면(`ui_console.c`) 작성, 연결 테스트 | 발표 시연 순서 정리 | 흐름도 최종 | **12/03(목)** `recommend`·`cookRecipe` PR, `recipes.txt` 20개 완성 | **12/03(목)** `stats.c` 전체 PR, 테스트 시나리오 작성<br>**12/06(일)** 전원 병합 완료, 콘솔 버전 실행 가능 |
| **통합 주**<br>12/07~12/13 | 통합·디버깅, **12/13(일) v1.0 태그** | 시연 리허설, 예상 질문 정리 | 버그 수정, 자기 코드 설명 연습 | 버그 수정, 데모 샘플 데이터 | 통합 테스트 실행, 버그를 Issues에 등록, 테스트 결과서 |
| **납품 주**<br>12/14~ | 결과보고서 취합, 발표 PPT | **최종 발표·데모 시연** | 보고서에 자기 모듈 설명 전달 | 보고서에 자기 모듈 설명 전달 | 테스트 결과서 최종, 보고서에 자기 모듈 설명 전달 |

GUI(도전)는 **v1.0이 완성된 뒤** 윤종하가 시간이 되면 진행합니다. 팀원이 맡을 일은 없습니다.

### 2.4 왜 11/25에 먼저 올리나요?

다른 사람의 함수를 부르는 모듈이 있기 때문입니다. 아래 함수는 **가장 먼저** 올려서 다른 팀원이 막히지 않게 합니다.

| 먼저 올릴 함수 | 담당 | 이 함수를 쓰는 사람 |
|---|---|---|
| `initFridge`, `addFood`, `freeFridge` | 박지연 | 윤종하(`loadFridge`, 화면), 김기범(`testExpiry`), 최민솔(`testRecipe`) |
| `initWasteList`, `appendWaste`, `addWaste`, `freeWasteList` | 박지연 | 윤종하(`loadWaste`, 화면) |
| `getToday`, `daysLeft`, `isValidDate` | 김기범 | 장태현(`inputDate`), 최민솔(`recommend`), 박지연(`discardExpired`), 윤종하(화면) |
| `initRecipeBook`, `addRecipe`, `freeRecipeBook` | 최민솔 | 윤종하(`loadRecipes`, 화면) |

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
| 로직 | `food`, `expiry`, `recipe`, `stats`, `fileio` | 계산, 구조체 수정, 파일 읽기·쓰기 | `printf`, `scanf`, `getchar`, Win32 API |
| 화면 | `main`, `ui_console`, `util` | 입출력, 로직 함수 호출 | 구조체 내부(`f->items[i].quantity = ...`)를 직접 고치기 |

값은 **매개변수**로 받고, 결과는 **반환값이나 포인터**로 돌려줍니다. 어떤 메시지를 보여 줄지는 화면이 정합니다. (파일 읽기·쓰기를 위한 `fopen`은 `fileio`에서 사용해도 됩니다.)

### 4.2 데이터 표현 약속

| 항목 | 약속 |
|---|---|
| 식재료 위치 | `Fridge.items` 배열의 인덱스 `idx` (0부터). 화면의 번호는 `idx + 1` |
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
 * 담당: 장태현
 * 설명: 식재료 등록·수정·삭제·수량 변경·검색 (F1, F5)
 */
```

`ui_console.c`는 윤종하만 수정합니다.

### 4.6 인코딩 (한글 깨짐 방지)

| 파일 종류 | 저장 인코딩 |
|---|---|
| 소스 (`.c`, `.h`) | **UTF-8 (서명 있음)** |
| 데이터 (`data/*.txt`) | **ANSI (EUC-KR)**. 메모장 → 다른 이름으로 저장 → 인코딩 ANSI |
| `README.md` | UTF-8 |

프로젝트 속성에 `/utf-8` 옵션은 **추가하지 않습니다.** (콘솔 한글 출력이 깨집니다.) 자세한 내용은 13.3장.

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
- 빌드가 안 되거나 실행 중 죽는 코드를 PR로 올리지 않기 (최소한 내 `--test` 테스트는 통과)
- `.vcxproj`, `.slnx` 파일을 임의로 고쳐서 커밋하지 않기

### 5.6 충돌(conflict)이 났을 때

`git merge main` 후 충돌 메시지가 나오면 Visual Studio나 VS Code에서 해당 파일을 엽니다. `<<<<<<<`, `=======`, `>>>>>>>` 표시가 있는 부분에서 맞는 쪽을 고른 뒤 표시를 모두 지우고, 저장하고 다시 커밋합니다. 잘 모르겠으면 아무것도 지우지 말고 팀장에게 바로 물어봅니다.

---

## 6. 팀원별 작업 안내

각 절에는 **만들어야 할 것(정확한 이름과 반환값)**, **구현 요점**, **완료 기준**, **혼자 테스트하는 방법**이 있습니다. 코드 블록의 `...` 부분을 직접 채우면 됩니다.

### 6.0 시작하기 전에

#### 의존 순서

```
common.h (윤종하, 전원 확정)
   │
   ├── food (박지연) ──────┐
   ├── expiry (김기범) ────┤
   ├── stats (박지연) ─────┼──> fileio (윤종하: addFood, addRecipe, appendWaste 사용)
   ├── util (장태현) ──────┤      └──> recipe (최민솔: expiry, food 사용)
   │                       │
   └── main.c / ui_console (윤종하: 모든 모듈 연결)
```

`food`, `expiry`, `stats`, `util`은 서로 독립적이라 **동시에** 개발할 수 있습니다. 2.4장의 "먼저 올릴 함수"만 11/25까지 올리면 나머지는 막힘 없이 진행됩니다. (`util`의 `inputDate`만 김기범의 `isValidDate`를 사용합니다.)

#### 혼자 테스트하는 방법 (`--test`)

콘솔 프로그램은 `main()`이 하나뿐이라, 각 모듈은 **`src/test_모듈.c`의 테스트 함수**로 따로 확인합니다. 팀장이 뼈대에 아래 파일을 만들어 둡니다. (이름 그대로, 내 모듈 파일만 채웁니다.)

| 테스트 파일 | 함수 | 채우는 사람 |
|---|---|---|
| `src/test_util.c` | `void testUtil(void)` | 장태현 |
| `src/test_expiry.c` | `void testExpiry(void)` | 김기범 |
| `src/test_recipe.c` | `void testRecipe(void)` | 최민솔 |
| `src/test_food.c` | `void testFood(void)` | 박지연 |
| `src/test_stats.c` | `void testStats(void)` | 박지연 |
| `src/test_fileio.c` | `void testFileio(void)` | 윤종하 |

실행 방법: 프로젝트 속성 → 디버깅 → **명령 인수**에 `--test food`를 입력하고 `Ctrl + F5`. (`util`, `expiry`, `recipe`, `food`, `stats`, `fileio` 중 하나. 이 설정은 내 컴퓨터에만 저장됩니다. 테스트가 끝나면 비우세요.)

테스트 파일에서는 `test.h`의 `CHECK`를 씁니다. 통과하면 `[PASS]`, 실패하면 `[FAIL]`과 실패한 조건이 출력됩니다. **테스트 파일은 로직 모듈이 아니므로 `printf`를 써도 됩니다.**

```c
/* test.h */
#include <stdio.h>
#define CHECK(cond) printf("%s  %s\n", (cond) ? "[PASS]" : "[FAIL]", #cond)

int runTest(const char *name);   /* main.c에 있음. "food" 등을 받아 해당 테스트 함수 호출 */
```

---

### 6.1 윤종하 (팀장)

**담당:** `common.h`, `main.c`, `ui_console.c/h`(모든 화면), `fileio.c/h`, `test.h`와 `test_*.c` 틀, 프로젝트 설정
**브랜치:** `feature/skeleton`, `feature/main`, `feature/fileio`

#### 11/23(월)까지: 뼈대 코드 업로드 (가장 중요)

팀원 전원이 이것을 받아서 시작합니다. 아래를 모두 갖춘 상태로 `main`에 올립니다.

- [ ] `common.h`: 11장 그대로 + 4.3장의 파일 경로 상수
- [ ] 로직 모듈 5개의 `.h`: 6.2~6.5장에 적힌 함수 선언을 **그대로** 복사
- [ ] 각 `.c`: 함수마다 컴파일만 되는 빈 껍데기 (반환값만 `return RC_OK;`, `return 0;` 등)
- [ ] `ui_console.h/.c`: 화면 함수 빈 껍데기
- [ ] `util.h/.c`: 함수 선언과 빈 껍데기
- [ ] `test.h`, `test_util.c`, `test_food.c`, `test_expiry.c`, `test_recipe.c`, `test_stats.c`, `test_fileio.c`: 빈 `testXxx()` 함수
- [ ] `.vcxproj`: 위 `test_*.c` 파일 추가, `_CRT_SECURE_NO_WARNINGS` 정의 추가, 디버깅 작업 디렉터리를 `$(SolutionDir)`로 설정 (13.2장)
- [ ] `data/` 폴더: `fridge.txt`, `waste.txt` (샘플 몇 줄), `recipes.txt` (빈 파일. 최민솔이 채움). **ANSI로 저장**
- [ ] 빌드와 실행이 오류 없이 되는지 확인한 뒤 단톡방에 공지

#### main.c

```c
/* main.c
 * 담당: 윤종하
 */
#include <string.h>
#include "common.h"
#include "food.h"
#include "recipe.h"
#include "stats.h"
#include "fileio.h"
#include "ui_console.h"
#include "test.h"

int main(int argc, char *argv[])
{
    AppData app;

    if (argc == 3 && strcmp(argv[1], "--test") == 0)
        return runTest(argv[2]);              /* 모듈 단독 테스트 */

    initFridge(&app.fridge);
    initRecipeBook(&app.recipes);
    initWasteList(&app.waste);

    loadFridge(FRIDGE_FILE, &app.fridge);
    loadRecipes(RECIPE_FILE, &app.recipes);
    loadWaste(WASTE_FILE, &app.waste);

    showStartupAlert(&app.fridge);            /* F4 */
    runMainMenu(&app);                        /* 0을 고를 때까지 반복 */

    saveFridge(FRIDGE_FILE, &app.fridge);
    saveRecipes(RECIPE_FILE, &app.recipes);
    saveWaste(WASTE_FILE, &app.waste);

    freeFridge(&app.fridge);
    freeRecipeBook(&app.recipes);
    freeWasteList(&app.waste);
    return 0;
}
```

`runTest`도 `main.c`에 둡니다. `"util"`이면 `testUtil()`, `"food"`면 `testFood()` 식으로 `strcmp`로 나눠 호출하고, 모르는 이름이면 사용법을 출력하고 `1`을 반환합니다.

#### ui_console.c: 메인 메뉴

```c
void runMainMenu(AppData *app)
{
    int choice;
    do {
        printMainMenu();                         /* 15장의 메인 메뉴 화면 출력 */
        choice = inputInt("선택 > ", 0, 7);
        switch (choice) {
        case 1: screenAddFood(app);         break;   /* 장태현 */
        case 2: screenFoodList(app);        break;   /* 김기범 */
        case 3: screenEditFood(app);        break;   /* 장태현 */
        case 4: screenRecommend(app);       break;   /* 최민솔 */
        case 5: screenAddRecipe(app);       break;   /* 최민솔 */
        case 6: screenDiscardExpired(app);  break;   /* 박지연 */
        case 7: screenWasteStats(app);      break;   /* 박지연 */
        }
    } while (choice != 0);
}
```

#### ui_console.c: 모든 화면 함수

화면은 모두 윤종하가 작성합니다. 사용하는 로직 함수는 담당 팀원이 만든 것이므로 이름과 반환값을 6.2~6.5장 그대로 호출합니다. 화면 예시는 15장입니다.

##### 식재료 등록·수정·삭제 화면

```c
void screenAddFood(AppData *app);    /* 메뉴 1: 이름, 유통기한, 수량, 보관 방법을 입력받아 addFood */
void screenEditFood(AppData *app);   /* 메뉴 3: 수정 / 삭제 / 수량 변경 */
```

- `screenAddFood`: `inputWord`, `inputDate`, `inputInt`(수량 1~999), 보관 방법(0 냉장, 1 냉동)을 받아 `addFood`를 호출하고, 반환값에 맞는 메시지를 출력합니다. (`RC_OK`면 "등록되었습니다", 같은 재료를 합쳤으면 합쳤다고 안내하면 더 좋습니다.)
- `screenEditFood`: `sortByExpiry`(김기범) 후 `printFoodTable`(김기범)로 목록을 보여 주고, **화면 번호 − 1 = `idx`**로 재료를 고릅니다(0은 취소). 이어서 `1 수정 / 2 삭제 / 3 수량 변경`을 처리합니다.
  - 삭제: "폐기로 기록할까요? (y/n)"를 묻고, `y`면 `addWaste(&app->waste, &item, getToday())` 후 `removeFood`, `n`이면 `removeFood`만 (F6 연결)
  - 수량 변경: 줄이면 `useFood`, 늘리면 `updateFood`


##### 시작 알림·목록 화면

```c
void showStartupAlert(const Fridge *f);     /* F4: 시작 알림 박스. 마지막에 waitForKey() */
void printFoodTable(const Fridge *f);       /* 목록 표 출력. 번호는 1부터 (idx + 1) */
void screenFoodList(AppData *app);          /* 메뉴 2: sortByExpiry 후 printFoodTable */
```

- `showStartupAlert`: 15장의 테두리 박스 형태. 기한 지난 재료·임박 재료의 **이름과 D-day**를 나열합니다. 둘 다 0이면 박스 대신 "임박한 재료가 없습니다" 한 줄만 보여 줍니다.
- `printFoodTable`: 15장의 표 형식. 남은 기간 칸은 `formatDday`로 만들고 기한이 지났으면 ` (지남)`, 임박이면 ` (임박)`을 붙입니다. 목록이 비었으면 "등록된 식재료가 없습니다".
- `printFoodTable`은 **장태현의 `screenEditFood`에서도 부르므로** 함수 이름과 인자를 바꾸지 않습니다. 번호 = 배열 인덱스 + 1이어야 합니다.


##### 요리 추천·레시피 추가 화면

```c
void screenRecommend(AppData *app);   /* 메뉴 4: 추천 목록 → 번호 선택 → 확인 → cookRecipe */
void screenAddRecipe(AppData *app);   /* 메뉴 5: 사용자 레시피 추가 */
```

- `screenRecommend`: `Recommendation out[20]`에 `recommend`를 호출해 15장의 화면대로 출력합니다. 번호를 고르면 "계란 3개, 대파 1개 사용. 만들까요? (y/n)"를 묻고, `y`면 `cookRecipe`를 호출한 뒤 "계란 6 → 3, 대파 1 → 0 (삭제)"처럼 **차감 전후 수량**을 보여 줍니다. `canCook`이 0인 요리를 고르면 부족한 재료를 안내합니다. 추천이 0개면 "추천할 요리가 없습니다".
- `screenAddRecipe`: 요리 이름 → 재료 개수(1~`MAX_ING`) → 재료마다 이름·수량을 입력받아 `Recipe`를 채우고 **`isCustom = 1`**로 `addRecipe`를 호출합니다.


##### 폐기·통계 화면

```c
void screenDiscardExpired(AppData *app);        /* 메뉴 6: 기한 지난 재료 일괄 폐기 */
void screenWasteStats(const AppData *app);      /* 메뉴 7: 폐기 기록 통계 */
```

- `screenDiscardExpired`: `countAlerts`로 기한 지난 재료 개수를 보여 주고 "N개를 폐기 처리할까요? (y/n)". `y`면 `discardExpired`를 호출하고 폐기 개수를 출력합니다. 0개면 "기한 지난 재료가 없습니다".
- `screenWasteStats`: `computeWasteStats(&app->waste, getToday(), &s)`를 호출하고 15장의 화면대로 출력합니다. 냉장·냉동 비율은 `fridgeCount * 100 / totalRecords`로 계산하되, **`totalRecords == 0`이면 0으로 나누지 말고** "아직 폐기 기록이 없습니다"를 출력합니다. 마지막 줄의 팁은 냉장 비율 등 조건에 따라 문구를 정하세요.



#### fileio.c/h

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

##### 혼자 테스트하기 (`--test fileio`)

```c
/* test_fileio.c : 저장 → 불러오기 왕복 확인 */
#include "test.h"
#include "fileio.h"
#include "food.h"

void testFileio(void)
{
    Fridge a, b;
    Food egg = { "계란", { 2026, 11, 12 }, 6, FRIDGE };

    initFridge(&a);
    initFridge(&b);
    addFood(&a, &egg);

    CHECK(saveFridge("data/test_fridge.txt", &a) == RC_OK);
    CHECK(loadFridge("data/test_fridge.txt", &b) == RC_OK);
    CHECK(b.count == 1 && b.items[0].quantity == 6);
    CHECK(loadFridge("data/없는파일.txt", &b) == RC_OK);    /* 파일이 없어도 RC_OK */
    CHECK(saveFridge("없는폴더/x.txt", &a) == RC_FILE_ERROR);

    freeFridge(&a);
    freeFridge(&b);
}
```

`testFileio`가 만든 `data/test_fridge.txt`는 커밋하지 말고 삭제합니다.

##### fileio 완료 기준
- 저장한 뒤 다시 불러오면 항목 수와 값이 저장 전과 똑같다 (재료, 레시피, 폐기 기록 모두)
- 데이터 파일이 없어도 프로그램이 정상 시작한다
- 한글 이름이 저장·불러오기 후에도 깨지지 않는다 (파일이 **ANSI**여야 함)

#### 완료 기준
- 뼈대만 있는 상태에서도 메뉴가 뜨고, 모든 번호가 오류 없이 돌아온다
- 0을 입력하면 세 파일이 저장되고 메모리가 해제된 뒤 종료된다
- 모든 PR을 4.x장 약속 기준으로 검토한다: 로직에 `printf` 없음, 담당자 표시, 테스트 통과, `NULL` 검사

#### 개발 외 업무
- GitHub 저장소 관리, 팀원 Collaborator 초대, PR 검토·`main` 병합, 버전 태그(v1.0, v2.0)
- 주간 회의 진행, 회의록, WBS 일정 관리
- 제안보고서·결과보고서 취합 (각자 맡은 모듈 설명을 받아서 정리), README 관리, 발표 PPT

---

### 6.2 장태현 (util)

**담당:** `util.c/h`, `test_util.c`
**브랜치:** `feature/util`

장태현은 **최종 발표**를 맡고 있어서 가장 작은 모듈 하나만 맡습니다. 입력 함수 몇 개로 이루어져 있고, 프로그램이 잘못된 입력에도 멈추지 않게 하는 중요한 역할입니다.

#### util.h

```c
#ifndef UTIL_H
#define UTIL_H
#include "common.h"

void clearInputBuffer(void);                              /* 입력 버퍼에 남은 줄 비우기 */
int  inputInt(const char *msg, int min, int max);         /* min~max 정수가 나올 때까지 반복 입력 */
int  inputDate(const char *msg, Date *d);                 /* YYYY-MM-DD 입력. 성공 1, "0" 입력(취소) 0 */
void inputWord(const char *msg, char *buf, int bufLen);   /* 공백 없는 단어 입력 (이름용) */
int  inputYesNo(const char *msg);                         /* y/Y면 1, n/N이면 0 */
void waitForKey(void);                                    /* 아무 키나 누를 때까지 대기 (_getch) */

#endif
```

구현 요점
- 입력은 `scanf`로 바로 받지 말고 **`fgets`로 한 줄을 받은 뒤 `sscanf`로 해석**합니다. 문자를 입력해도 무한 반복하지 않습니다.
- `inputDate`는 `sscanf(line, "%d-%d-%d", ...)`로 읽고, `isValidDate`(김기범)로 검사합니다. 틀리면 이유를 안내하고 다시 입력받습니다. 김기범의 PR이 올라오기 전에는 월 1~12, 일 1~31만 검사하도록 임시로 두세요.
- `inputWord`는 공백이 들어오면 거절하고, `bufLen - 1`바이트를 넘으면 잘라냅니다.

```c
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
```

#### 완료 기준
- `inputInt`에 글자, 빈 줄, 범위 밖 숫자를 넣어도 다시 입력받을 뿐 멈추거나 무한 반복하지 않는다
- `inputDate`에 `2026-02-30`, `2026-13-01`, `abc`를 넣으면 거절하고 다시 받는다. `0`을 넣으면 0을 반환한다
- `inputWord`에 공백이 든 입력은 거절하고, 너무 긴 입력은 `bufLen - 1`바이트로 자른다
- `inputYesNo`는 `y`, `Y`, `n`, `N`만 받는다

#### 혼자 테스트하기 (`--test util`)

사람이 직접 입력해 보는 테스트입니다. 잘못된 값도 일부러 넣어 보세요.

```c
/* test_util.c */
#include <stdio.h>
#include "test.h"
#include "util.h"

void testUtil(void)
{
    int n;
    Date d;
    char name[NAME_LEN];

    n = inputInt("1~10 사이 숫자 > ", 1, 10);
    printf("입력한 숫자: %d\n", n);

    inputWord("재료 이름 > ", name, sizeof(name));
    printf("입력한 이름: %s\n", name);

    if (inputDate("유통기한 (YYYY-MM-DD, 0=취소) > ", &d))
        printf("입력한 날짜: %04d-%02d-%02d\n", d.year, d.month, d.day);
    else
        printf("취소했습니다.\n");

    printf("y/n 결과: %d\n", inputYesNo("계속할까요? (y/n) > "));
}
```

#### 개발 외 업무: 최종 발표와 데모 시연
- 시연 순서 정리: 시작 알림 → 목록 → 등록 → 요리 추천 → 요리하기 → 폐기 통계
- 예상 질문과 답변 정리 (예: "왜 로직과 화면을 나눴나요?", "추천 점수는 어떻게 정했나요?")
- 리허설 1회 이상
- 산출물: 시연 시나리오, 예상 질문 정리

---

### 6.3 김기범 (expiry)

**담당:** `expiry.c/h`, `test_expiry.c`
**브랜치:** `feature/expiry`

#### expiry.h

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

#### 완료 기준
- `daysBetween({2026,11,9}, {2026,11,12}) == 3`, 월말·연말을 넘겨도 정확하다 (`{2026,2,28}` → `{2026,3,1}`은 1일, 윤년 `{2028,2,28}` → `{2028,3,1}`은 2일)
- `isValidDate({2026,2,29}) == 0`, `isValidDate({2028,2,29}) == 1`, 월 13이나 일 0은 0
- 정렬 후 `items[0]`이 가장 유통기한이 빠른 재료다
- 재료가 하나도 없어도 `countAlerts`와 `sortByExpiry`가 죽지 않는다

#### 혼자 테스트하기 (`--test expiry`)

```c
/* test_expiry.c */
#include <string.h>
#include "test.h"
#include "expiry.h"
#include "food.h"

void testExpiry(void)
{
    Date a = { 2026, 11, 9 }, b = { 2026, 11, 12 };
    Date leap = { 2028, 2, 29 }, noLeap = { 2026, 2, 29 };
    Fridge f;
    Food x = { "우유", { 2000, 1, 1 }, 1, FRIDGE };    /* 오래전 날짜 = 기한 지남 */
    Food y = { "두부", { 2099, 1, 1 }, 1, FRIDGE };
    char buf[32];
    int expired, soon;

    CHECK(daysBetween(a, b) == 3);
    CHECK(isValidDate(leap) == 1);
    CHECK(isValidDate(noLeap) == 0);
    CHECK(daysLeft(getToday()) == 0);

    formatDday(3, buf, sizeof(buf));   CHECK(strcmp(buf, "D-3") == 0);
    formatDday(0, buf, sizeof(buf));   CHECK(strcmp(buf, "D-DAY") == 0);
    formatDday(-2, buf, sizeof(buf));  CHECK(strcmp(buf, "D+2") == 0);

    initFridge(&f);
    addFood(&f, &y);
    addFood(&f, &x);                                   /* 일부러 순서를 거꾸로 넣는다 */
    sortByExpiry(&f);
    CHECK(strcmp(f.items[0].name, "우유") == 0);
    countAlerts(&f, &expired, &soon);
    CHECK(expired == 1 && soon == 0);
    freeFridge(&f);
}
```

`testExpiry`는 `addFood`를 쓰므로 박지연의 11/25 PR이 `main`에 들어온 뒤에 실행됩니다. 그 전에는 `f.items`에 직접 값을 넣는 임시 코드로 확인하세요.

#### 개발 외 업무
- 프로그램 흐름도 작성 (발표 PPT의 "3.3 프로그램 흐름도" 기준, 제안·결과보고서에 들어갈 그림)

---

### 6.4 최민솔 (recipe)

**담당:** `recipe.c/h`, `data/recipes.txt`, `test_recipe.c`
**브랜치:** `feature/recipe`

#### recipe.h

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

#### 완료 기준
- 재료가 충분한 레시피는 `canCook == 1`이고 점수가 가장 높다
- 재료가 절반 미만인 레시피는 추천에 나오지 않는다
- `cookRecipe`는 재료가 하나라도 부족하면 **아무것도 차감하지 않는다**
- 같은 재료가 유통기한이 다른 두 줄이면 **빠른 줄부터** 차감한다
- 레시피 20개 이상, 기본 레시피는 `isCustom == 0`

#### 혼자 테스트하기 (`--test recipe`)

```c
/* test_recipe.c */
#include "test.h"
#include "recipe.h"
#include "food.h"

void testRecipe(void)
{
    Fridge f;
    RecipeBook rb;
    Recommendation rec[10];
    Food egg   = { "계란", { 2099, 12, 31 }, 6, FRIDGE };
    Food onion = { "대파", { 2099, 12, 31 }, 1, FRIDGE };
    Recipe omelet = { "계란말이", { "계란", "대파" }, { 3, 1 }, 2, 0 };
    int n;

    initFridge(&f);
    initRecipeBook(&rb);
    addFood(&f, &egg);
    addFood(&f, &onion);
    CHECK(addRecipe(&rb, &omelet) == RC_OK);
    CHECK(addRecipe(&rb, &omelet) == RC_INVALID);          /* 이름 중복 */

    n = recommend(&f, &rb, rec, 10);
    CHECK(n == 1);
    CHECK(rec[0].canCook == 1);
    CHECK(rec[0].score == 40);                             /* 2개 × 10 + 20, 임박 없음 */

    CHECK(cookRecipe(&f, &omelet) == RC_OK);
    CHECK(f.count == 1 && f.items[0].quantity == 3);       /* 계란 6 → 3, 대파는 0이라 삭제 */
    CHECK(cookRecipe(&f, &omelet) == RC_NOT_ENOUGH);       /* 대파가 없다 */
    CHECK(f.items[0].quantity == 3);                       /* 실패했으니 차감되지 않음 */

    freeFridge(&f);
    freeRecipeBook(&rb);
}
```

#### 개발 외 업무
- 부팀장: 윤종하가 없을 때 PR 검토·병합 대행, 윤종하의 PR 검토
- 시연용 샘플 데이터(`data/fridge.txt`) 준비

---

### 6.5 박지연 (food, stats)

**담당:** `food.c/h`, `stats.c/h`, `test_food.c`, `test_stats.c`
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

날짜가 올바른지(윤년, 월별 일수)는 `food`가 아니라 **입력 단계(`inputDate`)에서** 검사합니다. 그래서 `food.c`는 `expiry.h`를 부르지 않습니다.

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

##### 혼자 테스트하기 (`--test food`)

```c
/* test_food.c */
#include "test.h"
#include "food.h"

void testFood(void)
{
    Fridge f;
    Food egg = { "계란", { 2026, 11, 12 }, 6, FRIDGE };
    int i;

    initFridge(&f);
    CHECK(addFood(&f, &egg) == RC_OK);
    CHECK(f.count == 1);

    CHECK(addFood(&f, &egg) == RC_OK);                 /* 같은 재료 → 수량 합치기 */
    CHECK(f.count == 1 && f.items[0].quantity == 12);

    CHECK(findFood(&f, "계란") == 0);
    CHECK(findFood(&f, "우유") == -1);

    CHECK(useFood(&f, 0, 100) == RC_NOT_ENOUGH);
    CHECK(removeFood(&f, 5) == RC_NOT_FOUND);
    CHECK(useFood(&f, 0, 12) == RC_OK && f.count == 0);   /* 0이 되면 자동 삭제 */

    for (i = 0; i < 20; i++) {                         /* 동적 배열 확장 확인 */
        Food x = { "재료", { 2026, 12, 1 }, 1, FRIDGE };
        x.expire.day = 1 + i;                          /* 날짜를 달리해 합쳐지지 않게 */
        CHECK(addFood(&f, &x) == RC_OK);
    }
    CHECK(f.count == 20);
    freeFridge(&f);
}
```

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

##### 혼자 테스트하기 (`--test stats`)

```c
/* test_stats.c */
#include <string.h>
#include "test.h"
#include "stats.h"

void testStats(void)
{
    WasteList w;
    WasteStats s;
    Date today = { 2026, 11, 9 };
    Food milk = { "우유",     { 2026, 11, 5 }, 1, FRIDGE };
    Food pork = { "돼지고기", { 2026, 11, 6 }, 2, FREEZER };

    initWasteList(&w);
    CHECK(addWaste(&w, &milk, today) == RC_OK);
    CHECK(addWaste(&w, &milk, today) == RC_OK);
    CHECK(addWaste(&w, &pork, today) == RC_OK);

    computeWasteStats(&w, today, &s);
    CHECK(s.totalRecords == 3);
    CHECK(s.totalQuantity == 4);
    CHECK(s.fridgeCount == 2 && s.freezerCount == 1);
    CHECK(strcmp(s.topName, "우유") == 0 && s.topCount == 2);
    CHECK(s.thisMonth == 3);

    freeWasteList(&w);
}
```

#### 개발 외 업무
- 테스트 시나리오 작성: 정상 입력, 잘못된 날짜(2월 30일), 빈 목록, 수량 0, 파일 없음, 이름에 공백, 한글 14자 초과 등
- 통합 테스트(12/07~) 실행, 발견한 버그를 **GitHub Issues**에 등록 (제목: `[버그] 어떤 화면 - 증상`, 내용: 재현 순서)
- 테스트 결과서 작성

---

### 6.6 GUI 도전 (v1.0 이후)

콘솔 버전이 완성되어 `v1.0` 태그가 붙은 뒤, 시간이 남으면 윤종하가 진행합니다. **로직 모듈은 고치지 않고** 그대로 호출합니다. 팀원이 맡을 일은 없으며, 화면 예시는 15.2장, 한글 변환은 13.3장입니다. **콘솔 버전은 항상 실행 가능한 상태**로 유지합니다.

---

## 7. 프로젝트 목표

### 필수 목표 (콘솔 버전)
- F1~F6 기능을 콘솔 프로그램으로 모두 완성
- 데이터 파일 저장·불러오기로 프로그램을 껐다 켜도 정보 유지
- 로직 모듈과 화면 모듈을 분리한 구조로 구현

### 도전 목표 (GUI 버전)
콘솔 버전이 완성된 뒤 단계별로 진행합니다. 로직 모듈은 **수정 없이 그대로** 재사용합니다.

| 단계 | 내용 | 난이도 |
|:---:|---|:---:|
| G1 | 프로그램 시작 시 유통기한 알림을 Windows 팝업 창(`MessageBoxW`)으로 표시 | 하 |
| G2 | 메인 창에 식재료 목록 표(ListView)와 기능 버튼 배치, 임박순 정렬 표시 | 중 |
| G3 | 식재료 등록·수정 입력 창, 삭제·수량 변경 버튼 | 중 |
| G4 | 요리 추천 창(추천 목록 + "요리하기" 버튼으로 자동 차감), 레시피 추가 창 | 상 |
| G5 | 폐기 통계 창, 임박 재료는 목록에서 빨간색·주황색으로 강조 | 상 |

> GUI는 G1부터 순서대로 진행하며, 일정이 부족하면 완성된 단계까지만 제출합니다. 콘솔 버전은 항상 실행 가능한 상태로 유지합니다.

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
| G1~G5 | GUI 버전 | 위 기능을 Windows 창 화면으로 제공 | `ui_gui` | 도전 |

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
- 콘솔: 테두리 박스 형태의 경고 화면 → 아무 키나 누르면 메인 메뉴로 이동
- GUI(G1): Windows 경고 팝업 창

**F5. 자동 차감**
- 추천 목록에서 요리를 선택하면 확인을 받은 뒤 레시피에 적힌 수량만큼 재료를 차감합니다.
- 수량이 0이 된 재료는 목록에서 자동 삭제됩니다.

**F6. 폐기 기록**
- 식재료를 삭제할 때 폐기로 기록할지 선택합니다.
- "기한 지난 재료 일괄 폐기" 기능으로 기한이 지난 재료를 한꺼번에 폐기 처리합니다.

---

## 9. 설계 원칙: 로직과 화면 분리

같은 기능을 콘솔과 GUI 두 화면에서 쓰기 위해, 프로그램을 **로직 계층**과 **화면 계층**으로 나눕니다.

```
┌──────────────────────┐    ┌──────────────────────┐
│  화면 계층 (콘솔)     │    │  화면 계층 (GUI)      │
│  main.c              │    │  main_gui.c          │
│  ui_console.c        │    │  ui_gui.c            │
│  printf / scanf_s    │    │  Win32 API           │
└──────────┬───────────┘    └──────────┬───────────┘
           │      같은 함수를 호출       │
           └──────────────┬─────────────┘
                          ▼
┌──────────────────────────────────────────────────┐
│  로직 계층 (공통)                                 │
│  food.c  expiry.c  recipe.c  stats.c  fileio.c   │
│  ※ printf / scanf / Win32 API 사용 금지           │
└──────────────────────────────────────────────────┘
```

### 규칙
1. **로직 모듈에서는 화면 입출력을 하지 않습니다.** 값은 매개변수로 받고, 결과는 반환값이나 포인터로 돌려줍니다.
2. 성공·실패는 `ResultCode`로 반환하고, 어떤 메시지를 보여 줄지는 화면 모듈이 결정합니다.
3. 화면 모듈은 로직 모듈의 함수만 호출하며, 구조체 내부를 직접 고치지 않습니다.

```c
/* 잘못된 예: 화면과 로직이 섞여 있어 GUI에서 재사용 불가 */
void addFood(Fridge *f) {
    printf("이름: ");
    scanf("%s", ...);
}

/* 좋은 예: 로직만 담당하므로 콘솔과 GUI 모두에서 그대로 호출 */
ResultCode addFood(Fridge *f, const Food *item);
```

---

## 10. 모듈 구조

하나의 솔루션 안에 **콘솔 프로젝트**와 **GUI 프로젝트** 두 개를 두고, 로직 모듈 소스 파일은 두 프로젝트가 함께 사용합니다.

```
fridge-manager/
├── README.md
├── .gitignore
├── FridgeManager.slnx         # 솔루션 파일 (이 파일을 열어서 작업)
├── FridgeConsole/
│   └── FridgeConsole.vcxproj # 콘솔 프로젝트 (필수)
├── FridgeGUI/
│   └── FridgeGUI.vcxproj     # GUI 프로젝트 (도전)
├── src/
│   ├── common.h              # 공통 구조체, 열거형, 상수, ResultCode
│   │
│   │   ── 로직 계층 (두 프로젝트 공통) ──
│   ├── food.h / food.c            # F1 등록, F5 수정·삭제·수량 변경·검색
│   ├── expiry.h / expiry.c        # F2 날짜 계산·D-day·정렬, F4 임박 개수 계산
│   ├── recipe.h / recipe.c        # F3 추천 점수 계산·레시피 추가, F5 자동 차감
│   ├── stats.h / stats.c          # F6 폐기 기록 추가·일괄 폐기·통계 계산
│   ├── fileio.h / fileio.c        # 모든 데이터 파일 저장·불러오기
│   │
│   │   ── 화면 계층 (콘솔 프로젝트, 필수) ──
│   ├── main.c                     # 콘솔 진입점: 데이터 불러오기 → 알림 → 메뉴 → 저장
│   ├── ui_console.h / ui_console.c  # 메뉴·목록·입력 화면 출력
│   ├── util.h / util.c            # 안전한 콘솔 입력, 입력 버퍼 정리
│   ├── test.h / test_*.c          # 모듈 단독 테스트 (--test 옵션, 6.0장)
│   │
│   │   ── 화면 계층 (GUI 프로젝트, 도전) ──
│   ├── main_gui.c                 # GUI 진입점 (wWinMain)
│   └── ui_gui.h / ui_gui.c        # 창, 버튼, ListView, 입력 대화상자
└── data/
    ├── fridge.txt            # 식재료 목록
    ├── recipes.txt           # 레시피 목록 (기본 + 사용자 추가)
    └── waste.txt             # 폐기 기록
```

| 프로젝트 | 포함하는 파일 |
|---|---|
| `FridgeConsole` | `main.c`, `ui_console.c`, `util.c`, `test_*.c` + 로직 모듈 5개 |
| `FridgeGUI` | `main_gui.c`, `ui_gui.c` + 로직 모듈 5개 |

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
| `ui_console` | `void showStartupAlert(const Fridge *f)` | 콘솔 경고 박스 출력 |
| | `void runMainMenu(AppData *app)` | 메인 메뉴 반복 |
| | `void screenAddFood / screenFoodList / screenEditFood ...` | 각 기능 화면 |
| `util` | `int inputInt(const char *msg, int min, int max)` | 범위 검사가 있는 정수 입력 |
| | `int inputDate(const char *msg, Date *d)` | `YYYY-MM-DD` 형식 검사 후 입력 |
| | `void clearInputBuffer(void)` | 입력 버퍼 비우기 |
| `ui_gui` | `void guiShowStartupAlert(const Fridge *f)` | G1: 경고 팝업 |
| | `LRESULT CALLBACK MainWndProc(...)` | G2: 메인 창 메시지 처리 |
| | `void guiRefreshList(HWND list, const Fridge *f)` | G2: ListView 갱신 |
| | `INT_PTR CALLBACK FoodDlgProc(...)` | G3: 등록·수정 대화상자 |
| | `void guiShowRecommend / guiShowStats ...` | G4·G5: 추천·통계 창 |

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

> **현재 상태:** `FridgeManager.slnx`, `FridgeConsole` 프로젝트, `src/`의 빈 파일 16개는 이미 올라와 있습니다. 팀장에게 남은 일은 아래 "두 프로젝트 공통 설정"(`_CRT_SECURE_NO_WARNINGS`, 작업 디렉터리)과 6.1장의 `test_*.c` 추가, `data/` 폴더 생성입니다. GUI 프로젝트는 v1.0 이후에 추가합니다. 팀원은 이 절을 따라 할 필요가 없습니다.

**콘솔 프로젝트 (FridgeConsole)**
1. 새 프로젝트 → **빈 프로젝트** → 이름 `FridgeConsole`, 솔루션 이름 `FridgeManager`
2. 솔루션 탐색기 → "소스 파일" 우클릭 → 추가 → 기존 항목 → `src/`의 콘솔용 파일과 로직 모듈 `.c` 파일 추가
3. "헤더 파일"에 `src/`의 `.h` 파일 추가
4. 파일 확장자는 반드시 **`.c`**로 합니다. (`.cpp`이면 C++로 컴파일됩니다.)

**GUI 프로젝트 (FridgeGUI, 도전 단계에서 추가)**
1. 솔루션 우클릭 → 추가 → 새 프로젝트 → **빈 프로젝트** → 이름 `FridgeGUI`
2. `main_gui.c`, `ui_gui.c`와 로직 모듈 `.c` 파일을 기존 항목으로 추가
   (로직 파일은 복사하지 않고 `src/`의 같은 파일을 두 프로젝트가 함께 참조합니다.)
3. 프로젝트 속성 → 링커 → 시스템 → **하위 시스템: Windows (/SUBSYSTEM:WINDOWS)**
4. 프로젝트 속성 → 고급 → **문자 집합: 유니코드 문자 집합 사용**
5. 공용 컨트롤(ListView) 라이브러리는 `ui_gui.c` 상단에서 연결합니다.
   ```c
   #include <windows.h>
   #include <commctrl.h>
   #pragma comment(lib, "comctl32.lib")
   ```

**두 프로젝트 공통 설정** (구성: **모든 구성**, 플랫폼: **모든 플랫폼**으로 선택 후 설정)
- C/C++ → 전처리기 → 전처리기 정의에 `_CRT_SECURE_NO_WARNINGS` 추가
  (`scanf`, `fopen` 사용 시 나오는 보안 경고 C4996 해제)
- 디버깅 → 작업 디렉터리를 **`$(SolutionDir)`**로 변경
  (실행할 때 솔루션 폴더의 `data/`를 찾을 수 있도록 함)

### 13.3 한글 처리

한국어 Windows의 콘솔은 기본적으로 **코드 페이지 949(EUC-KR)**를 사용합니다. 아래처럼 맞추면 별도 설정 없이 콘솔에서 한글 입력과 출력이 정상 동작합니다.

| 파일 종류 | 저장 인코딩 | 이유 |
|---|---|---|
| 소스 파일 (`.c`, `.h`) | **유니코드(서명 있는 UTF-8)** | GitHub에서도 한글 주석이 깨지지 않음. Visual Studio가 문자열을 실행 시 949로 자동 변환 |
| 데이터 파일 (`.txt`) | **ANSI (EUC-KR)** | 콘솔 입력과 같은 인코딩이라 `strcmp` 비교가 정확함 |
| `README.md` | UTF-8 | GitHub 표시용 |

- 소스 파일 인코딩 변경: 파일 → 다른 이름으로 저장 → 저장 버튼 옆 화살표 → **인코딩하여 저장** → "유니코드(서명 있음)(UTF-8) - 코드 페이지 65001"
- 데이터 파일 인코딩 변경: 메모장에서 열기 → 다른 이름으로 저장 → 인코딩 **ANSI**
- 프로젝트 속성에 `/utf-8` 옵션은 **추가하지 않습니다.** (추가하면 콘솔 한글 출력이 깨집니다.)

**GUI에서의 한글**: Win32 API는 유니코드(`wchar_t`) 함수(`...W`)를 사용합니다. 데이터 파일에서 읽은 문자열은 화면에 표시하기 전에 변환합니다.
```c
/* ui_gui.c: ANSI(949) → 유니코드 변환 (로직 데이터를 화면에 표시할 때) */
void ansiToWide(const char *src, wchar_t *dst, int dstLen) {
    MultiByteToWideChar(CP_ACP, 0, src, -1, dst, dstLen);
}

/* ui_gui.c: 유니코드 → ANSI(949) 변환 (입력창 값을 로직에 넘길 때) */
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
2. 솔루션 탐색기에서 실행할 프로젝트를 우클릭 → **시작 프로젝트로 설정**
   - 콘솔 버전: `FridgeConsole`
   - GUI 버전: `FridgeGUI`
3. `Ctrl + F5` (디버깅하지 않고 시작) 또는 `F5` (디버깅 시작)

> 실행 시 솔루션 폴더 안에 `data/` 폴더가 있어야 합니다. 작업 디렉터리 설정(13.2)을 확인하세요.

---

## 15. 실행 화면 예시

### 15.1 콘솔 버전 (필수)

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

### 15.2 GUI 버전 (도전)

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
| `v145 플랫폼 도구 집합을 찾을 수 없습니다` | 팀장의 Visual Studio가 더 최신 버전 | 팀장에게 알리기. 급하면 프로젝트 속성 → 일반 → 플랫폼 도구 집합을 설치된 것(`v143` 등)으로 바꿔 실행하되, **커밋 전에 `git restore FridgeConsole/FridgeConsole.vcxproj`로 되돌리기** |
| 한글이 `?`나 깨진 글자로 나옴 | 소스 인코딩 문제 | 소스는 UTF-8(서명 있음), 데이터는 ANSI. `/utf-8` 옵션은 추가하지 않기 (4.6, 13.3) |
| `error C4996: 'scanf': This function may be unsafe` | 보안 경고가 오류로 처리됨 | 프로젝트 속성 → C/C++ → 전처리기에 `_CRT_SECURE_NO_WARNINGS` (팀장이 설정. 안 되어 있으면 알리기) |
| 프로그램이 `data/fridge.txt`를 못 찾음 | 작업 디렉터리가 다름 | 속성 → 디버깅 → 작업 디렉터리 `$(SolutionDir)`. `.exe`를 직접 더블클릭하면 안 됨 |
| `LNK2019: 확인할 수 없는 외부 기호` | 함수 선언만 있고 정의가 없음, 또는 `.c`가 프로젝트에 없음 | 함수 이름·인자가 `.h`와 `.c`에서 똑같은지 확인. 파일 추가는 팀장에게 요청 |
| `LNK2005: 이미 정의되었습니다` | 헤더에 함수 본문을 넣음 | 본문은 `.c`에, `.h`에는 선언만 |
| 새 파일이 C++로 컴파일됨 (`error C2440` 등) | 확장자가 `.cpp` | `.c`로 만들기 |
| 실행하자마자 창이 닫힘 | `F5`로 실행 | `Ctrl + F5` 사용 |
| 입력 후 다음 입력이 건너뛰어짐 / 무한 반복 | `scanf` 뒤 입력 버퍼에 개행이 남음 | `fgets` + `sscanf` 사용 (`util.c`의 입력 함수만 쓰기) |
| 실행 중 갑자기 종료 (`Access violation`) | `NULL` 포인터, 배열 범위 밖 접근 | 인자 `NULL` 검사, `idx < count` 검사. 디버거(`F5`)로 멈춘 줄 확인 |
| 재료를 많이 넣으면 죽음 | `realloc` 결과를 잘못 처리 | 임시 변수에 받고 `NULL` 검사 후 대입 (4.5) |
| 이름 검색이 안 됨 | 이름 끝에 개행(`\n`)이 붙어 있음 | `fgets`로 받은 문자열의 `\n` 제거 |
| `git push`가 거절됨 | 내 브랜치가 오래됨 | `git pull origin 내브랜치` 후 다시 push |
| `.vcxproj`에서 충돌이 남 | 누군가 프로젝트 파일을 수정함 | 직접 해결하지 말고 팀장에게 알리기 |
| `x64/`, `.vs/` 같은 폴더가 `git status`에 나옴 | `.gitignore`에 없는 빌드 결과물 | 커밋하지 말고 팀장에게 알리기. `git add 파일이름`으로 내가 고친 파일만 추가 |

---

## 17. 완료 체크리스트

### 기능 (콘솔 v1.0)
- [ ] F1 식재료 등록, 같은 재료 수량 합치기 (박지연)
- [ ] F2 D-day 계산과 임박순 정렬 (김기범)
- [ ] F3 요리 추천과 레시피 추가, 기본 레시피 20개 이상 (최민솔)
- [ ] F4 시작 시 임박 알림 (김기범 함수, 윤종하 화면)
- [ ] F5 수정·삭제·수량 변경, 요리 시 자동 차감 (박지연, 최민솔)
- [ ] F6 폐기 기록, 일괄 폐기, 통계 (박지연)
- [ ] 파일 저장·불러오기, 껐다 켜도 데이터 유지 (윤종하)
- [ ] 안전한 입력 함수 (장태현)
- [ ] 메인 메뉴와 전체 연결 (윤종하)

### 코드 규칙
- [ ] 로직 모듈 5개에 `printf`, `scanf`, Win32 API가 없다
- [ ] `malloc`/`realloc` 뒤 `NULL` 검사를 했고 종료 전 `free` 한다
- [ ] 모든 함수에 한글 설명 주석, 파일 맨 위에 담당자 표시가 있다
- [ ] `--test` 테스트가 모두 `[PASS]`다
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
- [ ] (도전) `v2.0` 태그: GUI

---

## 18. 참고 자료
- 수업 교재: `교재명 / 저자 / 출판사`
- C 표준 라이브러리 `time.h`: https://en.cppreference.com/w/c/chrono
- `qsort`: https://en.cppreference.com/w/c/algorithm/qsort
- Visual Studio에서 C 프로그램 만들기 (Microsoft Learn): https://learn.microsoft.com/ko-kr/cpp/build/walkthrough-compile-a-c-program-on-the-command-line
- Win32 API 시작하기 (Microsoft Learn): https://learn.microsoft.com/ko-kr/windows/win32/learnwin32/learn-to-program-for-windows
- ListView 컨트롤 (Microsoft Learn): https://learn.microsoft.com/ko-kr/windows/win32/controls/list-view-controls-overview
