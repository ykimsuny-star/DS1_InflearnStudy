# 공부노트 02 — BindAction 한 줄 뜯어보기

> 대상 코드: `Source/DS1/Characters/DS1Character.cpp:109`
```cpp
EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ThisClass::AutoToggleCombat);
```
> 한 문장 요약: "좌클릭(AttackAction)이 눌리는 순간(Started)되면, 나(this)의 검 자동 꺼내기(AutoToggleCombat)를 실행해줘"

---

## 1. 전체 구조: 누가, 무엇을, 언제, 어떻게

```
[우체국] -> [선 연결] ( [정의서], [타이밍], [누가], [무슨 일] );
EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ThisClass::AutoToggleCombat);
```

| 순서 | 코드 | 영어 단어 풀이 | 예시로 이해하기 |
|---|---|---|---|
| 1 | `EnhancedInputComponent` | Enhanced(향상된, 업그레이드된) + Input(입력) + Component(부품) | 옛날 우체통 → 신형 택배 허브. 키보드/마우스 입력을 받아서 어디로 보낼지 정하는 곳. `SetupPlayerInputComponent`에서 `Cast`로 가져온 그거 |
| 2 | `->` | pointer(가리키는 것)를 통해 접근 | `EnhancedInputComponent`가 포인터라서 `.`이 아니라 `->` 사용. 예: `리모컨->전원버튼` 같은 느낌 |
| 3 | `BindAction` | Bind(묶다, 연결하다) + Action(행동, 동작 정의) | 전화선 연결. "이 키 들어오면 이 함수로 연결해"라는 예약. 예: 초인종 버튼(Bind) 누르면 우리집 벨이 울리게 선 연결 |
| 4 | `AttackAction` | Attack(공격) + Action(행동 정의서) | 에디터에서 만든 정의서 에셋(`UInputAction`). "좌클릭 = 공격"이라는 종이일 뿐, 실제 동작은 없음. 예: 메뉴판에 "A세트"라고 적혀있는 것 |
| 5 | `ETriggerEvent::Started` | Trigger(방아쇠를 당기다, 작동시키다) + Event(사건) + Started(시작된) | 방아쇠가 당겨지기 시작한 순간. 아래 타이밍표 참고 |
| 6 | `this` | this(이것, 나 자신) | 지금 이 캐릭터 객체 자신. "누가 실행하냐면 바로 내가"라는 뜻. 예: 배달 주소가 "본인" |
| 7 | `&ThisClass::AutoToggleCombat` | This(이번) + Class(반, 종류) / Auto(자동) + Toggle(끄고 켜기 스위치) + Combat(전투) | `ThisClass` = `ADS1Character`의 별명. `&` = 주소(명함). `()`가 없으니 지금 실행이 아니라 "나중에 불러줘"라며 명함만 넘김. 예: `()` 있으면 "지금 전화 걸기", `&`만 있으면 "전화번호 저장" |

---

## 2. ETriggerEvent 타이밍 4종 (제일 중요)

키 하나를 눌렀다 뗐다고 생각해보자.

| 이벤트 | 언제 불리나? | 우리 프로젝트 예시 | 예시로 이해하기 |
|---|---|---|---|
| `Started` | 누르는 순간 1번 | `InteractAction + Started -> Interact` (E키 누르자마자 줍기 판정)<br>`AttackAction + Started -> AutoToggleCombat` (좌클릭 누르자마자 검 꺼내기) | 초인종을 누른 손가락이 닿은 순간 "딩동" 1번 |
| `Triggered` | 누르고 있는 동안 계속 | `MoveAction + Triggered -> Move` (W 누르고 있는 동안 계속 이동)<br>`SprintRollingAction + Triggered -> Sprinting` (Shift 누르고 있는 동안 계속 질주)<br>`AttackAction + Triggered -> SpecialAttack` (좌클 길게 누르면 특수공격) | 초인종을 계속 누르고 있으면 "딩동딩동딩동" 계속 |
| `Completed` | 뗀 순간 | `SprintRollingAction + Completed -> StopSprint` (Shift 떼면 질주 중단) | 손가락 뗀 순간 "이제 그만" |
| `Canceled` | 중간에 취소/빼앗긴 순간 | `SprintRollingAction + Canceled -> Rolling` (짧게 끊으면 구르기)<br>`AttackAction + Canceled -> Attack` (짧게 좌클릭하면 일반공격) | 누가 옆에서 손을 쳐서 초인종에서 손이 떨어진 경우 |

같은 키로 여러 개 거는 게 포인트:
```cpp
// 같은 SprintRollingAction인데 타이밍별로 다르게 동작
BindAction(SprintRollingAction, Triggered, ... Sprinting);  // 길게 누름 = 질주
BindAction(SprintRollingAction, Completed, ... StopSprint); // 뗌 = 질주 끝
BindAction(SprintRollingAction, Canceled,  ... Rolling);    // 짧게 끊음 = 구르기

// 같은 AttackAction인데 누르는 길이에 따라 다르게 동작
BindAction(AttackAction, Started,   ... AutoToggleCombat); // 누르자마자 검 꺼내기
BindAction(AttackAction, Canceled,  ... Attack);           // 짧게 좌클 = 일반공격
BindAction(AttackAction, Triggered, ... SpecialAttack);    // 길게 좌클 = 특수공격
```

---

## 3. AutoToggleCombat이 하는 일

`Source/DS1/Characters/DS1Character.cpp:309-318`
```cpp
void ADS1Character::AutoToggleCombat()
{
    if (CombatComponent)
    {
        if (!CombatComponent->IsCombatEnabled()) // Combat(전투상태)이 Enabled(켜짐)가 아니면
        {
            ToggleCombat(); // Toggle(스위치 뒤집기) = 검 꺼내기
        }
    }
}
```

* 평소엔 검 집어넣고 다니다가, 공격키 누르는 순간 자동으로 검 꺼내는 편의 기능.
* 이미 전투 중이면 아무것도 안 함 (중복 재생 방지).
* `HeavyAttack`(`340줄`)에서도 맨 먼저 `AutoToggleCombat()`을 불러서 Shift+좌클 때도 검부터 꺼냄.

---

## 4. 헷갈리기 쉬운 포인트 정리

1. `()`가 없다 = 지금 실행 아님, 예약임.
   ```cpp
   &ThisClass::Move  // 전화번호 저장 (나중에 걸겠음)
   Move()            // 지금 전화 걸기
   ```
2. `AttackAction` 자체에는 로직이 없다. 그냥 에디터 정의서.
3. 진짜 동작은 뒤에 붙은 함수(`Move`, `Attack`, `Rolling`...)가 한다.
4. `ThisClass`는 언리얼이 자동 생성한 별명 = `ADS1Character`라고 읽으면 됨.

## 5. 다음에 볼 때 체크용 한 줄 해석 연습

```cpp
EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
// "WASD(MoveAction)가 눌려있는 동안(Triggered), 나(this)의 이동(Move)을 실행해줘"
```
