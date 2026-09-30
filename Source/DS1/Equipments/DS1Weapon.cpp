// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipments/DS1Weapon.h"

#include "DS1GameplayTags.h"
#include "Components/DS1CombatComponent.h"
#include "Data/DS1MontageActionData.h"

ADS1Weapon::ADS1Weapon()
{
	/* 각 공격유형에 따라 필요한 스테미나 소모값 설정 */
	StaminaCostMap.Add(DS1GameplayTags::Character_Attack_Light, 7.f); 
	StaminaCostMap.Add(DS1GameplayTags::Character_Attack_Running, 12.f);
	StaminaCostMap.Add(DS1GameplayTags::Character_Attack_Special, 15.f);
	StaminaCostMap.Add(DS1GameplayTags::Character_Attack_Heavy, 20.f);
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
