// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/DS1PlayerHUDWidget.h"

#include "DS1StatBarWidget.h"
#include "Components/DS1AttributeComponent.h"


UDS1PlayerHUDWidget::UDS1PlayerHUDWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UDS1PlayerHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (APawn* OwningPawn = GetOwningPlayerPawn())
	{
		if (UDS1AttributeComponent* Attribute = OwningPawn->GetComponentByClass<UDS1AttributeComponent>())
		{
			Attribute->OnAttributeChanged.AddUObject(this, &ThisClass::OnAttributeChanged);
			Attribute->BroadcastAttributeChanged(EDS1AttributeType::Stamina);
		}
	}
}

void UDS1PlayerHUDWidget::OnAttributeChanged(EDS1AttributeType AttributeType, float InValue)
{
	switch (AttributeType)
	{
	case EDS1AttributeType::Stamina:
		StaminaBarWidget->SetRatio(InValue);
		break;
	case EDS1AttributeType::Health:
		break;
	}
}
