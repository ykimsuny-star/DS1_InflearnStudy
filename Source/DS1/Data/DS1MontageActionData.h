// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "DS1MontageActionData.generated.h"


USTRUCT(BlueprintType)
struct FDS1MontageGroup
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UAnimMontage*> Animations;
};
/** 인프론 강사님: TMap에서 struct FDS1MontageGroup 만들어 넣은 이유는,
 * UPROPERTY로 에디터에서 편집할수 있게 만드려면, `Map`콘테이너 안에 콘테이너가 들어갈 수 없기 때문에 struct를 만들어서 넣어준다.
 * 잘못된예시->>>  TMap<FGameplayTag, TArray<UAnimMontage*>> 
 */
UCLASS()
class DS1_API UDS1MontageActionData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="Montage Groups")
	TMap<FGameplayTag, FDS1MontageGroup> MontageGroupMap;//
	
public:
	//TMap 콘테이너에서 애니메이션을 찾아 주는 함수, 키 값은 FGameplayTag 과 동일, 벨류는 TArray<UAnimMontage*> 안에 있는 애니메이션을 인덱스번호로 호출하기 위함
	UAnimMontage* GetMontageForTag(const FGameplayTag& GroupTag, const int32 Index);
	
};
