// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DS1Define.h"
#include "Components/ActorComponent.h"
#include "DS1AttributeComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FDelegateOnAttributeChanged, EDS1AttributeType, float);
DECLARE_MULTICAST_DELEGATE(FOnDeath)

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DS1_API UDS1AttributeComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	/** 스탯 변경 Delegate */
	FDelegateOnAttributeChanged OnAttributeChanged;
	
	/** 죽음을 알리는 Delegate */
	FOnDeath OnDeath;

protected:

	UPROPERTY(EditAnywhere, Category="Stamina") //현재 스테미나
	float BaseStamina = 100.f;

	UPROPERTY(EditAnywhere, Category = "Stamina") //최대 스테미나
	float MaxStamina = 100.f;

	UPROPERTY(EditAnywhere, Category = "Stamina") //스테미나 회복 속도
	float StaminaRegenRate = 0.5f;
	
	UPROPERTY(EditAnywhere, Category = "Health") //현재 체력
	float BaseHealth = 100.f;
	
	UPROPERTY(EditAnywhere, Category = "Health") //최대 체력
	float MaxHealth = 100.f;
	
	/** 스태미나 재충전 타이머 핸들 */
	FTimerHandle StaminaRegenTimerHandle;

public:	
	UDS1AttributeComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	FORCEINLINE float GetBaseStamina() const { return BaseStamina; };
	FORCEINLINE float GetMaxStamina() const { return MaxStamina; };
	FORCEINLINE float GetBaseHealth() const { return MaxStamina; };
	FORCEINLINE float GetMaxHealth() const { return MaxStamina; };
	
	/** 스테미나 비율 계산 */
	FORCEINLINE float GetStaminaRatio() const { return BaseStamina / MaxStamina; };

	/** 스테미너가 충분한지 체크 */
	bool CheckHasEnoughStamina(float StaminaCost) const;

	/** 스테미너 차감 */
	void DecreaseStamina(float StaminaCost);

	/** 스테미너 재충전/중지 토글 */
	void ToggleStaminaRegeneration(bool bEnabled, float StartDelay = 1.5f);

	/** 스텟 변경을 통지하는 Broadcast Function */
	void BroadcastAttributeChanged(EDS1AttributeType InAttributeType) const;
	
	/** 데미지를 받아 체력을 깍아줄 함수 */
	void TakeDamageAmount(float DamageAmount);

private:
	/** 스태미나 재충전 처리 핸들링 함수 */
	void RegenerateStaminaHandler();
		
};
