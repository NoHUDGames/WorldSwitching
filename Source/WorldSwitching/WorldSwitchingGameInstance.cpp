// Fill out your copyright notice in the Description page of Project Settings.

#include "WorldSwitchingGameInstance.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "SpawnHelper.h"
#include "Kismet/GameplayStatics.h"


UWorldSwitchingGameInstance::UWorldSwitchingGameInstance()
{
	NumberOfLevelsWithSpawnHelpers = 2;
}

void UWorldSwitchingGameInstance::BeginGame()
{
	LevelPickupParameters.Init(nullptr, NumberOfLevelsWithSpawnHelpers);
}

void UWorldSwitchingGameInstance::ManageLevelPickups()
{
	GetCurrentLevel();

	if (CurrentLoadedLevel == ECurrentLoadedLevel::Non_Game) return;

	if ((CurrentLoadedLevel == ECurrentLoadedLevel::Level_1 && bIsFirsTimeLoadingLevelOne) ||
		(CurrentLoadedLevel == ECurrentLoadedLevel::Level_2 && bIsFirsTimeLoadingLevelTwo))
	{
		GatherSpawnLocations();
		SpawnPickups();
		return;
	}

	if (CurrentLoadedLevel == ECurrentLoadedLevel::Hub) return;



	SpawnPickups();
}

void UWorldSwitchingGameInstance::GatherSpawnLocations()
{
	for (TActorIterator<ASpawnHelper> SpawnHelperItr(GetWorld()); SpawnHelperItr; ++SpawnHelperItr)
	{
		ASpawnHelper *SpawnHelper = *SpawnHelperItr;

		if (SpawnHelper->SpawnHelperType == ESpawnHelperType::Artifact)
		{
			
			if (CurrentLoadedLevel == ECurrentLoadedLevel::Level_1)
			{
				Level_1PickupParameters.ArtifactLocations.Add(SpawnHelper->GetActorLocation());
				Level_1PickupParameters.ArtifactPickedUp.Add(false);
			}
			else if (CurrentLoadedLevel == ECurrentLoadedLevel::Level_2)
			{
				Level_2PickupParameters.ArtifactLocations.Add(SpawnHelper->GetActorLocation());
				Level_2PickupParameters.ArtifactPickedUp.Add(false);
			}
			
			
		}
		else if (SpawnHelper->SpawnHelperType == ESpawnHelperType::Shield)
		{
			if (CurrentLoadedLevel == ECurrentLoadedLevel::Level_1)
			{
				Level_1PickupParameters.ShieldLocations.Add(SpawnHelper->GetActorLocation());
				Level_1PickupParameters.ShieldPickedUp.Add(false);
			}
			else if (CurrentLoadedLevel == ECurrentLoadedLevel::Level_2)
			{
				Level_2PickupParameters.ShieldLocations.Add(SpawnHelper->GetActorLocation());
				Level_2PickupParameters.ShieldPickedUp.Add(false);
			}
		}
	}
	
	if (CurrentLoadedLevel == ECurrentLoadedLevel::Level_1)
	{
		LevelPickupParameters[Level] = &Level_1PickupParameters;
		bIsFirsTimeLoadingLevelOne = false;
	}

	else if (CurrentLoadedLevel == ECurrentLoadedLevel::Level_2)
	{
		LevelPickupParameters[Level] = &Level_2PickupParameters;
		bIsFirsTimeLoadingLevelTwo = false;
	}

}


void UWorldSwitchingGameInstance::GetCurrentLevel()
{
	CurrentMapName = GetWorld()->GetMapName();

	if (CurrentMapName.Contains("Level_1")) CurrentLoadedLevel = ECurrentLoadedLevel::Level_1;
	else if (CurrentMapName.Contains("Level_2")) CurrentLoadedLevel = ECurrentLoadedLevel::Level_2;
	else if (CurrentMapName.Contains("Hub")) CurrentLoadedLevel = ECurrentLoadedLevel::Hub;
	else CurrentLoadedLevel = ECurrentLoadedLevel::Non_Game;

	int Level = static_cast<int>(CurrentLoadedLevel);
}


void UWorldSwitchingGameInstance::SpawnPickups()
{
	UWorld* World = GetWorld();

		int NumberOfArtifactsToSpawn = LevelPickupParameters[Level]->ArtifactLocations.Num();

		for (int i = 0; i < NumberOfArtifactsToSpawn; ++i)
		{
			if (!LevelPickupParameters[Level]->ArtifactPickedUp[i])
			{
				AArtifacts* Artifact = World->SpawnActor<AArtifacts>(ArtifactToSpawn, LevelPickupParameters[Level]->ArtifactLocations[i], FRotator(0));
				Artifact->SetArrayIndex(i);
			}
		}
		int NumberOfShieldsToSpawn = LevelPickupParameters[Level]->ShieldLocations.Num();

		for (int i = 0; i < NumberOfShieldsToSpawn; ++i)
		{
			if (!LevelPickupParameters[Level]->ShieldPickedUp[i])
			{
				AS_PickupShield* Shield = World->SpawnActor<AS_PickupShield>(ShieldToSpawn, LevelPickupParameters[Level]->ShieldLocations[i], FRotator(0));
				Shield->SetArrayIndex(i);
			}
		}
}

void UWorldSwitchingGameInstance::RegisterPickUp(int index, AActor* OtherActor)
{
	if (Cast<AArtifacts>(OtherActor))
	LevelPickupParameters[Level]->ArtifactPickedUp[index] = true;

	else if (Cast<AS_PickupShield>(OtherActor))
	LevelPickupParameters[Level]->ShieldPickedUp[index] = true;

}


void UWorldSwitchingGameInstance::SetPlayerHealth(int PlayerHealth)
{
	GI_PlayerHealth = PlayerHealth;
}

void UWorldSwitchingGameInstance::SetPlayerArtifacts(int PlayerArtifacts)
{
	GI_PlayerArtifacts = PlayerArtifacts;
}

int UWorldSwitchingGameInstance::GetPlayerHealth()
{
	return GI_PlayerHealth;
}

int UWorldSwitchingGameInstance::GetPlayerArtifacts()
{
	return GI_PlayerArtifacts;
}

bool UWorldSwitchingGameInstance::GetbIsFirstTimeStartingGame()
{
	return bIsFirstTimeStartingGame;
}

void UWorldSwitchingGameInstance::SetbIsFirstTimeStartingGame(bool State)
{
	bIsFirstTimeStartingGame = State;
}

int UWorldSwitchingGameInstance::GetAltarArtifacts()
{
	return GI_AltarArtifacts;
}

void UWorldSwitchingGameInstance::SetAltarArtifacts(int Number)
{
	GI_AltarArtifacts = Number;
}