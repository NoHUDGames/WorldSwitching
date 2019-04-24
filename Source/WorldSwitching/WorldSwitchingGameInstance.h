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
	FLevelPickupParameters Level_1_PickupParameters;
	FLevelPickupParameters Level_2_PickupParameters;


	int NumberOfArtifactsLevel1{0};
	int NumberOfArtifactsLevel2{0};
	int NumberOfShieldsLevel1{0};
	int NumberOfShieldsLevel2{0};

private:

	ECurrentLoadedLevel CurrentLoadedLevel;
	int Level;
	FString CurrentMapName;


	//GameInstance_

	int GI_PlayerHealth = 3;
	int GI_PlayerArtifacts = 0;
	int GI_AltarArtifacts = 0;

	bool bIsFirsTimeLoadingLevelOne = true;
	bool bIsFirsTimeLoadingLevelTwo = true;
	bool bIsFirstTimeStartingGame = true;

	bool HubPortalToLevel2 = false;
	

public:

	bool bIsFirstTimeLoadingGame = true;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AArtifacts> ArtifactToSpawn;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AS_PickupShield> ShieldToSpawn;

	UFUNCTION(BlueprintImplementableEvent)
		void AttachPickupToSpawnHelper(AActor* Child, AActor* Parent);


	void ProcessPickupParameters();
	void CountSpawnHelpersInLevel();

	//Initialize first, so we don't add one by one later to arrays inside struct
	void InitializePickupParameters();
	void AssignPickupParameters();


	void ManageLevelPickups();
	void GetCurrentLevel();
	void SpawnPickups();
	void RegisterPickUp(int index, AActor* OtherActor);

	void SetPlayerHealth(int PlayerHealth);
	void SetPlayerArtifacts(int PlayerArtifacts);
	void SetArtifactPickedUp(int index);

	int GetPlayerHealth();
	int GetPlayerArtifacts();

	UFUNCTION(BlueprintCallable)
	int GetAltarArtifacts();

	void SetAltarArtifacts(int Number);

	bool GetbIsFirstTimeStartingGame();
	void SetbIsFirstTimeStartingGame(bool State);

	bool GetHubPortalLevel2Open() { return HubPortalToLevel2; }
	void SetHubPortalLevel2Open(bool State) { HubPortalToLevel2 = State; }

	void BeginGame();

};
