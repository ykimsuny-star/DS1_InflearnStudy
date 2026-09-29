// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/DS1Character.h"

#include "DS1GameplayTags.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/DS1CombatComponent.h"
#include "Components/DS1AttributeComponent.h"
#include "Components/DS1StateComponent.h"
#include "Equipments/DS1Weapon.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interfaces/DS1Interact.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UI/DS1PlayerHUDWidget.h"

ADS1Character::ADS1Character()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);

	/** 이동, 감속 속도 */
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->SetRelativeRotation(FRotator(-30.f, 0.f, 0.f));
	CameraBoom->bUsePawnControlRotation = true;


	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom);
	FollowCamera->bUsePawnControlRotation = false;

	AttributeComponent = CreateDefaultSubobject<UDS1AttributeComponent>(TEXT("Attribute"));
	StateComponent = CreateDefaultSubobject<UDS1StateComponent>(TEXT("State"));
	CombatComponent = CreateDefaultSubobject<UDS1CombatComponent>(TEXT("Combat"));

}

void ADS1Character::BeginPlay()
{
	Super::BeginPlay();

	if (PlayerHUDWidgetClass)
	{
		PlayerHUDWidget = CreateWidget<UDS1PlayerHUDWidget>(GetWorld(), PlayerHUDWidgetClass);
		if (PlayerHUDWidget)
		{
			PlayerHUDWidget->AddToViewport();
		}
	}
}

void ADS1Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	/** 스테미나 디버깅용 텍스트 출력
	GEngine->AddOnScreenDebugMessage(0, 1.5f, FColor::Cyan, FString::Printf(TEXT("Stamina : %f"), AttributeComponent->GetBaseStamina()));
	GEngine->AddOnScreenDebugMessage(2, 1.5f, FColor::Cyan, FString::Printf(TEXT("MaxWalkSpeed : %f"), GetCharacterMovement()->MaxWalkSpeed));
	*/
}

void ADS1Character::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void ADS1Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Look);
			
		//질주
		EnhancedInputComponent->BindAction(SprintRollingAction, ETriggerEvent::Triggered, this, &ThisClass::Sprinting);
		EnhancedInputComponent->BindAction(SprintRollingAction, ETriggerEvent::Completed, this, &ThisClass::StopSprint);
		// 구르기
		EnhancedInputComponent->BindAction(SprintRollingAction, ETriggerEvent::Canceled, this, &ThisClass::Rolling);
		// 인터렉션
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ThisClass::Interact);
		// 전투 활성/비활성
		EnhancedInputComponent->BindAction(ToggleCombatAction, ETriggerEvent::Started, this, &ThisClass::ToggleCombat);
	}

}

bool ADS1Character::IsMoving() const
{
	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		return MovementComp->Velocity.Size2D() > 3.f && MovementComp->GetCurrentAcceleration() != FVector::Zero();
	}

	return false;
}

/** 토글을 가능한 상태인지? */
bool ADS1Character::CanToggleCombat() const
{
	check(StateComponent);
	
	FGameplayTagContainer CheckTags; //컨테이너 생성
	CheckTags.AddTag(DS1GameplayTags::Character_State_Attacking); //공격상태 추가 
	CheckTags.AddTag(DS1GameplayTags::Character_State_Rolling); //구르기 상태 추가
	CheckTags.AddTag(DS1GameplayTags::Character_State_GeneralAction); //GeneralAction 상태 추가
	
	return StateComponent->IsCurrentStateEqualToAny(CheckTags) == false; //캐릭터의 현재 상태 들을 비활성화
}

/** 이동 */
void ADS1Character::Move(const FInputActionValue& Values)
{
	check(StateComponent);

	// 이동 입력 가능 상태인지 체크.
	if (StateComponent->MovementInputEnabled() == false)
	{
		return;
	}

	FVector2D MovementVector = Values.Get<FVector2D>();

	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotator(0, Rotation.Yaw, 0);

		const FVector ForwardVector = FRotationMatrix(YawRotator).GetUnitAxis(EAxis::X);
		const FVector RightVector = FRotationMatrix(YawRotator).GetUnitAxis(EAxis::Y);

		// 주어진 월드 방향 벡터(보통 정규화됨)를 따라 'ScaleValue'만큼 스케일된 이동 입력을 추가합니다. 
		// ScaleValue가 0보다 작으면, 이동은 반대 방향으로 이루어집니다.
		// ScaleValue는 아날로그 입력에 사용될 수 있습니다. 
		// 예를 들어, 0.5 값은 정상 값의 절반을 적용하고, -1.0은 방향을 반대로 합니다.
		AddMovementInput(ForwardVector, MovementVector.Y);
		AddMovementInput(RightVector, MovementVector.X);
		
	}
}

/** 카메라 방향 */
void ADS1Character::Look(const FInputActionValue& Values)
{
	FVector2D LookDirection = Values.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookDirection.X);
		AddControllerPitchInput(LookDirection.Y);
	}
}

/** 질주 */
void ADS1Character::Sprinting()
{
	if (AttributeComponent->CheckHasEnoughStamina(5.f) && IsMoving())
	{
		AttributeComponent->ToggleStaminaRegeneration(false);

		GetCharacterMovement()->MaxWalkSpeed = SprintingSpeed;

		AttributeComponent->DecreaseStamina(0.1f);
	}
	else
	{
		StopSprint();
	}
}

/** 질주 중단 */
void ADS1Character::StopSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	AttributeComponent->ToggleStaminaRegeneration(true);
}

/** 구르기 */
void ADS1Character::Rolling()
{
	check(AttributeComponent);
	check(StateComponent);

	if (AttributeComponent->CheckHasEnoughStamina(15.f))
	{
		// 스태미나 재충전 멈춤
		AttributeComponent->ToggleStaminaRegeneration(false);

		// 이동입력 처리 무시.
		StateComponent->ToggleMovementInput(false);

		// 스태미나 차감.
		AttributeComponent->DecreaseStamina(15.f);

		// 구르기 애니메이션 재생
		PlayAnimMontage(RollingMontage);

		StateComponent->SetState(DS1GameplayTags::Character_State_Rolling);

		// 스태미나 재충전 시작
		AttributeComponent->ToggleStaminaRegeneration(true, 1.5f);
	}
}

/** 인터렉션(상호작용) */
void ADS1Character::Interact() //CollisionTrace(콜리전트레이스)를 활용해서 
{ 
	FHitResult OutHit;
	const FVector Start = GetActorLocation(); // DS1캐릭터의 로케이션 값
	const FVector End = Start;
	constexpr float Radius = 100.f;
	
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes; 
	/** (강의영상 NO.8 30:00 참고)에디터 프로젝트 세팅에서 새로운 콜리전 오브젝트 채널을 만들고,적용시키는 방법과 채널명을 재정의 해서 사용 */
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(COLLISION_OBJECT_INTERACTION));
	TArray<AActor*> ActorsToIgnore;
	
	bool bHit = UKismetSystemLibrary::SphereTraceSingleForObjects( //구 형태의 감지 영역에, 들어오는 물체를 감지하는 함수 
		this, 
		Start, //감지영역의 시작
		End, //감지영역의 끝
		Radius, //감지영역의 크기
		ObjectTypes, //어떤 타입의 오브젝트만 감지하도록 걸러주는 설정
		false,
		ActorsToIgnore, // 감지 하지 않을 대상 제외처리
		EDrawDebugTrace::ForDuration, //디버그용 트레이스 설정, ForDuration = 정해진 시간만큼 감지하다가 사라지는 트레이스 설정 
		OutHit, //결과 값을 받아줄 변수, 레퍼런스로 전달
		true); // 감지영역에서 나 자신을 제외하는 옵션
		//WorldContextObject 로 나 자신이라는 대상을 지정 

	if (bHit) //감지 ON
	{
		if (AActor* HitActor = OutHit.GetActor()) //감지된 액터를 가지고 옴
		{
			if (IDS1Interact* Interaction = Cast<IDS1Interact>(HitActor)) //가지고 온 액터가 만들어진 인터렉트라는 인터페이스를 구현하고 있는지 확인
			{
				Interaction->Interact(this); //참이면, Interact 함수를 호출(this)
			}
		}
	}
}

/** 전투상태 전환 */
void ADS1Character::ToggleCombat() //바인딩 된 입력을 처리하는 함수
{
	check(CombatComponent) // 널 체크
	check(StateComponent) // null check

	if (CombatComponent)
	{
		if (const ADS1Weapon* Weapon = CombatComponent->GetMainWeapon())
		{
			if (CanToggleCombat()) // 토글 가능 상태 체크
			{
				// 캐릭터의 상태를 Character_State_GeneralAction 상태로 변경
				StateComponent->SetState(DS1GameplayTags::Character_State_GeneralAction);
				
				/** 캐릭터의 전투 활성/비활성 상태에 따른 애니메이션 출력 설정, 
				 * 전투상태 = 캐릭터가 검을 손에 들고 있는 상태라면, 키 바인딩 하여 검을 등으로 집어넣는 애니메이션 출력
				 * 비전투상태 = 키 바인딩하여 캐릭터가 등에 있는 검을 손으로 가져오는 애니메이션 출력 */
				if (CombatComponent->IsCombatEnabled())
				{
					PlayAnimMontage(Weapon->GetMontageForTag(DS1GameplayTags::Character_Action_Unequip));
				}
				else
				{
					PlayAnimMontage(Weapon->GetMontageForTag(DS1GameplayTags::Character_Action_Equip));
				}
			}
		}
	}
}

