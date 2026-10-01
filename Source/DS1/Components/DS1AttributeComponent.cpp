// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/DS1AttributeComponent.h"

#include "DS1GameplayTags.h"
#include "DS1StateComponent.h"

UDS1AttributeComponent::UDS1AttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UDS1AttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	
}


void UDS1AttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

bool UDS1AttributeComponent::CheckHasEnoughStamina(float StaminaCost) const
{
	return BaseStamina >= StaminaCost;
}

void UDS1AttributeComponent::DecreaseStamina(float StaminaCost)
{
	BaseStamina = FMath::Clamp(BaseStamina - StaminaCost, 0.f, MaxStamina);

	BroadcastAttributeChanged(EDS1AttributeType::Stamina);
}

void UDS1AttributeComponent::ToggleStaminaRegeneration(bool bEnabled, float StartDelay)
{
	if (bEnabled)
	{
		if (GetWorld()->GetTimerManager().IsTimerActive(StaminaRegenTimerHandle) == false)
		{
			GetWorld()->GetTimerManager().SetTimer(StaminaRegenTimerHandle, this, &ThisClass::RegenerateStaminaHandler, 0.1f, true, StartDelay);
		} //딜레이 시간 후 회복하는 주기 - 0.1초에 1번씩 회복
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(StaminaRegenTimerHandle);
	}
}

void UDS1AttributeComponent::BroadcastAttributeChanged(EDS1AttributeType InAttributeType) const
{
	if (OnAttributeChanged.IsBound())
	{
		// Type에 따라서 값을 골라준다.
		float Ratio = 0.f;
		switch (InAttributeType)
		{
		case EDS1AttributeType::Stamina:
			Ratio = GetStaminaRatio();
			break;

		case EDS1AttributeType::Health:
			break;
		}

		OnAttributeChanged.Broadcast(InAttributeType, Ratio);
	}
}

void UDS1AttributeComponent::TakeDamageAmount(float DamageAmount)
{
	//체력 차감
	BaseHealth = FMath::Clamp(BaseHealth - DamageAmount, 0.f, MaxHealth); //들어온 데미지 만큼 체력에서 감소
	
	BroadcastAttributeChanged(EDS1AttributeType::Health); //체력이 깍였다고 브로드 캐스팅

	if (BaseHealth <= 0.f) //체력이 0이하로 떨어지면
	{
		//Call Death Delegate - 죽음을 알림
		if (OnDeath.IsBound())
		{
			OnDeath.Broadcast();			
		}
		
		//Set Death State
		if (UDS1StateComponent* StateComp = GetOwner()->FindComponentByClass<UDS1StateComponent>())
		{
			StateComp->SetState(DS1GameplayTags::Character_State_Death); //캐릭터를 `죽음`상태로 변경
		}
	}
}

void UDS1AttributeComponent::RegenerateStaminaHandler()
{
	BaseStamina = FMath::Clamp(BaseStamina + StaminaRegenRate, 0.f, MaxStamina);

	BroadcastAttributeChanged(EDS1AttributeType::Stamina);

	if (BaseStamina >= MaxStamina)
	{
		ToggleStaminaRegeneration(false);
	}
}

