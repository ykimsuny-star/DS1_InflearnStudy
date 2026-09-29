// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipments/DS1Equipment.h"

#include "GameFramework/Character.h"

ADS1Equipment::ADS1Equipment()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EquipmentMesh")); //스테틱 메쉬 컴포넌트 생성
	SetRootComponent(Mesh); //루트 컴포넌트로 설정
	Mesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName); //외형만 표시하고 콜리전은 NO 콜리젼으로 설정
}

void ADS1Equipment::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADS1Equipment::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADS1Equipment::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (MeshAsset)
	{
		Mesh->SetStaticMesh(MeshAsset);
	}
}

void ADS1Equipment::EquipItem() //자식인 Weapon.h 에서 구현 할 예정임으로 작성 안함
{
}

void ADS1Equipment::UnequipItem() //자식인 Weapon.h 에서 구현 할 예정임으로 작성 안함
{
}

/* 가지고 온 캐릭터 스켈레탈 메시의 특정 소켓에 장비 아이템을 장착을 해주는 코드 */
void ADS1Equipment::AttachToOwner(FName SocketName)
{
	if (ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner())) //GetOwner 함수로 캐릭터를 가져옴
	{
		if (USkeletalMeshComponent* CharacterMesh = OwnerCharacter->GetMesh()) //캐릭터의 스켈레탈 메시 컴포넌트를 가지고 옴
		{
			AttachToComponent(CharacterMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), SocketName); 
		} 
	}
}

