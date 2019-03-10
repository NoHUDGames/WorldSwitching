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

private:

	ECurrentLevelLoaded CurrentLevelLoaded;
	FString CurrentMapName;

	FString PrototypeLevelName;
	FString LevelOneName;
	FString LevelTwoName;



	uint8 AltarHoldingArtifacts = 0;

	//TArray<TArray<AActor*>> AllArtifactsState;

	TArray<AActor*> Level_1Artifacts;
	TArray<AActor*> Level_2Artifacts;

	bool bIsFirsTimeLoadingLevelOne = true;
	bool bIsFirsTimeLoadingLevelTwo = true;

public:


	void ManageLevelArtifactsState();
	void GetCurrentLevel();
	void ManageFirstTimeLevelLoad();
};
