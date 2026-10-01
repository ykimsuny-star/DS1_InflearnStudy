# 공부노트 04 — CollisionTrace + SphereTraceMultiForObjects

> 대상: `Source/DS1/Components/DS1WeaponCollisionComponent.cpp:67-111`
> 한 문장 요약: "칼자루~칼끝까지 비눗방울을 굴려서 닿은 사람 전부를 찾아내고, 한 번 휘두를 때 중복으로 때리지 않게 처리"

```cpp
void UDS1WeaponCollisionComponent::CollisionTrace()
{
	TArray<FHitResult> OutHits;

	// 무기 매쉬의 Start, End 소켓 위치를 가져와 저장;
	const FVector Start = WeaponMesh->GetSocketLocation(TraceStartSocketName);
	const FVector End = WeaponMesh->GetSocketLocation(TraceEndSocketName);

	//타원형태로 충돌을 감지하는 트레이스 함수로 충돌을 감지
	bool const bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
		GetOwner(),
		Start, //시작 위치 ~
		End, //~끝나는 위치
		TraceRadius, //타원형의 반지름 크기Complex
		TraceObjectTypes, //감지대상의 타입 - 오브젝트타입 감지
		false,
		IgnoredActors, //감지 제외 대상
		DrawDebugType, //디버그 확인용 collision 표시
		OutHits, //충돌한 대상의 정보
		true); //자신을 감지 하지 않도록

	if (bHit)
	{
		for (const FHitResult& Hit : OutHits)
		{
			AActor* HitActor = Hit.GetActor();

			if (HitActor == nullptr)
			{
				continue;
			}

			if (CanHitActor(HitActor))
			{
				AlreadyHitActors.Add(HitActor);

				// Call OnHitActor BroadCast
				if (OnHitActor.IsBound())
				{
					OnHitActor.Broadcast(Hit);
				}
			}
		}
	}
}
```

---

## 1. 함수 이름부터 뜯기

| 코드 | 단어 풀이 | 예시로 이해하기 |
|---|---|---|
| `Collision` | Collision(충돌, 부딪힘) | 차끼리 "쿵" 부딪히는 것 |
| `Trace` | Trace(흔적을 따라가다) | 눈 오는 날 발자국 따라가기 |
| `CollisionTrace` | 부딪힌 흔적을 따라가서 확인하다 | 칼 휘두른 자리에 "누가 맞았나?" 확인하는 함수 |
| `SphereTraceMultiForObjects` | Sphere(공, 구슬) + Trace + Multi(여러 개) + For(~대상으로) + Objects(물체 종류들) | 비눗방울을 굴리면서 닿은 사람 **전부** 찾기. 쌍둥이 비교: `Single(처음 1명만, 저격총)`, `Multi(전부, 샷건/칼)` |

---

## 2. Start / End 구하기

```cpp
TArray<FHitResult> OutHits;
const FVector Start = WeaponMesh->GetSocketLocation(TraceStartSocketName);
const FVector End = WeaponMesh->GetSocketLocation(TraceEndSocketName);
```

| 코드 | 단어 풀이 | 예시로 이해하기 |
|---|---|---|
| `TArray` | T(아무거나 담는) + Array(배열, 줄줄이) | 출석부. 몇 명이 맞을지 모르니 빈 종이 먼저 준비 |
| `FHitResult` | Hit(맞음) + Result(결과) | "누가, 어디에, 얼마나 맞았나" 적힌 진단서 1장 |
| `OutHits` | Out(밖으로) + Hits(맞은 것들) | 함수가 채워줄 빈 출석부. `const`가 아닌 이유 = 함수가 안에 글씨를 써야 해서 |
| `FVector` | Vector(방향+크기, 위치) | 지도 위 좌표 핀 1개 (X, Y, Z) |
| `Start / End` | Start(시작) / End(끝) | 칼자루 소켓 ~ 칼끝 소켓. 칼이 움직이면 두 점도 같이 움직임 |
| `GetSocketLocation` | Get + Socket(소켓, 플러그 꽂는 구멍) + Location(위치) | 건담 손에 빔사벨 꽂는 구멍 위치 물어보기. 무기 메시에 미리 만들어둔 2개 구멍 좌표를 가져옴 |
| `const` | const(고정된, 변하지 않는) | "이번 프레임 검사하는 동안 시작/끝 바꾸지 마" 자물쇠 |
| `WeaponMesh` | Weapon(무기) + Mesh(그물망, 3D 모양) | 진짜 칼 3D 모델. `SetWeaponMesh`로 미리 받아둠 |

> 그림으로 보면: `Start●━━━━ 소시지 ━━━━●End` — 두 점을 이은 소시지 모양이 검사 범위.

---

## 3. SphereTraceMultiForObjects 인자 10개

```
GetOwner(),       // 1. 누구 기준으로?
Start,            // 2. 어디서부터?
End,              // 3. 어디까지?
TraceRadius,      // 4. 얼마나 굵게?
TraceObjectTypes, // 5. 누구를 찾아?
false,            // 6. 정밀하게 볼까?
IgnoredActors,    // 7. 누구를 빼?
DrawDebugType,    // 8. 눈에 보여줘?
OutHits,          // 9. 결과는 어디에?
true              // 10. 나 자신은 빼?
```

| 순서 | 코드 | 단어 풀이 | 예시로 이해하기 |
|---|---|---|---|
| 1 | `GetOwner()` | Get + Owner(주인) | 신분증 내밀기. "이 부품 주인은 칼 든 캐릭터야"라고 월드에 알려줌 |
| 2/3 | `Start, End` | 위 참고 | 칼자루 ~ 칼끝. 휘두르면 매 프레임 위치가 바뀜 |
| 4 | `TraceRadius` | Trace + Radius(반지름) | 소시지 굵기. 기본 `20.f`. 칼날이 얇아 스치면 판정 안 나니까 20cm 비눗방울로 부풀려서 "스치기만 해도 맞음"으로 만듦 |
| 5 | `TraceObjectTypes` | Object(물체) + Types(종류들) | 생성자에서 `ECC_Pawn` 등록 = "사람 종류만 찾아줘, 벽/바닥 신경 꺼". `Interact`는 `WorldStatic(바닥 물건)`을 찾았고, 여긴 사람 때리려고 `Pawn`만 찾음 |
| 6 | `false` | bTraceComplex (Trace + Complex, 복잡한/정밀한) | `false` = 몸통 박스로 대충 판정 (싸고 빠름, 칼질 정석). `true` = 손가락까지 정밀 판정 (비싸고 느림) |
| 7 | `IgnoredActors` | Ignore(무시하다) + Actors(등장인물들) | 블랙리스트. "이 사람들은 때려도 맞은 걸로 치지 마". 자기 자신, 아군 넣는 곳. `AddIgnoredActor`로 추가 |
| 8 | `DrawDebugType` | Draw(그리다) + Debug(오류찾기) + Type(종류) | CCTV 화면에 빨간 선 그려줄까? 헤더 기본값 `ForDuration(일정 시간 보여줘)`. 개발 끝나면 `None(그리지 마)`으로 바꿈. 예전에 에러 났던 자리 (9개 넣어서 개수 안 맞았음) |
| 9 | `OutHits` | 위 참고 | 빈 출석부 건네주기. 1명 맞으면 1줄, 3명 맞으면 3줄 적혀서 돌아옴 |
| 10 | `true` | bIgnoreSelf (Ignore + Self, 자기 자신) | 자동 "나 빼기" 버튼. 칼이 내 몸에 제일 가까우니 `false`면 휘두르자마자 내가 맞음 |

> 뒤에 숨은 3개 (`TraceColor, TraceHitColor, DrawTime`)는 기본값 (빨강/초록/5초)이라 생략 가능.

---

## 4. 결과 처리 — 왜 중복 체크가 필요한가?

```cpp
bool const bHit = ...; // "그래서 한 명이라도 맞았어?"
if (bHit)
{
	for (const FHitResult& Hit : OutHits) // 명단 한 명씩 꺼내기
```

| 코드 | 단어 풀이 | 예시로 이해하기 |
|---|---|---|
| `bool const bHit` | bool(참/거짓) + const(고정) + Hit(맞음) | "한 명이라도 맞았어?" `true`면 명단 확인, `false`면 끝. `const`라 뒤에서 실수로 못 바꿈 |
| `for (const FHitResult& Hit : OutHits)` | for(~마다 반복) + Hit 1장씩 꺼내기 | 출석부 첫 줄부터 한 줄씩 읽기. `&` = 원본 그대로 빌려보기 (복사 안 해서 빠름), `const` = 읽는 동안 고치지 마 |
| `Hit.GetActor()` | Get + Actor | 진단서에서 "환자 이름이 누구야?" 꺼내기 |
| `nullptr` | null(없음) + pointer(가리키는 것) | 빈 줄. `continue` = "이번 줄은 패스, 다음 줄" |
| `continue` | continue(계속하다, 다음으로) | 줄 서 있는데 앞 사람이 빈자리면 건너뛰고 다음 사람 |

```cpp
		if (CanHitActor(HitActor)) // "이번 휘두르기에서 이미 때린 애 아니야?"
		{
			AlreadyHitActors.Add(HitActor);
			if (OnHitActor.IsBound())
			{
				OnHitActor.Broadcast(Hit);
			}
		}
```

| 코드 | 단어 풀이 | 예시로 이해하기 |
|---|---|---|
| `CanHitActor` | Can(할 수 있나?) + Hit + Actor | "때려도 되는 애야?" 이미 때렸으면 `false`. 60fps라 1초 휘두르면 60번 맞은 걸로 되니까 방지용 |
| `AlreadyHitActors` | Already(이미) + Hit + Actors | 도장 찍힌 출석부. `TurnOnCollision`에서 `Empty()`로 비움 = "자, 이번 휘두르기 시작. 기억 지워!" |
| `OnHitActor` | On(~했을 때) + Hit + Actor | "누가 맞았을 때" 비상벨. `DECLARE_MULTICAST_DELEGATE`로 만든 방송국 |
| `IsBound` | Is(~인가?) + Bound(묶인, 연결된) | "이 방송 듣는 사람 있어?" 아무도 안 들으면 소리 질러도 소용없으니 확인 |
| `Broadcast` | Broadcast(방송하다) | 스피커로 "야, 맞았다! 정보는 이거야(Hit)" 소리 지르기. 들은 무기/캐릭터가 데미지, 피 이펙트 처리 |

---

## 5. 전체 흐름 (Tick + ON/OFF)

```
공격 애니메이션 시작 지점 (AnimNotify)
  → TurnOnCollision() : AlreadyHitActors 비우기 + bIsCollisionEnabled = true
    → Tick 매 프레임 CollisionTrace() : 비눗방울 굴리기
      → 처음 맞은 애만 Broadcast
  → TurnOffCollision() : bIsCollisionEnabled = false
```

| 함수 | 하는 일 | 예시 |
|---|---|---|
| `TurnOnCollision` | 출석부 비우고 검사 켜기 | "자, 이번 휘두르기 시작. 기억 지워!" |
| `TickComponent` | 켜져 있을 때만 검사 | CCTV는 스위치 켜져 있을 때만 녹화 |
| `TurnOffCollision` | 검사 끄기 | "휘두르기 끝. 이제 때려도 안 맞음" |

---

## 6. 헷갈리기 쉬운 포인트

1. `Single` vs `Multi`: `Interact(줍기)`는 1개면 되니 `Single`, 칼질은 여러 명 맞을 수 있으니 `Multi`.
2. `IgnoredActors` vs `bIgnoreSelf`: 7번은 수동 블랙리스트, 10번은 자동 "나 빼기". 둘 다 필요.
3. `OutHits`가 비어있는데 왜 넘기나? 함수가 채워주는 빈 종이라서. `const` 붙이면 컴파일 에러.
4. `DrawDebugType` 빠뜨리면? "13개인데 9개 넣었어요" 에러. `OutHits` 앞으로 한 칸 당겨진 현행 순서 주의.
