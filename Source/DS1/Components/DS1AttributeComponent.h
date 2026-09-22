// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DS1Define.h"
#include "Components/ActorComponent.h"
#include "DS1AttributeComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FDelegateOnAttributeChanged, EDS1AttributeType, float);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DS1_API UDS1AttributeComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	/** 스탯 변경 Delegate */
	FDelegateOnAttributeChanged OnAttributeChanged;

protected:

	UPROPERTY(EditAnywhere, Category="Stamina")
	float BaseStamina = 100.f;

	UPROPERTY(EditAnywhere, Category = "Stamina")
	float MaxStamina = 100.f;

	UPROPERTY(EditAnywhere, Category = "Stamina")
	float StaminaRegenRate = 0.2f;

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

	/** 스테미나 비율 계산 */
	FORCEINLINE float GetStaminaRatio() const { return BaseStamina / MaxStamina; };

	/** 스테미너가 충분한지 체크 */
	bool CheckHasEnoughStamina(float StaminaCost) const;

	/** 스테미너 차감 */
	void DecreaseStamina(float StaminaCost);

	/** 스테미너 재충전/중지 토글 */
	void ToggleStaminaRegeneration(bool bEnabled, float StartDelay = 2.f);

	/** 스텟 변경을 통지하는 Broadcast Function */
	void BroadcastAttributeChanged(EDS1AttributeType InAttributeType) const;

private:
	/** 스태미나 재충전 처리 핸들링 함수 */
	void RegenerateStaminaHandler();
		
};
