// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Artifacts.h"
#include "OurEnums.h"
#include "WorldSwitchingGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class WORLDSWITCHING_API UWorldSwitchingGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	UWorldSwitchingGameInstance();

	//_Start contains all location variables on BeginPlay
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVector> Level_1ArtifactLocations_Start;

	//_Continuing copies _Start at BeginPlay. Elements are removed from _Continuing as Artifacts are picked up.
	TArray<FVector> Level_1ArtifactLocations_Continuing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVector> Level_2ArtifactLocations_Start;

	TArray<FVector> Level_2ArtifactLocations_Continuing;

private:

	ECurrentLevelLoaded CurrentLoadedLevel;
	FString CurrentMapName;

	uint8 AltarHoldingArtifacts = 0;

	bool bIsFirsTimeLoadingLevelOne = true;
	bool bIsFirsTimeLoadingLevelTwo = true;

public:

	UPROPERTY(EditAnywhere)
		TSubclassOf<AArtifacts> ArtifactToSpawn;

	void ManageLevelArtifacts();
	void GetCurrentLevel();
	void ManageFirstTimeLevelLoad();
	void SpawnArtifacts();
	void RemovePickedUpArtifact(int index);
};
