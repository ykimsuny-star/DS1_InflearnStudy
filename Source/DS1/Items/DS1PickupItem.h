// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/DS1Interact.h"
#include "DS1PickupItem.generated.h"

class ADS1Equipment;

UCLASS()
class DS1_API ADS1PickupItem : public AActor, public IDS1Interact
{
	GENERATED_BODY()
	
public:	
	ADS1PickupItem();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	
	virtual void Interact(AActor* Interactor) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Item")
	UStaticMeshComponent* Mesh; //외형을 표시해줄 스태틱 메시의 컴포넌트 제작
	
	/** 캐릭터가 장비를 픽업할 때, 장비가 아닌 다른 오브젝트를 픽업 할 수 없도록 인터페이스를 만듦.
	 * 언리얼에서 독자적으로 다중상속을 이용해 인터페이스를 구현, 제공하고 있음. 이를 활용하여 제작함.
	 * 함수로 만들어서 처리를 하게되면, 장비가 아닌 수많은 오브젝트 마다마다 처리해주는 코드를 입력하게 되기에, 관리하기 복잡해짐.
	 */
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Item")
	TSubclassOf<ADS1Equipment> EquipmentClass;
};
