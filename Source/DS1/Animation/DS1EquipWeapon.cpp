// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/DS1EquipWeapon.h"

#include "DS1GameplayTags.h"
#include "Components/DS1CombatComponent.h"
#include "Equipments/DS1Weapon.h"

UDS1EquipWeapon::UDS1EquipWeapon(const FObjectInitializer& ObjectInitializer)
{
}

void UDS1EquipWeapon::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	//MeshComp 의 Owner 즉, 캐릭터를 확인 가져옴 (Owner에 저장)
	if (const AActor* Owner = MeshComp->GetOwner())
	{
		// Owner(캐릭터)의 CombatComponent를 가져와서 (CombatComponent에 저장)
		if (UDS1CombatComponent* CombatComponent = Owner->GetComponentByClass<UDS1CombatComponent>())
		{
			//CombatComponent의 MainWeapon 함수를 통해 현재 가지고 있는 무기 저장(MainWeapon에 보유중인 무기저장) 
			if (ADS1Weapon* MainWeapon = CombatComponent->GetMainWeapon())
			{
				bool bCombatEnabled = CombatComponent->IsCombatEnabled(); //bCombatEnabled 초기화 
				FName WeaponSocketName;
				
				/* 장착 상태에 따라서 변수 상태 변경 */
				if (MontageActionTag == DS1GameplayTags::Character_Action_Equip) //손에 장착
				{
					bCombatEnabled = true; //전투상태(활성화)
					WeaponSocketName = MainWeapon->GetEquipSocketName(); //소켓 이름도 따라 설정
				}
				else if (MontageActionTag == DS1GameplayTags::Character_Action_Unequip) //등에 장착
				{
					bCombatEnabled = false; // 비전투상태(비활성화)
					WeaponSocketName = MainWeapon->GetUnequipSocketName(); //소켓 이름도 따라 설정
				}
				
				// AttachToPlayer 함수보다 먼저 호출해야 한다.
				CombatComponent->SetCombatEnabled(bCombatEnabled); //함수에 값 반환 (boolean)
				MainWeapon->AttachToOwner(WeaponSocketName); //함수에 이름
			}
		}
	}
}
