// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/UserDefinedEnum.h"
#include "OurEnums.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class ESpawnHelperType : uint8
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

enum class ECurrentLoadedLevel : uint8
{
	Level_1,
	Level_2,
	Hub,
	Non_Game
};

UENUM(BlueprintType)
enum class EDestinationLevel : uint8
{
	Level_1	UMETA(DisplayName = "Level 1"),
	Level_2 UMETA(DisplayName = "Level 2"),
	Hub		UMETA(DisplayName = "Hub"),
	None	UMETA(DisplayName = "None")
};

UENUM(BlueprintType)
enum class EPortalIndex : uint8
{
	HubLevel_1			UMETA(DisplayName = "Hub Level 1"),
	HubLevel_2			UMETA(DisplayName = "Hub Level 2"),
	Level_1Entrance		UMETA(DisplayName = "Level 1 Entrance"),
	Level_1Exit			UMETA(DisplayName = "Level 1 Exit"),
	Level_2Entrance		UMETA(DisplayName = "Level 2 Entrance"),
	Level_2Exit			UMETA(DisplayName = "Level 2 Exit"),
	Level_3Entrance		UMETA(DisplayName = "Level 3 Entrance"),
	Level_3Exit			UMETA(DisplayName = "Level 3 Exit")
};

UCLASS()
class WORLDSWITCHING_API UOurEnums : public UUserDefinedEnum
{
	GENERATED_BODY()
	
};
