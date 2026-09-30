// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "DS1CombatComponent.generated.h"

class ADS1Weapon;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DS1_API UDS1CombatComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	ADS1Weapon* MainWeapon;
	
	/* 전투 활성화 상태인지? */
	UPROPERTY(EditAnywhere)
	bool bCombatEnabled = false;
	
	/* 현재 진행중인 공격 타입을 관리하기 위해, 마지막 AttackType 변수 생성 */
	UPROPERTY(VisibleAnywhere)
	FGameplayTag LastAttackType;
	
public:	
	UDS1CombatComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void SetWeapon(ADS1Weapon* NewWeapon);
	
public:
	FORCEINLINE bool IsCombatEnabled() const { return bCombatEnabled; }
	FORCEINLINE void SetCombatEnabled(const bool bEnabled) { bCombatEnabled = bEnabled; }
	
	FORCEINLINE ADS1Weapon* GetMainWeapon() const { return MainWeapon; };
	
	FORCEINLINE FGameplayTag GetLastAttackType() const { return LastAttackType; }; //변수 내용을 확인하기 위해 Getter 생성
	FORCEINLINE void SetLastAttackType(const FGameplayTag& NewAttackTypeTag) { LastAttackType = NewAttackTypeTag; }; //값을 설정하기 위해 Setter 생성
};
