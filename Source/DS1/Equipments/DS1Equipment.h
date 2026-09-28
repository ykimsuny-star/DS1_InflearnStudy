// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DS1Equipment.generated.h"

UCLASS()
class DS1_API ADS1Equipment : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category="Equipment | Mesh")
	UStaticMesh* MeshAsset; // 장비아이템의 외형을 표시해줄 스테틱 메쉬 

	UPROPERTY(VisibleAnywhere, Category="Equipment | Mesh")
	UStaticMeshComponent* Mesh; // 만든 스테틱 메시를 랜더링 해줄 컴포넌트 생성
	// 컴포넌트에 매쉬를 설정해야 한다
	
public:	
	ADS1Equipment();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void OnConstruction(const FTransform& Transform) override;
	
public:
	virtual void EquipItem(); //자식인 Weapon.h 에서 구현 할 예정임으로 작성 안함
	
	virtual void UnequipItem(); //자식인 Weapon.h 에서 구현 할 예정임으로 작성 안함
	
	virtual void AttachToOwner(FName SocketName); // 특정부위에 장비를 장착하게 하는 코드
};
