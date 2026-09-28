// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/DS1CombatComponent.h"

#include "Characters/DS1Character.h"
#include "Equipments/DS1Weapon.h"
#include "Items/DS1PickupItem.h"

UDS1CombatComponent::UDS1CombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UDS1CombatComponent::BeginPlay()
{
	Super::BeginPlay();

	
}


void UDS1CombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UDS1CombatComponent::SetWeapon(ADS1Weapon* NewWeapon)
{
	if (::IsValid(MainWeapon)) //다른 무기를 보유 중인지 확인
	{
		if (ADS1Character* OwnerCharacter = Cast<ADS1Character>(GetOwner()))
		{
			ADS1PickupItem* PickupItem = GetWorld()->SpawnActorDeferred<ADS1PickupItem>(ADS1PickupItem::StaticClass(), OwnerCharacter->GetActorTransform(), nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
			PickupItem->SetEquipmentClass(MainWeapon->GetClass());
			PickupItem->FinishSpawning(GetOwner()->GetActorTransform());
			
			MainWeapon->Destroy();
		}
	}/** 스폰을 실행하고 스폰이 되기 전 무기클래스를 전달해주고 피니쉬스포닝 함수를 이용해서 스폰 완료처리
	스폰이 완료가 되면서 땅바닥에 기존에 있던 무기의 정보를 이용한 픽업아이템이 생성 되겠죠. 기존에 있던 무기는 이제 땅바닥에 떨궜으니까 삭제를 해주고,
	새로 집은 무기를 캐싱을 해줌. */
	
	MainWeapon = NewWeapon;
}

