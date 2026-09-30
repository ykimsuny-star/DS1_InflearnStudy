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
		
		// Combat 상태로 자동 전환.
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ThisClass::AutoToggleCombat);
		// 일반 공격
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Canceled, this, &ThisClass::Attack); //짧게 좌클, Canceled 이벤트;
		// 특수 공격
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ThisClass::SpecialAttack); //길게 좌클, Triggered 이벤트;
		// HeavyAttack
		EnhancedInputComponent->BindAction(HeavyAttackAction, ETriggerEvent::Started, this, &ThisClass::HeavyAttack); //별도의 인풋액션으로 Shift+좌클 로 발동;
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
		
		bSprinting = true; // 질주 상태 ON;
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
	bSprinting = false; // 질주 상태 OFF;
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

void ADS1Character::AutoToggleCombat()
{
	if (CombatComponent)
	{
		if (!CombatComponent->IsCombatEnabled()) //CombatEnabled 가 false면 (전투상태가 아니면),  
		{
			ToggleCombat(); // 전투 상태 전환 ToggleCombat 함수 호출 (검 꺼내기)
		}
	}
}

void ADS1Character::Attack()
{
	const FGameplayTag AttackTypeTag = GetAttackPerform(); //GetAttackPerform 함수를 통해 상태 Tag 변경

	if (CanPerformAttack(AttackTypeTag))
	{
		ExecuteComboAttack(AttackTypeTag);
	}
}

void ADS1Character::SpecialAttack()
{
	const FGameplayTag AttackTypeTag = DS1GameplayTags::Character_Attack_Special;

	if (CanPerformAttack(AttackTypeTag))
	{
		ExecuteComboAttack(AttackTypeTag);
	}
}

void ADS1Character::HeavyAttack()
{
	AutoToggleCombat();
	
	const FGameplayTag AttackTypeTag = DS1GameplayTags::Character_Attack_Heavy;

	if (CanPerformAttack(AttackTypeTag))
	{
		ExecuteComboAttack(AttackTypeTag);
	}
}

FGameplayTag ADS1Character::GetAttackPerform() const
{
	if (IsSprinting()) // IsSprinting 함수를 통해 질주 중인지 확인하고, 질주 상태라면?
	{
		return DS1GameplayTags::Character_Attack_Running; // 질주 공격 상태를 실행
	}
	return DS1GameplayTags::Character_Attack_Light; // 일반 공격 상태를 실행
}

bool ADS1Character::CanPerformAttack(const FGameplayTag& AttackTypeTag) const
{
	/* 컴포넌트들 널체크 */
	check(StateComponent)
	check(CombatComponent)
	check(AttributeComponent)

	if (IsValid(CombatComponent->GetMainWeapon()) == false) // 무기를 들고 있는지 체크
	{
		return false; //들고 있지 않으면 false 값 반환(함수 탈출)
	}
	
	/* 특정상태의 경우에도 공격을 할 수 없도록 설정 */
	FGameplayTagContainer CheckTags; 
	CheckTags.AddTag(DS1GameplayTags::Character_State_Rolling); //구르기 도중 공격 X
	CheckTags.AddTag(DS1GameplayTags::Character_State_GeneralAction); // 다른 액션 도중에 공격 X
	
	const float StaminaCost = CombatComponent->GetMainWeapon()->GetStaminaCost(AttackTypeTag); // 현재 공격타입에 맞는 필요 스테미나 코스트 값을 저장
	
	return StateComponent->IsCurrentStateEqualToAny(CheckTags) == false // 현재 상태 구르기, 다른 액션 도중 상태가 아니고,
		&& CombatComponent->IsCombatEnabled() //전투 상태이여야 하며, (무기를 손에 든 상태)
		&& AttributeComponent->CheckHasEnoughStamina(StaminaCost); //필요 스테미나 만큼의 스테미나를 가지고 있는지 체크
	//MEMO: 위 조건들을 모두 충족한 상태라면 현재 함수를 (true) 활성화 (반환) 한다.
}

void ADS1Character::DoAttack(const FGameplayTag& AttackTypeTag)
{
	/* 컴포넌트 체크 */
	check(StateComponent)
	check(AttributeComponent)
	check(CombatComponent)

	if (const ADS1Weapon* Weapon = CombatComponent->GetMainWeapon()) //무기 장착한 상태인지 체크
	{
		StateComponent->SetState(DS1GameplayTags::Character_State_Attacking); // StateComponent에 현재 상태를 `공격중상태`로 세팅
		StateComponent->ToggleMovementInput(false); //공격중 이동이 불가능 하도록 이동 입력을 차단함
		CombatComponent->SetLastAttackType(AttackTypeTag); //CombatComponent에도 현재 `상태 Tag`를 세팅
		
		AttributeComponent->ToggleStaminaRegeneration(false); //스테미나 재충전 기능 OFF
		
		UAnimMontage* Montage = Weapon->GetMontageForTag(AttackTypeTag, ComboCounter); //콤보 카운트를 키값으로 맞는 애님 몽타주를 찾음
		if (!Montage)// 콤보 카운트에 맞는 애니메이션을 찾지 못할경우
		{
			//콤보 한계 도달.
			ComboCounter = 0; //콤보 카운트를 0으로 초기화
			Montage = Weapon->GetMontageForTag(AttackTypeTag, ComboCounter); //다시 애님 몽타주를 찾음
		}
		
		PlayAnimMontage(Montage); // 찾은 애니메이션 몽타주를 실행
		
		const float StaminaCost = Weapon->GetStaminaCost(AttackTypeTag); // 현재 무기 사용에 필요한 코스트를 찾아서
		AttributeComponent->DecreaseStamina(StaminaCost); //스테미나를 소모시킴
		AttributeComponent->ToggleStaminaRegeneration(true, 1.5f); // 다시 스테미나 재충전을 활성화
	}
}

void ADS1Character::ExecuteComboAttack(const FGameplayTag& AttackTypeTag)
{
	if (StateComponent->GetCurrentState() != DS1GameplayTags::Character_State_Attacking) // 캐릭터가 현재 `공격중상태` 가 아니라면
	{
		if (bComboSequenceRunning && bCanComboInput == false) //ComboResetDelay 시간 중에 입력을 콤보를 입력했다면
		{
			//애니메이션은 끝났지만 아직 콤보 시퀀스가 유효할 때 - 추가 입력 기회
			ComboCounter++;
			UE_LOG(LogTemp, Warning, TEXT("Additional input : Combo Counter = %d"), ComboCounter);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT(">>> ComboSequence Started <<<"));
			ResetCombo(); // 콤보 관련 변수 초기화 후
			bComboSequenceRunning = true; // 콤보시퀀스실행변수 활성
		}
		
		DoAttack(AttackTypeTag); // 현재 콤보 수에 맞는 몽타주를 실행하는 함수
		GetWorld()->GetTimerManager().ClearTimer(ComboResetTimerHandle);
	}
	else if (bCanComboInput)
	{
		//콤보 윈도우가 열려 있을 때 - 최적의 타이밍
		bSavedComboInput = true;
	}
}

/* 콤보 관련 변수들을 모두 리셋 */
void ADS1Character::ResetCombo()
{
	UE_LOG(LogTemp, Warning, TEXT("Combo Reset"));
	
	bComboSequenceRunning = false;
	bCanComboInput = false;
	bSavedComboInput = false;
	ComboCounter = 0;
}

//Memo: 에디터에서 몽타주에 배치한 AnimNotify가 실행중일때, 
void ADS1Character::EnableComboWindow()
{
	bCanComboInput = true; //`콤보 입력 가능상태` 변수 활성화
	UE_LOG(LogTemp, Warning, TEXT("Combo Window Opened: Combo Counter = %d"), ComboCounter);
}

//Memo: 에디터에서 몽타주에 배치한 AnimNotify가 끝나면,
void ADS1Character::DisableComboWindow()
{
	check(CombatComponent)
	 
	bCanComboInput = false; //`콤보 입력 가능상태` 변수 비활성화

	if (bSavedComboInput) //`콤보 입력 가능상태` 일때, 콤보를 입력했다면 
	{
		bSavedComboInput = false; //입력하지 않은 상태로 바꾸고
		ComboCounter++; //콤보 카운터를 1씩 증가
		UE_LOG(LogTemp, Warning, TEXT("Combo Window Closed: Advancing to next combo = %d"), ComboCounter);
		DoAttack(CombatComponent->GetLastAttackType()); //증가한 카운터 수의 콤보를 이어서 실행
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Combo Window Closed: No input received")); 
	}
}

//Memo: AnimNotify 실행중인동안 입력하지 못해도 끝부분에 입력 기회를 주고, 공격이 끝나면 다시 캐릭터가 이동할 수 있는 상태로 전환 (몽타주 끝부분에 배치)
void ADS1Character::AttackFinished(const float ComboResetDelay)
{
	UE_LOG(LogTemp, Warning, TEXT("AttackFinished"));
	if (StateComponent)
	{
		StateComponent->ToggleMovementInput(true); //캐릭터가 이동이 가능하도록 입력을 활성화
	}
	// ComboResetDelay 후에 콤보 시퀀스 종료
	GetWorld()->GetTimerManager().SetTimer(ComboResetTimerHandle, this, &ThisClass::ResetCombo, ComboResetDelay, false);
	//Memo: float 매개변수 ComboResetDelay 만큼 시간이 지난 후에 콤보를 리셋 해서, 처음부터 다시 콤보를 시작하게 설정
}

