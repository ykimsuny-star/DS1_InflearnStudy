// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/DS1AnimInstance.h"

#include "Characters/DS1Character.h"
#include "Components/DS1StateComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UDS1AnimInstance::UDS1AnimInstance()
{
}

void UDS1AnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    Character = Cast<ACharacter>(GetOwningActor());

    if (Character)
    {
        MovementComponent = Character->GetCharacterMovement();
    }
}

void UDS1AnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    if (Character == nullptr)
    {
        return;
    }

    if (MovementComponent == nullptr)
    {
        return;
    }

    Velocity = MovementComponent->Velocity;
    GroundSpeed = Velocity.Size2D();

    bShouldMove = GroundSpeed > 3.f && MovementComponent->GetCurrentAcceleration() != FVector::ZeroVector;

    bIsFalling = MovementComponent->IsFalling();
}

void UDS1AnimInstance::AnimNotify_ResetMovementInput()
{
    if (ADS1Character* LocalCharacter = Cast<ADS1Character>(GetOwningActor()))
    {
        LocalCharacter->GetStateComponent()->ToggleMovementInput(true);
    }
}
