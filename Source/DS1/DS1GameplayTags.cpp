// Fill out your copyright notice in the Description page of Project Settings.


#include "DS1GameplayTags.h"

namespace DS1GameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG(Character_State_Attacking, "Character.State.Attacking");
	UE_DEFINE_GAMEPLAY_TAG(Character_State_Rolling, "Character.State.Rolling");
	UE_DEFINE_GAMEPLAY_TAG(Character_State_GeneralAction, "Character.State.GeneralAction");
	UE_DEFINE_GAMEPLAY_TAG(Character_State_Death, "Character.State.Death");
	
	UE_DEFINE_GAMEPLAY_TAG(Character_Action_Equip, "Character.Action.Equip");
	UE_DEFINE_GAMEPLAY_TAG(Character_Action_Unequip, "Character.Action.Unequip");
	
	//4가지 공격 종류, 정의(Define)
	UE_DEFINE_GAMEPLAY_TAG(Character_Attack_Light, "Character.Attack.Light");
	UE_DEFINE_GAMEPLAY_TAG(Character_Attack_Running, "Character.Attack.Running");
	UE_DEFINE_GAMEPLAY_TAG(Character_Attack_Special, "Character.Attack.Spacial");
	UE_DEFINE_GAMEPLAY_TAG(Character_Attack_Heavy, "Character.Attack.Heavy");
		
	
}
