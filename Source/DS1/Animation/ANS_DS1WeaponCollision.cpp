// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/ANS_DS1WeaponCollision.h"

#include "Components/DS1CombatComponent.h"
#include "Components/DS1WeaponCollisionComponent.h"
#include "Equipments/DS1Weapon.h"

UANS_DS1WeaponCollision::UANS_DS1WeaponCollision(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

/* 노티파이 만나면 */
void UANS_DS1WeaponCollision::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (const AActor* OwnerActor = MeshComp->GetOwner()) //캐릭터를 가져옴
	{
		if (const UDS1CombatComponent* CombatComponent = OwnerActor->GetComponentByClass<UDS1CombatComponent>()) //캐릭터의 컴벳 컴포넌트를 가져옴
		{
			const ADS1Weapon* Weapon = CombatComponent->GetMainWeapon(); //컴벳 컴포넌트의 무기를 가져옴
			if (::IsValid(Weapon)) //유효성 체크
			{
				Weapon->GetCollision()->TurnOnCollision(); // 충돌 감지 `활성화` - Collision ON!
			}
		}
	}
}

/* 노티파이 끝나면 */
void UANS_DS1WeaponCollision::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (const AActor* OwnerActor = MeshComp->GetOwner()) //캐릭터를 가져옴
	{
		if (UDS1CombatComponent* CombatComponent = OwnerActor->GetComponentByClass<UDS1CombatComponent>()) //캐릭터의 컴벳 컴포넌트를 가져옴
		{
			const ADS1Weapon* Weapon = CombatComponent->GetMainWeapon(); //컴벳 컴포넌트의 무기를 가져옴
			if (::IsValid(Weapon)) //유효성 체크
			{
				Weapon->GetCollision()->TurnOffCollision(); // 충돌 감지 `비활성화` - Collision OFF!
			}
		}
	}
}
