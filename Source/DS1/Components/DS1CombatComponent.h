// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
};
