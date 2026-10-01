# 공부노트 05 — OnHitActor + ApplyPointDamage (맞은 애한테 데미지 주기)

> 대상: `Source/DS1/Equipments/DS1Weapon.cpp:84-103`
> 한 문장 요약: "칼에 맞은 애를 꺼내서, 방향 + 계산된 데미지를添付해서, 공용 데미지 기계에 넣어주기"
> 이전 노트(04)와 연결: `CollisionTrace → OnHitActor.Broadcast(Hit) → 여기(OnHitActor)가 받음`

```cpp
/* 충돌된 액터에게 데미지를 전달해주는 함수 = 적에게 데미지를 줌 */
void ADS1Weapon::OnHitActor(const FHitResult& Hit)
{
	AActor* TargetActor = Hit.GetActor(); //충돌한 액터를 타겟액터로 저장

	//데미지 방향 - 현재 캐릭터의 전방벡터방향으로
	FVector DamageDirection = GetOwner()->GetActorForwardVector();

	//데미지 - 계산식 함수에서 가져와 저장
	float AttackDamage = GetAttackDamage();

	UGameplayStatics::ApplyPointDamage(
		TargetActor, //누구한테 데미지를 적용할지?
		AttackDamage, //데미지 값?
		DamageDirection, //데미지 방향?
		Hit, //데미지 적용 결과 어따 저장?
		GetOwner()->GetInstigatorController(), //공격자가 누구?
		this,
		nullptr);
}
```

---

## 0. 이 함수는 누가 불러주나? (선 연결)

`DS1Weapon.cpp:15-16` 생성자:
```cpp
WeaponCollision = CreateDefaultSubobject<UDS1WeaponCollisionComponent>("WeaponCollision");
WeaponCollision->OnHitActor.AddUObject(this, &ThisClass::OnHitActor);
```

| 코드 | 단어 풀이 | 예시로 이해하기 |
|---|---|---|
| `WeaponCollision` | Weapon(무기) + Collision(충돌) | 칼에 붙은 충돌 감지기. 04번 노트의 비눗방울 굴리는 그 부품 |
| `OnHitActor` | On(~했을 때) + Hit(맞음) + Actor | "누가 맞았을 때" 비상벨. 방송국 |
| `AddUObject` | Add(추가하다) + UObject(언리얼 물체) | 수신자 등록. "벨 울리면 이 번호로 전화해" 명단에 추가 |
| `&ThisClass::OnHitActor` | 이번 종류의 OnHitActor 함수 주소 | 경비실 전화번호. `()`가 없으니 지금 실행이 아니라 "나중에 불러줘" 명함만 넘김 |

> 예시: 비상벨(충돌 감지기)이 울리면 경비실(무기)이 자동으로 받게 개통 공사해둔 것. 그래서 `CollisionTrace`에서 `Broadcast(Hit)`만 하면 여기가 자동으로 불림.

---

## 1. 맞은 애 꺼내기

```cpp
AActor* TargetActor = Hit.GetActor();
```

| 코드 | 단어 풀이 | 예시로 이해하기 |
|---|---|---|
| `Hit` | Hit(맞음) | 진단서 1장. "어디에, 누가, 어떻게 맞았나"全部 들어있음. 04번에서 방송으로 날아온 그거 |
| `GetActor` | Get(가져오다) + Actor(등장인물) | 진단서에서 "환자 이름이 누구야?" 꺼내기 |
| `TargetActor` | Target(과녁, 목표) + Actor | 과녁 이름 메모지. 뒤에서 "이 사람한테 데미지 줘" 할 때 씀 |
| `AActor*` | 액터를 가리키는 종이 (`*` = 가리키는 것) | 환자 팔찌. 진짜 사람이 아니라 "저 사람"을 가리키는 쪽지 |

---

## 2. 데미지 방향 정하기

```cpp
FVector DamageDirection = GetOwner()->GetActorForwardVector();
```

| 코드 | 단어 풀이 | 예시로 이해하기 |
|---|---|---|
| `DamageDirection` | Damage(피해) + Direction(방향) | 밀치는 방향 화살표. 앞에서 밀면 뒤로 넘어지지? 그 화살표 |
| `GetOwner` | Get + Owner(주인) | 칼 주인 = 칼 든 캐릭터 |
| `GetActorForwardVector` | Forward(앞쪽) + Vector(방향+크기) | 캐릭터 코가 향한 앞 방향. 예: 내가 북쪽 보면 (북쪽 화살표)가 나옴 |

> 왜 필요하나? 넉백(밀려나기), 피 튀는 방향 계산에 씀. 없이 때리면 "아프긴 한데 어디로 넘어져야 할지 모름" 상태.

---

## 3. 데미지 숫자 계산하기

```cpp
float AttackDamage = GetAttackDamage();
```

| 코드 | 단어 풀이 | 예시로 이해하기 |
|---|---|---|
| `AttackDamage` | Attack(공격) + Damage | 처방전 용량. "이번에 몇 줄까?" 숫자 메모지 |
| `GetAttackDamage` | 계산식 함수 (`68-82줄`) | 계산기. 내용은 아래 참고 |

```cpp
float ADS1Weapon::GetAttackDamage() const
{
	if (const AActor* OwnerActor = GetOwner())
	{
		const FGameplayTag LastAttackType = CombatComponent->GetLastAttackType(); // 지금 무슨 공격 중이야?
		if (DamageMultiplierMap.Contains(LastAttackType)) // 가격표에 있어?
		{
			const float Multiplier = DamageMultiplierMap[LastAttackType]; // 배율 꺼내기
			return BaseDamege * Multiplier; // 기본공격력 x 배율
		}
	}
	return BaseDamege; // 표에 없으면 기본값 그대로
}
```

| 공격 종류 (`Tag`) | 배율 (`Multiplier`) | 예시 (기본 10이라면) |
|---|---|---|
| `Light` (일반) | 표에 없음 → 1.0배 | 10 |
| `Heavy` (강공) | 1.8배 | 18 |
| `Running` (질주공격) | 1.8배 | 18 |
| `Special` (특수) | 2.1배 | 21 |

* 단어 풀이: Damage(피해) + Multiplier(곱하는 수, 배율) + Map(지도, 가격표)
* 예시: 식당 메뉴판. "김치찌개는 기본, 특식은 2.1배 가격". `Light`는 메뉴판에 없어서 기본가 그대로 나감.

---

## 4. 실제 데미지 주기 — ApplyPointDamage 7개 인자

```cpp
UGameplayStatics::ApplyPointDamage(TargetActor, AttackDamage, DamageDirection, Hit,
	GetOwner()->GetInstigatorController(), this, nullptr);
```

| 코드 | 단어 풀이 | 예시로 이해하기 |
|---|---|---|
| `UGameplayStatics` | Gameplay(게임진행) + Statics(공용 도구함) | 학교 방송실처럼 누구나 갖다 쓰는 공용 기계함 |
| `ApplyPointDamage` | Apply(적용하다) + Point(점, 부위) + Damage | "점(부위) 데미지 주기" 기계. `ApplyDamage`는 부위 구분 없이 그냥 때리기, `Point`는 "머리 맞았나, 다리 맞았나" 부위 정보까지 넘김 |

| 순서 | 코드 | 단어 풀이 + 예시 |
|---|---|---|
| 1 | `TargetActor` | "누구한테?" 병원 환자 이름. 맞은 적 |
| 2 | `AttackDamage` | "얼마를?" 처방전 용량. 위에서 계산한 숫자 |
| 3 | `DamageDirection` | "어느 방향으로?" 밀치는 화살표. 넉백 방향 |
| 4 | `Hit` | 진단서 원본 통째로. "어깨 뼈에 맞았음, 충격 세기 이 정도" 상세 정보. 피 이펙트 위치 잡을 때 씀 |
| 5 | `GetOwner()->GetInstigatorController()` | Instigator(선동자, 시킨 사람) + Controller(조종자). 칼 주인의 리모컨 주인 = 플레이어 컨트롤러. 예: "이 킬 누구 거야?" 킬로그, 경험치 줄 때 필요해서 명의 적어둠 |
| 6 | `this` | Damage + Causer(일으킨 것). 흉기. 지금 이 무기 자신. 예: "무슨 칼로 때렸어?" 나중에 "이 칼로 맞으면 화상 추가" 만들 때 씀 |
| 7 | `nullptr` | null(없음) + pointer. 데미지 종류 지정 안 함 = 기본맛. 예: 양념 선택 안 함. 불/독 데미지 만들면 여기에 `UFireDamageType` 같은 걸 넣음 |

> 한 줄 요약: "맞은 애한테, 계산된 숫자만큼, 내 앞방향으로, 맞은 부위 정보까지添付해서, 조종자 명의로, 흉기는 이 칼이라고 적어서, 기본 데미지로 넣어줘"

---

## 5. 불리고 난 뒤에는?

`ApplyPointDamage`가 불리면, 맞은 적 쪽의 `TakeDamage / OnTakePointDamage`가 자동으로 불리면서 피 깎고 → 넉백하고 → 죽음 처리로 이어짐.

```
04 CollisionTrace (비눗방울로 찾기)
 → Broadcast (소리 지르기)
  → 05 OnHitActor (받아서 계산)
   → ApplyPointDamage (공용 기계에 넣기)
    → 적 TakeDamage (피 깎기)
```

---

## 6. 헷갈리기 쉬운 포인트

1. `Hit`를 왜 통째로 넘기나? `TargetActor`만 있으면 "누가"는 아는데 "어디에"를 모름. 피 이펙트를 어깨에 붙여야 하는데 발에 붙이면 이상하지? 그래서 진단서 원본을 같이 넘김.
2. `Instigator` vs `Causer`: 시킨 사람(플레이어) vs 흉기(칼). 킬로그는 전자, 속성 추가는 후자 담당.
3. `GetAttackDamage`는 왜 `LastAttackType`을 보나? 공격 버튼 누른 순간이 아니라 "맞은 순간"에 무슨 공격이었는지 확정하기 위해. 콤보 도중에 바뀌어도 맞은 시점 기준으로 계산됨.
