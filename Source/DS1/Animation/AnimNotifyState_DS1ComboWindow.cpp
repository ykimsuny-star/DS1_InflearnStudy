// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotifyState_DS1ComboWindow.h"

#include "Characters/DS1Character.h"

UAnimNotifyState_DS1ComboWindow::UAnimNotifyState_DS1ComboWindow(const FObjectInitializer& Objectinitializer)
	:Super(Objectinitializer)
{
}

/* 노티파이 비긴 -> EnableComboWindow 함수 실행(노티파이가 실행되면 DS1Character가 가지고 있는 함수 실행) */
void UAnimNotifyState_DS1ComboWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (ADS1Character* Character = Cast<ADS1Character>(MeshComp->GetOwner()))
	{
		Character->EnableComboWindow();
	}
}

/* 노티파이 엔드 -> DisableComboWindow 함수 실행(노티파이가 종료되면 DS1Character가 가지고 있는 함수 실행) */
void UAnimNotifyState_DS1ComboWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (ADS1Character* Character = Cast<ADS1Character>(MeshComp->GetOwner()))
	{
		Character->DisableComboWindow();
	}
}
