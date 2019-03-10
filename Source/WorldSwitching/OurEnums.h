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

UENUM(BlueprintType)
enum class EArtifactBelongsToLevel : uint8
{	
	Level_1		UMETA(DisplayName ="Level 1"),
	Level_2		UMETA(DisplayName ="Level 2")
};

enum class ECurrentLevelLoaded : uint8
{
	Level_1,
	Level_2,
	Hub
};

UENUM(BlueprintType)
enum class EDestinationLevel : uint8
{
	Level_1,
	Level_2,
	Hub
};

UCLASS()
class WORLDSWITCHING_API UOurEnums : public UUserDefinedEnum
{
	GENERATED_BODY()
	
};
