// Fill out your copyright notice in the Description page of Project Settings.
// Binding

#include "Equipments/DS1Weapon.h"

#include "DS1GameplayTags.h"
#include "Components/DS1CombatComponent.h"
#include "Components/DS1WeaponCollisionComponent.h"
#include "Data/DS1MontageActionData.h"
#include "Kismet/GameplayStatics.h"

ADS1Weapon::ADS1Weapon()
{
	/* 생성자로 무기에 충돌을 처리할 수 있는 Component를 생성, 충돌이 발생하면 DELEGATE에 전달할 OnHitActor 함수 바인딩 */
	WeaponCollision = CreateDefaultSubobject<UDS1WeaponCollisionComponent>("WeaponCollision");
	WeaponCollision->OnHitActor.AddUObject(this, &ThisClass::OnHitActor);
	
	/* 각 공격유형에 따라 필요한 스테미나 소모값 설정 */
	StaminaCostMap.Add(DS1GameplayTags::Character_Attack_Light, 7.f); 
	StaminaCostMap.Add(DS1GameplayTags::Character_Attack_Running, 12.f);
	StaminaCostMap.Add(DS1GameplayTags::Character_Attack_Special, 15.f);
	StaminaCostMap.Add(DS1GameplayTags::Character_Attack_Heavy, 20.f);
	
	/* 각 공격유형에 따른 데미지 설정 */
	DamageMultiplierMap.Add(DS1GameplayTags::Character_Attack_Heavy, 1.8f);
	DamageMultiplierMap.Add(DS1GameplayTags::Character_Attack_Running, 1.8f);
	DamageMultiplierMap.Add(DS1GameplayTags::Character_Attack_Special, 2.1f);
}

void ADS1Weapon::EquipItem()
{
	Super::EquipItem();
	
	CombatComponent = GetOwner()->GetComponentByClass<UDS1CombatComponent>(); // 컴벳 컴포넌트를 가져와서 캐싱함

	if (CombatComponent) //널채크
	{
		CombatComponent->SetWeapon(this); // 컴뱃 컴포넌트에도 이 무기가 장착 되었다고 캐싱
		
		//IsCombatEnabled 상태에 따라서 저장한 AttachSocket
		const FName AttachSocket = CombatComponent->IsCombatEnabled() ? EquipSocketName : UnequipSocketName;
		
		AttachToOwner(AttachSocket); // AttachToOwner 함수로 캐릭터의 스켈레톤 메쉬에 장비를 부착
		
		// 무기의 충돌 트레이스 컴포넌트에 무기 메쉬 컴포넌트를 설정합니다.
		WeaponCollision->SetWeaponMesh(Mesh);
		
		// 무기를 소유한 OwnerActor를 충돌에서 무시합니다.
		WeaponCollision->AddIgnoredActor(GetOwner());
	}
}

/* GameplayTag 키값에 맞는 애니메이션을 호출하도록 하는 함수 */ 
UAnimMontage* ADS1Weapon::GetMontageForTag(const FGameplayTag& Tag, const int32 Index) const
{
	return MontageActionData->GetMontageForTag(Tag, Index);
}

float ADS1Weapon::GetStaminaCost(const FGameplayTag& InTag) const
{
	if (StaminaCostMap.Contains(InTag)) //StaminaCostMap 안에 Tag 가 있는지 확인
	{
		return StaminaCostMap[InTag]; //확인한 테크 값을 반환(return)
	}
	return 0.f;
}

float ADS1Weapon::GetAttackDamage() const
{
	if (const AActor* OwnerActor = GetOwner())
	{
		const FGameplayTag LastAttackType = CombatComponent->GetLastAttackType(); //현재 실행중인 공격타입을 가져옴

		if (DamageMultiplierMap.Contains(LastAttackType)) // 가져온 공격타입을 Map에서 찾음
		{
			const float Multiplier = DamageMultiplierMap[LastAttackType]; // 저장된 공격타입의 데미지 설정값을 가져와서
			return BaseDamege * Multiplier; //기본데미지에 곱해줌
		}
	}
	
	return BaseDamege; //최종적으로 계산식을 마친 데미지 출력 (계산하지 않으면 그대로)
}

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
