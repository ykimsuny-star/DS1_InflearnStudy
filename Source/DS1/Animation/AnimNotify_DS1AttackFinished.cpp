// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify_DS1AttackFinished.h"

#include "Characters/DS1Character.h"

UAnimNotify_DS1AttackFinished::UAnimNotify_DS1AttackFinished(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

/* 노티파이가 발동이 되면 캐릭터의 AttackFinished 함수에 ComboResetDelay 파라미터(매개변수)를 넣어서 실행 */
void UAnimNotify_DS1AttackFinished::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (ADS1Character* Character = Cast<ADS1Character>(MeshComp->GetOwner()))
	{
		Character->AttackFinished(ComboResetDelay);
	}
}
