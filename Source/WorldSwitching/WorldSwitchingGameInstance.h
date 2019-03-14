// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OurEnums.h"
#include "Artifacts.h"
#include "S_PickupShield.h"
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


	
	//Level 1 Pickups
	TArray<FVector> Level_1ArtifactLocations;
	TArray<bool> Level_1ArtifactPickedUp;

	TArray<FVector> Level_1ShieldLocations;
	TArray<bool> Level_1ShieldPickedUp;



	//Level 2 Pickups
	TArray<FVector> Level_2ArtifactLocations;
	TArray<bool> Level_2ArtifactPickedUp;

	TArray<FVector> Level_2ShieldLocations;
	TArray<bool> Level_2ShieldPickedUp;


private:

	ECurrentLoadedLevel CurrentLoadedLevel;
	FString CurrentMapName;


	//GameInstance_

	int GI_PlayerHealth = 3;
	int GI_PlayerArtifacts = 0;

	bool bIsFirsTimeLoadingLevelOne = true;
	bool bIsFirsTimeLoadingLevelTwo = true;
	bool bIsFirstTimeStartingGame = true;

	

public:

	bool bIsFirstTimeLoadingGame = true;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AArtifacts> ArtifactToSpawn;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AS_PickupShield> ShieldToSpawn;



	void GatherSpawnLocations();

	void ManageLevelPickups();
	void GetCurrentLevel();
	void SetIsFirstTimeLoadingLevel();
	void SpawnPickups();
	void RegisterPickedUpArtifact(int index);
	void RegisterPickedUpShield(int index);

	void FetchPlayerHealth(int PlayerHealth);
	void FetchPlayerArtifacts(int PlayerArtifacts);

	int FeedPlayerHealth();
	int FeedPlayerArtifacts();

	bool GetbIsFirstTimeStartingGame();
	void SetbIsFirstTimeStartingGame(bool State);

	void BeginGame();

};
