// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NativeGameplayTags.h"

namespace DS1GameplayTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_State_Attacking);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_State_Rolling);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_State_GeneralAction);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_State_Death);
	
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_Action_Equip);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_Action_Unequip);
	
	//4가지 공격 종류, 정의(Define)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_Attack_Light);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_Attack_Running);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_Attack_Special);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_Attack_Heavy);
	
}
