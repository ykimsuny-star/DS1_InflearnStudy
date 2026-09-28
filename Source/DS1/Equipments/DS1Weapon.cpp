// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipments/DS1Weapon.h"

#include "Components/DS1CombatComponent.h"

ADS1Weapon::ADS1Weapon()
{
}

void ADS1Weapon::EquipItem()
{
	Super::EquipItem();
	
	CombatComponent = GetOwner()->GetComponentByClass<UDS1CombatComponent>(); // 컴벳 컴포넌트를 가져와서 캐싱함

	if (CombatComponent) //널채크
	{
		CombatComponent->SetWeapon(this); // 컴뱃 컴포넌트에도 이 무기가 장착 되었다고 캐싱
		
		AttachToOwner(UnequipSocketName); // AttachToOwner 함수로 캐릭터의 스켈레톤 메쉬에 장비를 부착
	}
}
