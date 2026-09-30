// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Equipments/DS1Equipment.h"
#include "DS1Weapon.generated.h"

class UDS1MontageActionData;
class UDS1CombatComponent;
/**
 * 
 */
UCLASS()
class DS1_API ADS1Weapon : public ADS1Equipment
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Equipment | Socket")
	FName EquipSocketName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Equipment | Socket")
	FName UnequipSocketName;
	
	/* 언리얼에서 생성한 데이타 에셋을 프로퍼티로 받고 있음 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Equipment | Animation")
	UDS1MontageActionData* MontageActionData;
	
protected:
	UPROPERTY()
	UDS1CombatComponent* CombatComponent;
	
// 무기의 각 공격상태 마다 소모되는 스테미나 양을 다르게 설정
protected:
	UPROPERTY(EditAnywhere)
	TMap<FGameplayTag, float> StaminaCostMap; //게임플레이 테크를 키값으로, 스테미나 양 = float 값을 정함
	
public:
	ADS1Weapon();
	
public:
	virtual void EquipItem() override;
	
	UAnimMontage* GetMontageForTag(const FGameplayTag& Tag, const int32 Index = 0) const;
	
	/* 무기의 스테미나 소모값을 받아오는 함수 */
	float GetStaminaCost(const FGameplayTag& InTag) const;
	
	/* 외부에서 참조할수 있도록 Socket 2개 Get */
	FORCEINLINE FName GetEquipSocketName() const { return EquipSocketName; };
	FORCEINLINE FName GetUnequipSocketName() const { return UnequipSocketName; };
};
