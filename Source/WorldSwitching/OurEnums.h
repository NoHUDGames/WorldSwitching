// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/UserDefinedEnum.h"
#include "OurEnums.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EPickupTypes : uint8
{
	Artifact	UMETA(DisplayName="Artifact"),
	Shield		UMETA(DisplayName="Shield")
};

UCLASS()
class WORLDSWITCHING_API UOurEnums : public UUserDefinedEnum
{
	GENERATED_BODY()
	
};
