// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DS1Enemy.generated.h"

class UDS1StateComponent;
class UDS1AttributeComponent;

UCLASS()
class DS1_API ADS1Enemy : public ACharacter
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(VisibleAnywhere)
	UDS1AttributeComponent* AttributeComponent;
	
	UPROPERTY(VisibleAnywhere)
	UDS1StateComponent* StateComponent;
	
// Effect Section - 피격시 Effect & Sound
protected:
	UPROPERTY(EditAnywhere, Category="Effect")
	USoundCue* ImpactSound;
	
	UPROPERTY(EditAnywhere, Category="Effect")
	UParticleSystem* ImpactParticle;
	
//Montage Section - 피격 방향에 따른 애니메이션
protected:
	UPROPERTY(EditAnywhere, Category="Montage | HitReact") // 전방 피격
	UAnimMontage* HitReactAnimFront;
	
	UPROPERTY(EditAnywhere, Category="Montage | HitReact") // 후방 피격
	UAnimMontage* HitReactAnimBack;
	
	UPROPERTY(EditAnywhere, Category="Montage | HitReact") // 좌측 피격
	UAnimMontage* HitReactAnimLeft;
	
	UPROPERTY(EditAnywhere, Category="Montage | HitReact") // 우측 피격
 	UAnimMontage* HitReactAnimRight;
public:
	ADS1Enemy();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float Damage, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
public:
	virtual void OnDeath();

protected:
	void ImpactEffect(const FVector& Location);
	void HitReaction(const AActor* Attacker);
	UAnimMontage* GetHitReactAnimation(const AActor* Attacker) const;
};
