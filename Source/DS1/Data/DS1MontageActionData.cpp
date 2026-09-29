// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/DS1MontageActionData.h"

UAnimMontage* UDS1MontageActionData::GetMontageForTag(const FGameplayTag& GroupTag, const int32 Index)
{
	//매개변수 Key로 받은 FGameplayTag를 확인 검사
	if (MontageGroupMap.Contains(GroupTag))
	{
		//key가 정상이면, Value를 가져옴
		const FDS1MontageGroup& MontageGroup = MontageGroupMap[GroupTag];
		
		//nullptr 검사후, 배열의 인덱스값으로 애니메이션을 리턴해줌
		if (MontageGroup.Animations.Num() > 0 && MontageGroup.Animations.Num() > Index)
		{
			return MontageGroup.Animations[Index];
		}
	}
	
	return nullptr;
}
