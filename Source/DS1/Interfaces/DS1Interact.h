// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DS1Interact.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDS1Interact : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DS1_API IDS1Interact
{
	GENERATED_BODY()

public:
	virtual void Interact(AActor* Interactor) = 0; //PicupItem.h 에서 인터페이스 함수를 구현해서 통신할 수 있도록 처리
};
