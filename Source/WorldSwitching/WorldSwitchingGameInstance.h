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


USTRUCT()
struct FLevelPickupParameters
{
	GENERATED_BODY()
	TArray<FVector> ArtifactLocations;
	TArray<bool> ArtifactPickedUp;

	TArray<FVector> ShieldLocations;
	TArray<bool> ShieldPickedUp;
};


UCLASS()
class WORLDSWITCHING_API UWorldSwitchingGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	UWorldSwitchingGameInstance();

	UPROPERTY(EditAnywhere)
	int NumberOfLevelsWithSpawnHelpers;

	TArray<FLevelPickupParameters*> LevelPickupParameters;
	FLevelPickupParameters Level_1PickupParameters;
	FLevelPickupParameters Level_2PickupParameters;
	


	//Level 1 Pickups
	//TArray<FVector> Level_1ArtifactLocations;
	//TArray<bool> Level_1ArtifactPickedUp;

	//TArray<FVector> Level_1ShieldLocations;
	//TArray<bool> Level_1ShieldPickedUp;



	//Level 2 Pickups
	//TArray<FVector> Level_2ArtifactLocations;
	//TArray<bool> Level_2ArtifactPickedUp;

	//TArray<FVector> Level_2ShieldLocations;
	//TArray<bool> Level_2ShieldPickedUp;


private:

	ECurrentLoadedLevel CurrentLoadedLevel;
	int Level;
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
	void SpawnPickups();
	void RegisterPickUp(int index, AActor* OtherActor);

	void SetPlayerHealth(int PlayerHealth);
	void SetPlayerArtifacts(int PlayerArtifacts);

	int GetPlayerHealth();
	int GetPlayerArtifacts();

	bool GetbIsFirstTimeStartingGame();
	void SetbIsFirstTimeStartingGame(bool State);

	void BeginGame();

};
