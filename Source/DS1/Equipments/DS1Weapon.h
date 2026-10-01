// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Equipments/DS1Equipment.h"
#include "DS1Weapon.generated.h"

class UDS1WeaponCollisionComponent;
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
	
//Component Section
protected:
	UPROPERTY(VisibleAnywhere)
	UDS1WeaponCollisionComponent* WeaponCollision; //충돌감지 컴포넌트를 무기에 부착
	
protected:
	UPROPERTY()
	UDS1CombatComponent* CombatComponent;
	
//Data Section
protected:
	// 무기의 각 공격상태 마다 소모되는 스테미나 양을 다르게 설정
	UPROPERTY(EditAnywhere)
	TMap<FGameplayTag, float> StaminaCostMap; //게임플레이 테크를 키값으로, 스테미나 양 = float 값을 정함
	
	/** 기본 데미지 */
	UPROPERTY(EditAnywhere)
	float BaseDamege = 15.f;
	
	/** 데미지 승수 - 각 무기의 모션마다 다른 데미지를 구현하기 위한 맵 */
	UPROPERTY(EditAnywhere)
	TMap<FGameplayTag, float> DamageMultiplierMap;
	
public:
	ADS1Weapon();
	
public:
	virtual void EquipItem() override;
	
	UAnimMontage* GetMontageForTag(const FGameplayTag& Tag, const int32 Index = 0) const;
	
	/* 무기의 스테미나 소모값을 받아오는 함수 */
	float GetStaminaCost(const FGameplayTag& InTag) const;
	
	/* 공격 데미지를 계산하는 함수 */
	float GetAttackDamage() const;
	
	/* 외부에서 참조할수 있도록 Getter */
	FORCEINLINE FName GetEquipSocketName() const { return EquipSocketName; };
	FORCEINLINE FName GetUnequipSocketName() const { return UnequipSocketName; };
	FORCEINLINE UDS1WeaponCollisionComponent* GetCollision() const { return WeaponCollision; }; 
	
public:
	/** 무기의 Collision에 검출된 Actor에 Damage를 전달 */
	void OnHitActor(const FHitResult& Hit);
};
