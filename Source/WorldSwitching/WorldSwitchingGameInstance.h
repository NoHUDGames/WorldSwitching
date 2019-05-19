// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OurEnums.h"
#include "Artifacts.h"
#include "S_PickupShield.h"
#include "SpawnHelper.h"
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

	//------ START of system that keeps track of which pickups have been picked up between levels-------//
	//Its essentially an Array (LevelPickupParameters), of structs (Level_#_PickupParameters), 
	//containing 4 arrays (Location and bool PickedUpStates for each pickup type and level).
	//SpawnHelpers only provide location for where to spawn PickUps, otherwise there is no relationship between them. 
	//Pickups are matched to bool PickedUp arrays based on index given to them at spawn time. 
	//Enter a level, get some pickups, exit and reenter level, and only pickups that have not already been taken are allowed to spawn again. 
	//Only Pickups spawned in this way can be kept track of.

	UPROPERTY(EditAnywhere)
	int NumberOfLevelsWithSpawnHelpers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool FinishedTutorial{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool FinishedLevel2{ false };

	TArray<FLevelPickupParameters*> LevelPickupParameters;
	FLevelPickupParameters Level_1_PickupParameters;
	FLevelPickupParameters Level_2_PickupParameters;

	int NumberOfArtifactsLevel1{0};
	int NumberOfArtifactsLevel2{0};
	int NumberOfShieldsLevel1{0};
	int NumberOfShieldsLevel2{0};


	void ProcessPickupParameters();
	void CountSpawnHelpersInLevel();

	//Initialize all arrays in Level PickupParameters structs in one go, so we don't add one by one later to arrays inside struct,
	//Minimizing relocation of struct with 4 arrays
	void InitializePickupParameters();
	void AssignPickupParameters();

	void ManageLevelPickups();
	void GetCurrentLevel();
	void SpawnPickups();
	void RegisterPickedUp(int index, AActor* OtherActor);
	//------ END of system that keeps track of which pickups have been picked up between levels-------//

	//Locking of abilities until player has reached correct place in tutorial
	UPROPERTY(BlueprintReadWrite)
	bool bIsSensingLocked = true;

	UPROPERTY(BlueprintReadWrite)
	bool bIsWorldChangingLocked = true;

	UPROPERTY(BlueprintReadWrite)
	bool bIsDashingLocked = true;


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

	

	bool HubPortalToLevel2 = false;
	

public:


	UPROPERTY(EditAnywhere)
	TSubclassOf<AArtifacts> ArtifactToSpawn;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AS_PickupShield> ShieldToSpawn;


	UPROPERTY(BlueprintReadOnly)
	bool bIsFirstTimeStartingGame = true;

	UPROPERTY(BlueprintReadWrite)
	bool bStartWithIntroDialogue = true;



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

	UFUNCTION(BlueprintCallable)
	void ResetGameVariables();

};
