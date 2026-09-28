// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/DS1PickupItem.h"

ADS1PickupItem::ADS1PickupItem()
{
	PrimaryActorTick.bCanEverTick = true;
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupItemMesh")); //언리얼 자체에 있는 스태틱메쉬를 부모로 기반, "PickupItemMesh"라는 이름을 붙여서 Mesh 생성 
	SetRootComponent(Mesh); // 뿌리구성요소 설정, 

}

void ADS1PickupItem::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADS1PickupItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADS1PickupItem::Interact(AActor* Interactor)
{
	//디버깅 용 텍스트 출력
	GEngine->AddOnScreenDebugMessage(3, 1.5f, FColor::Cyan, TEXT("Hello!"));
}

