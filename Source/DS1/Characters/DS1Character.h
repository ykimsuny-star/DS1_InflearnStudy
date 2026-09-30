// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Character.h"
#include "DS1Character.generated.h"

class UDS1CombatComponent;
class UDS1StateComponent;
class UDS1PlayerHUDWidget;
struct FInputActionValue;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UDS1AttributeComponent;

UCLASS()
class DS1_API ADS1Character : public ACharacter
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

// Input Section
private:
	UPROPERTY(EditAnywhere, Category = "Input") // 맵핑 콘텍스트
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, Category = "Input") // 캐릭터 이동, 움직임 키
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input") // 화면 이동, 마우스 키
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category = "Input") // 달리기 & 구르기 키
	UInputAction* SprintRollingAction;
	
	UPROPERTY(EditAnywhere, Category = "Input") // 다른 물체랑 상호작용 하는 키
	UInputAction* InteractAction;

	/*[기존]
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* ToggleCombatAction;
	*/
	/* 전투 활성화/비활성화 토글 */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* ToggleCombatAction;
	
	/** Attack */
	//MEMO: 3가지 공격을 기본적으로 마우스(좌)버튼으로 사용, (짧은 좌클릭 Light Attack, 질주 도중 짧은 좌클릭 Running Attack, 길게 좌클릭 Special Attack)
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* AttackAction;
	
	/** Heavy Attack */
	//MEMO: Shift+(짧)좌클릭
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* HeavyAttackAction;
	
private:
	/** 캐릭터의 각종 스탯 관리 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UDS1AttributeComponent* AttributeComponent;
	
	/* 캐릭터의 상태 관리 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UDS1StateComponent* StateComponent;
	
	/** 무기, 전투 관리 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UDS1CombatComponent* CombatComponent;
	
	
// UI Section
protected:
	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<UUserWidget> PlayerHUDWidgetClass;

	UPROPERTY()
	UDS1PlayerHUDWidget* PlayerHUDWidget;

protected:
	/** 질주 속도 */
	UPROPERTY(EditAnywhere, Category="Sprinting")
	float SprintingSpeed = 750.f;

	/** 일반 속도 */
	UPROPERTY(EditAnywhere, Category = "Sprinting")
	float NormalSpeed = 500.f;
	
	/** 질주 중인지? */
	UPROPERTY(VisibleAnywhere, Category="Sprinting")
	bool bSprinting = false;

//Combo Section
protected:
	/** 콤보 시퀀스 진행중 */
	//MEMO: 전체 콤보 시퀀스가 진행중인지, 끝났는지 확인하는 변수
	bool bComboSequenceRunning = false;
	
	/** 콤보 입력 가능? */
	bool bCanComboInput = false;
	
	/** 콤보 카운터 */
	//MEMO: 몇번째 콤보를 실행해야되는지 관리해주는 변수
	int32 ComboCounter = 0;
	
	/** 콤보 입력 여부 */
	bool bSavedComboInput = false;
	
	/** 콤보 리셋 타이머 핸들 */
	//MEMO: 콤보 입력이 들어오지 않으면 리셋할 수 있는 타이머 변수
	FTimerHandle ComboResetTimerHandle;
	
//Montage Section	
protected:
	UPROPERTY(EditAnywhere, Category="Montage")
	UAnimMontage* RollingMontage;

public:
	ADS1Character();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyControllerChanged() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	FORCEINLINE UDS1StateComponent* GetStateComponent() const { return StateComponent; };

protected:
	/** 캐릭터가 이동중인지 체크 */
	bool IsMoving() const;
	/** 토글전환 가능한 상태인지? */
	bool CanToggleCombat() const;
	/** 스프린터 상태인지? Getter 형식 */
	FORCEINLINE bool IsSprinting() const { return bSprinting; }

	/** 이동 */
	void Move(const FInputActionValue& Values);
	/** 카메라 방향 */
	void Look(const FInputActionValue& Values);
	/** 질주 */
	void Sprinting();
	/** 질주 중단 */
	void StopSprint();
	/** 구르기 */
	void Rolling();
	/** 인터렉션(상호작용) */
	void Interact();
	/** 전투상태 전환 */
	void ToggleCombat();
	void AutoToggleCombat(); //MEMO: 무기를 등에 차고있는 비전투 상태일때, 바로 공격버튼을 누르게 되면 먼저 무기를 손에 들수 있도록 하는 함수
	
	/** Attack */
	void Attack();
	void SpecialAttack();
	void HeavyAttack();
	
protected:
	/** 현재 상태에서 수해 가능한 일반공격 */
	FGameplayTag GetAttackPerform() const;
	
	/** 공격 가능 조건 체크 */
	bool CanPerformAttack(const FGameplayTag& AttackTypeTag) const;
	/** 공격 실행 */
	void DoAttack(const FGameplayTag& AttackTypeTag);
	/** 콤보 실행 */
	void ExecuteComboAttack(const FGameplayTag& AttackTypeTag);
	/** 콤보 초기화 */
	void ResetCombo();
	
//Combo AnimNotify Section
public:
	void EnableComboWindow();
	void DisableComboWindow();
	void AttackFinished(const float ComboResetDelay);
	
};
