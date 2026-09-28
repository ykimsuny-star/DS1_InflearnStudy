// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/DS1PickupItem.h"

#include "DS1Define.h"
#include "Equipments/DS1Equipment.h"

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

void ADS1PickupItem::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (EquipmentClass) //장비 블루프린트 클래스에 널 체크
	{
		/** CDO = Class Default Object 의 약어로, 이 클래스로 생성되는 오브젝트의 템플릿을 가르키는 것이고,
		 현 코딩에서는 해당 템플릿의 블루프린트 클래스에서 설정한 값을 가져오기 위함.*/
		if (ADS1Equipment* CDO = EquipmentClass->GetDefaultObject<ADS1Equipment>())
		{
			Mesh->SetStaticMesh(CDO->MeshAsset);
			Mesh->SetSimulatePhysics(true); //물리 시스템 활성화, 장비가 바닥에 떨어지도록
			
			Mesh->SetCollisionObjectType(COLLISION_OBJECT_INTERACTION); /* 오브젝트 타입을 `Interaction` 타입으로 설정 */
			Mesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore); /* 카메라의 충돌은 무시 */ 
			Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); /* 폰클래스랑은 오버랩 */
		}
	}
}

void ADS1PickupItem::Interact(AActor* InteractionActor)
{
	/** 디버깅 용 텍스트 출력
	GEngine->AddOnScreenDebugMessage(3, 1.5f, FColor::Cyan, TEXT("Hello!"));
	*/
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = InteractionActor;
	
	ADS1Equipment* SpawnItem = GetWorld()->SpawnActor<ADS1Equipment>(EquipmentClass, GetActorTransform(), SpawnParams);
	if (SpawnItem)
	{
		SpawnItem->EquipItem();
		Destroy();
	}
}

