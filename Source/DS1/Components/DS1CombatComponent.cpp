// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/DS1CombatComponent.h"

UDS1CombatComponent::UDS1CombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UDS1CombatComponent::BeginPlay()
{
	Super::BeginPlay();

	
}


void UDS1CombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UDS1CombatComponent::SetWeapon(ADS1Weapon* NewWeapon)
{
	MainWeapon = NewWeapon;
}

