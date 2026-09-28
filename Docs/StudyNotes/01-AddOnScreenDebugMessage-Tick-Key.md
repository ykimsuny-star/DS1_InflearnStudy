# Unreal 공부노트 01 — Tick + AddOnScreenDebugMessage + Key

> 발단: `DS1PickupItem::Interact`의 `Hello!`가 호출은 되는데 화면에 안 찍힘
> 원인: `DS1Character::Tick`이 매 프레임 `Key 0`을 `Stamina`로 덮어씀
> 위치: `Source/DS1/Characters/DS1Character.cpp:66-73`

```cpp
void ADS1Character::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    /** 스테미나 디버깅용 텍스트 출력
    GEngine->AddOnScreenDebugMessage(0, 1.5f, FColor::Cyan, FString::Printf(TEXT("Stamina : %f"), AttributeComponent->GetBaseStamina()));
    GEngine->AddOnScreenDebugMessage(2, 1.5f, FColor::Cyan, FString::Printf(TEXT("MaxWalkSpeed : %f"), GetCharacterMovement()->MaxWalkSpeed));
    */
}
```

---

## 1. Tick 기초

* `Tick(float DeltaTime)`: 매 프레임 자동 호출. 60fps면 1초에 60번.
* `DeltaTime`: 지난 프레임과의 시간차. 60fps면 약 `0.016초`.
* `Super::Tick(DeltaTime)`: 부모(`ACharacter`) 일 먼저 처리. 빼먹으면 이동/애님 꼬임.
* `PrimaryActorTick.bCanEverTick = true`가 생성자에 있어야 Tick이 돈다.

## 2. AddOnScreenDebugMessage 원형

```cpp
AddOnScreenDebugMessage(int32 Key, float TimeToDisplay, FColor Color, FString Message)
```

| 인자 | 의미 | 예시 |
|---|---|---|
| `Key` | 사물함 번호. 같은 번호는 덮어씀. `-1`은 새로 추가 | `0`, `2`, `-1` |
| `TimeToDisplay` | 몇 초 보여줄지 | `1.5f` = 1.5초 |
| `Color` | 글자 색 | `FColor::Cyan` |
| `Message` | 출력 문자열 | `TEXT("Hello!")` |

문자열 조립 패턴:
```cpp
FString::Printf(TEXT("Stamina : %f"), AttributeComponent->GetBaseStamina())
// %f 자리에 float 꽂아줌. TEXT()는 언리얼 문자열 매크로라 무조건 감싸기.
```

Tick에서 매 프레임 다시 찍으면 `1.5초`짜리가 계속 연장돼서 **상시 표시처럼** 보인다.

## 3. int32 오해 정리

* `int32`의 `32`는 개수가 아니라 **비트 수**.
* 범위: `-2,147,483,648 ~ +2,147,483,647` (약 -21억 ~ +21억)
* `-32~32` 64개 제한 아님.

자주 보는 별명:
* `int32` — 기본 정수
* `int64` — 더 큰 정수
* `uint8` — 0~255, 플래그용

## 4. Key 원리 — 왜 키를 올리면 내려가는가?

엔진은 내부에 `TMap<int32, 메시지>` 같은 목록을 들고 있고,
뷰포트는 **Key 오름차순으로 정렬해서 위에서부터 한 줄씩** 그린다.

```
화면 좌상단
0번 줄 <- Key 0 (Stamina)
1번 줄 (빔, Key 1 없음)
2번 줄 <- Key 2 (MaxWalkSpeed)
```

* `Key`가 클수록 밑에 그려짐.
* 중간 번호를 비우면 빈 줄이 생김.
* `-1` (`INDEX_NONE`)은 특별 약속: “덮지 말고 맨 밑에 새로 추가”.

실무 패턴:
```cpp
// Tick처럼 계속 갱신: 고정 키
GEngine->AddOnScreenDebugMessage(0, 5.f, FColor::Cyan, TEXT("고정 출력용"));
// 이벤트처럼 한 번 보고 버릴 로그: -1
GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("한번용"));
// 안 쓰는 큰 번호로 충돌 회피도 가능
GEngine->AddOnScreenDebugMessage(100, 5.f, FColor::Cyan, TEXT("Hello!"));
```

> 픽셀 단위 위치 지정 아님. 진짜 UI는 `UMG/Widget`으로.

## 5. 트러블슈팅 — Hello!가 안 보였던 이유

`DS1PickupItem::Interact`:
```cpp
GEngine->AddOnScreenDebugMessage(0, 1.5f, FColor::Cyan, TEXT("Hello!"));
```

1. `Interact`가 `0번 사물함`에 `Hello!`를 붙임.
2. 바로 다음 프레임에 캐릭터 `Tick`이 `0번 사물함`에 `Stamina : ...`를 덮어씀.
3. `Hello!`는 1프레임(약 0.016초)만 살아서 눈에 안 보임.

해결: `Interact` 쪽 키를 `-1`이나 `10`, `100` 같은 안 쓰는 번호로 변경.

체크리스트 (다음에 안 보이면):
1. `if (GEngine)` 널 체크 했나?
2. 키가 Tick 메시지와 겹치지 않나? (`-1`로 테스트)
3. `TimeToDisplay`가 너무 짧지 않나? (`5.f`로 늘리기)
4. 뷰포트가 아니라 Output Log만 보고 있지 않나? (화면 메시지는 로그에 안 찍힘, `UE_LOG` 병행)
5. `EnableOnScreenDebugMessages 1` 켜져 있나? / Dedicated Server로 실행 중이 아닌가?

```cpp
if (GEngine)
{
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("Hello!"));
}
UE_LOG(LogTemp, Warning, TEXT("Hello! Interactor: %s"), *GetNameSafe(Interactor));
```
