# Rider Git 커밋 & 푸시 튜토리얼 (Fork 없이 학원↔집 작업하기)

> 대상: Rider 처음 쓰는 UE 개발자 / 학원↔집 반복 패턴
> 프로젝트: DS1_InflearnStudy (`main` 브랜치)

## 1. 결론

평소는 **Rider에서만 `Commit + Push`**, 집/학원 도착하면 **`Update(Ctrl+T)`로 Pull** 하면 된다.
Fork는 히스토리 정리(rebase, squash), 꼬인 히스토리 복구할 때만 켜면 된다.

| 구분 | Fork | Rider 내장 Git |
|------|------|----------------|
| 장점 | 히스토리 그래프, interactive rebase, stash, reflog, amend, cherry-pick 등 고급 기능이 편함. 충돌 해결 UI 강력 | 코드 편집→커밋→푸시까지 한 화면. `Ctrl+K`로 끝. 컴파일 에러 파일이 섞이는 걸 바로 확인 가능 |
| 단점 | 창 전환 필요. UE 작업 흐름과 분리됨 | 복잡한 rebase, 히스토리 정리, force-push 관리 등은 불편 |
| 추천 용도 | 푸시 전 커밋 합치기/메시지 정리, 꼬인 히스토리 풀기, 브랜치 정리 | 일상 커밋/풀/푸시 전부 |

## 2. Commit 창 각 부분 설명

Rider 왼쪽 세로 툴바 `Commit` 탭 기준.

### 2.1 Changes N files
- 이미 Git이 아는 파일 중 수정된 것.
- 위 체크박스 = 전체 선택/해제. 체크된 것만 커밋됨.

파일 앞 아이콘:
- `C++` = .cpp, `.H` = .h, `uasset 아이콘` = 언리얼 에셋
- 파란 글씨 = 수정, 주황 글씨 = 신규(Unversioned)

### 2.2 Unversioned Files
- 새로 만든 파일. Git이 아직 모름.
- 체크하면 `git add` + 커밋이 한 번에 됨. 따로 add 불필요.
- 예: `IA_Attack / IA_HeavyAttack / IA_Shift.uasset`

### 2.3 Amend last commit
- 처음엔 건드리지 말 것.
- 마지막 커밋에 끼워넣기. 이미 푸시한 커밋에 쓰면 학원/집 꼬임.

### 2.4 Commit Message 박스
- 한글로 적어도 됨. 예:
```
콤보 윈도우 NotifyState, 공격 종료 Notify 추가, 공격 IA 3종 추가
```

### 2.5 하단 버튼
- `Commit` = 내 PC에만 저장. GitHub에 안 올라감. 다른 PC에서 못 받음.
- `Commit and Push...` = 커밋 + GitHub 푸시까지. **학원↔집 이동할 땐 무조건 이거.**
- 우하단 톱니 = Commit Checks. 기본값 유지.

단축키:
- `Ctrl+K` = Commit 창 열기
- `Ctrl+T` = Update (Pull)
- `Ctrl+Shift+K` = Push

## 3. 정상 커밋 순서 (5단계)

1. **Diff 확인**
   - 파일 더블클릭 → 오른쪽에 변경 내용 표시.
   - `.cpp/.h`는 코드 diff가 보임. `.uasset`은 바이너리라 내용 안 보임. 정상.
2. **체크박스 체크**
   - `Changes`, `Unversioned Files` 왼쪽 네모 클릭해 전체 체크.
3. **메시지 쓰기**
4. **`Commit and Push...` 클릭**
   - 처음 한 번은 GitHub 로그인 창 → 브라우저 허용.
   - Push 창 뜨면 `Push` 다시 클릭.
5. **푸시 성공 확인**
   - 하단 Git 로그에 `Pushed main` 뜨면 끝.

## 4. 학원↔집 루틴

**떠날 때:** `Ctrl+K` → 파일 체크 → `Commit and Push`
**도착해서:** Rider 열고 `Ctrl+T` → `OK` → 최신 받기 → 작업 시작

푸시가 `rejected` 뜨면? 반대쪽 PC에서 먼저 푸시한 게 있다는 뜻.
`Update(Ctrl+T)` 먼저 하고 다시 `Push`.

## 5. 실수 복구: Commit and Push 눌렀는데 취소했더니 Commit 창이 비었을 때

### 증상
- 두 번째 사진처럼 `Changes`가 비어 있고, 하단에 `Select files to commit and specify commit message` 표시.

### 원인
- 이것은 에러가 아님. **Commit은 성공했고, Push만 취소된 상태.**
- 증거: `git status -sb` → `## main...origin/main [ahead 1]`
- 즉, 로컬 `main`이 `origin/main`보다 1개 커밋 앞서 있음. 왼쪽에 보일 변경분이 없는 게 정상.

### 해결 (Push만 다시 하면 됨)
1. `Ctrl+Shift+K` (또는 상단 `Git > Push...`)
2. Push 창에 커밋 `31f8110 [이전 작업:학원]...` 1개 보이는지 확인
3. `Push` 클릭
4. 하단에 `Pushed` 확인 → Fork/깃허브 웹에서 같은 커밋 보이면 완료

> `Commit` 버튼이 비활성화돼 있는 것도 정상. 커밋할 게 없기 때문.
> 파일을 날린 게 아니니 `Revert`나 `Amend` 누르지 말 것.

## 6. UE 프로젝트 주의사항

1. 푸시 전 `.gitignore` 확인: `Intermediate/`, `Saved/`, `DerivedDataCache/`, `Binaries/`, `*.sln`, `.idea/`는 커밋 금지. 용량 폭발 + 충돌 원인.
2. `.uasset`은 바이너리라 머지 불가. 학원/집 양쪽에서 같은 블루프린트 동시에 고치지 말 것. 푸시 전엔 꼭 먼저 Pull.
3. `Git > Local Changes`에서 리뷰 후 `Ctrl+K`로 커밋하는 습관 유지.
