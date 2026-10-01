// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/DS1WeaponCollisionComponent.h"

UDS1WeaponCollisionComponent::UDS1WeaponCollisionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn)); //트레이스가 폰타입의 충돌을 감지
}


void UDS1WeaponCollisionComponent::BeginPlay()
{
	Super::BeginPlay();

	
}


void UDS1WeaponCollisionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsCollisionEnabled) 
	{
		CollisionTrace(); // 충돌활성화 `ON` 인 경우에 `Tick` 에서 감지
	}

}

// bIsCollisionEnabled(충돌활성화)변수를 ON/OFF 해주는 함수
void UDS1WeaponCollisionComponent::TurnOnCollision()
{
	AlreadyHitActors.Empty(); // 중복처리 되지 않도록, 충돌한 대상의 정보를 지움 - 기억상실
	bIsCollisionEnabled = true; //ON
}

void UDS1WeaponCollisionComponent::TurnOffCollision()
{
	bIsCollisionEnabled = false; //OFF
}

/** 무기에 메쉬를 설정하는 함수 */
void UDS1WeaponCollisionComponent::SetWeaponMesh(UPrimitiveComponent* MeshComponenet)
{
	WeaponMesh = MeshComponenet;
}

// 충돌 대상에서 제외할 액터를 추가/삭제 해주는 함수
void UDS1WeaponCollisionComponent::AddIgnoredActor(AActor* Actor)
{
	IgnoredActors.Add(Actor); // 추가
}

void UDS1WeaponCollisionComponent::RemoveIgnoredActor(AActor* Actor)
{
	IgnoredActors.Remove(Actor); //삭제
}

//충돌한 대상을 Array 에 담아서 확인할 함수
bool UDS1WeaponCollisionComponent::CanHitActor(AActor* Actor) const
{
	return AlreadyHitActors.Contains(Actor) == false;
}

void UDS1WeaponCollisionComponent::CollisionTrace()
{
	TArray<FHitResult> OutHits;
	
	// 무기 매쉬의 Start, End 소켓 위치를 가져와 저장;
	const FVector Start = WeaponMesh->GetSocketLocation(TraceStartSocketName);
	const FVector End = WeaponMesh->GetSocketLocation(TraceEndSocketName);
	
	//타원형태로 충돌을 감지하는 트레이스 함수로 충돌을 감지 
	bool const bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
		GetOwner(),
		Start, //시작 위치 ~
		End, //~끝나는 위치
		TraceRadius, //타원형의 반지름 크기Complex
		TraceObjectTypes, //감지대상의 타입 - 오브젝트타입 감지
		false,
		IgnoredActors, //감지 제외 대상
		DrawDebugType, //디버그 확인용 collision 표시 
		OutHits, //충돌한 대상의 정보
		true); //자신을 감지 하지 않도록

	if (bHit) //감지 하게 되면,
	{
		for (const FHitResult& Hit : OutHits) //충돌 된 대상들을 한번씩 다 확인함
		{
			AActor* HitActor = Hit.GetActor();

			if (HitActor == nullptr) // 다 확인하면 다음으로 넘어가고
			{
				continue;
			}

			if (CanHitActor(HitActor)) //감지한 대상을 함수로 AlreadyHitActors 에서 찾음
			{
				AlreadyHitActors.Add(HitActor); // 찾지 `못하면`, 감지한 대상을 Array 담음 - 60프레임마다 감지를 하기때문에 최초 1회만 감지하도록
				
				// Call OnHitActor BroadCast
				if (OnHitActor.IsBound())
				{
					OnHitActor.Broadcast(Hit); //데일 게이트에 바인딩 된 함수에게 충돌된 객체를 알린다
				}
			}
		}
	}
}

