// Fill out your copyright notice in the Description page of Project Settings.

#include "WorldSwitchingGameInstance.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "SpawnHelper.h"
#include "PS_Portal.h"
#include "Kismet/GameplayStatics.h"


UWorldSwitchingGameInstance::UWorldSwitchingGameInstance()
{
	NumberOfLevelsWithSpawnHelpers = 2;
}

void UWorldSwitchingGameInstance::BeginGame()
{
	LevelPickupParameters.Init(nullptr, NumberOfLevelsWithSpawnHelpers);
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

void UWorldSwitchingGameInstance::ManageLevelPickups()
{
	GetCurrentLevel();

	if (CurrentLoadedLevel == ECurrentLoadedLevel::Non_Game ||
		CurrentLoadedLevel == ECurrentLoadedLevel::Hub) return;

	if ((CurrentLoadedLevel == ECurrentLoadedLevel::Level_1 && bIsFirsTimeLoadingLevelOne) ||
		(CurrentLoadedLevel == ECurrentLoadedLevel::Level_2 && bIsFirsTimeLoadingLevelTwo))
	{
		ProcessPickupParameters();
		SpawnPickups();
		return;
	}

	SpawnPickups();
}

void UWorldSwitchingGameInstance::ProcessPickupParameters()
{
	//countsPicku
	CountSpawnHelpersInLevel();

	//Initialize PickupParameters in Arrays in Struct, using above counts, so we dont add one by one later to arrays, 
	//avoiding reallocation of whole struct with many arrays
	InitializePickupParameters();

	AssignPickupParameters();
}

void UWorldSwitchingGameInstance::CountSpawnHelpersInLevel()
{
	if (CurrentLoadedLevel == ECurrentLoadedLevel::Level_1)
	{
		for (TActorIterator<ASpawnHelper> SpawnHelperItr(GetWorld()); SpawnHelperItr; ++SpawnHelperItr)
		{
			ASpawnHelper *SpawnHelper = *SpawnHelperItr;

			if (SpawnHelper->SpawnHelperType == ESpawnHelperType::Artifact)
			{
				NumberOfArtifactsLevel1++;
			}

			else if (SpawnHelper->SpawnHelperType == ESpawnHelperType::Shield)
			{
				NumberOfShieldsLevel1++;
			}
		}
		UE_LOG(LogTemp,Warning, TEXT("NumberOfArtifacts in level: %i"), NumberOfArtifactsLevel1)
	}

	if (CurrentLoadedLevel == ECurrentLoadedLevel::Level_2)
	{
		for (TActorIterator<ASpawnHelper> SpawnHelperItr(GetWorld()); SpawnHelperItr; ++SpawnHelperItr)
		{
			ASpawnHelper *SpawnHelper = *SpawnHelperItr;

			if (SpawnHelper->SpawnHelperType == ESpawnHelperType::Artifact)
			{
				NumberOfArtifactsLevel2++;
			}

			else if (SpawnHelper->SpawnHelperType == ESpawnHelperType::Shield)
			{
				NumberOfShieldsLevel2++;
			}
		}
	}
}

void UWorldSwitchingGameInstance::InitializePickupParameters()
{
	if (CurrentLoadedLevel == ECurrentLoadedLevel::Level_1)
	{
		Level_1_PickupParameters.ArtifactLocations.Init(FVector(0, 0, 0), NumberOfArtifactsLevel1);
		Level_1_PickupParameters.ArtifactPickedUp.Init(false, NumberOfArtifactsLevel1);

		Level_1_PickupParameters.ShieldLocations.Init(FVector(0, 0, 0), NumberOfShieldsLevel1);
		Level_1_PickupParameters.ShieldPickedUp.Init(false, NumberOfShieldsLevel1);

		LevelPickupParameters[Level] = &Level_1_PickupParameters;
		bIsFirsTimeLoadingLevelOne = false;
	}

	if (CurrentLoadedLevel == ECurrentLoadedLevel::Level_2)
	{
		Level_2_PickupParameters.ArtifactLocations.Init(FVector(0, 0, 0), NumberOfArtifactsLevel2);
		Level_2_PickupParameters.ArtifactPickedUp.Init(false, NumberOfArtifactsLevel2);

		Level_2_PickupParameters.ShieldLocations.Init(FVector(0, 0, 0), NumberOfShieldsLevel2);
		Level_2_PickupParameters.ShieldPickedUp.Init(false, NumberOfShieldsLevel2);

		LevelPickupParameters[Level] = &Level_2_PickupParameters;
		bIsFirsTimeLoadingLevelTwo = false;
	}

}

void UWorldSwitchingGameInstance::AssignPickupParameters()
{
	if (CurrentLoadedLevel == ECurrentLoadedLevel::Level_1)
	{
		int ArtifactIndex{ 0 };
		int ShieldIndex{ 0 };

		for (TActorIterator<ASpawnHelper> SpawnHelperItr(GetWorld()); SpawnHelperItr; ++SpawnHelperItr)
		{
			ASpawnHelper *SpawnHelper = *SpawnHelperItr;

			if (SpawnHelper->SpawnHelperType == ESpawnHelperType::Artifact)
			{
				Level_1_PickupParameters.ArtifactLocations[ArtifactIndex] = SpawnHelper->GetActorLocation();
				ArtifactIndex++;
			}

			else if (SpawnHelper->SpawnHelperType == ESpawnHelperType::Shield)
			{
				Level_1_PickupParameters.ShieldLocations[ShieldIndex] = SpawnHelper->GetActorLocation();

				ShieldIndex++;
			}
		}
	}

	else if (CurrentLoadedLevel == ECurrentLoadedLevel::Level_2)
	{
		int ArtifactIndex{ 0 };
		int ShieldIndex{ 0 };

		for (TActorIterator<ASpawnHelper> SpawnHelperItr(GetWorld()); SpawnHelperItr; ++SpawnHelperItr)
		{
			ASpawnHelper *SpawnHelper = *SpawnHelperItr;

			if (SpawnHelper->SpawnHelperType == ESpawnHelperType::Artifact)
			{
				Level_2_PickupParameters.ArtifactLocations[ArtifactIndex] = SpawnHelper->GetActorLocation();
				ArtifactIndex++;
			}

			else if (SpawnHelper->SpawnHelperType == ESpawnHelperType::Shield)
			{
				Level_2_PickupParameters.ShieldLocations[ShieldIndex] = SpawnHelper->GetActorLocation();
				ShieldIndex++;
			}
		}
	}
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
			Artifact->bKeepTrackOf = true;
			Artifact->SetArrayIndex(i);
		}
	}
	int NumberOfShieldsToSpawn = LevelPickupParameters[Level]->ShieldLocations.Num();

	for (int i = 0; i < NumberOfShieldsToSpawn; ++i)
	{
		if (!LevelPickupParameters[Level]->ShieldPickedUp[i])
		{
			AS_PickupShield* Shield = World->SpawnActor<AS_PickupShield>(ShieldToSpawn, LevelPickupParameters[Level]->ShieldLocations[i], FRotator(0));
			Shield->bKeepTrackOf = true;
			Shield->SetArrayIndex(i);
		}
	}
}

void UWorldSwitchingGameInstance::RegisterPickedUp(int index, AActor* OtherActor)
{
	if (Cast<AArtifacts>(OtherActor))
	LevelPickupParameters[Level]->ArtifactPickedUp[index] = true;

	else if (Cast<AS_PickupShield>(OtherActor))
	LevelPickupParameters[Level]->ShieldPickedUp[index] = true;

}


void UWorldSwitchingGameInstance::SetArtifactPickedUp(int index)
{
	if (GetWorld()->GetMapName().Contains("1"))
	{

		Level_1_PickupParameters.ArtifactPickedUp[index] = false;
	}

	if (GetWorld()->GetMapName().Contains("2"))
	{

		Level_2_PickupParameters.ArtifactPickedUp[index] = false;
	}
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

void UWorldSwitchingGameInstance::ResetGameVariables()
{
	bIsFirstTimeStartingGame = true;
	bIsFirsTimeLoadingLevelOne = true;
	bIsFirsTimeLoadingLevelTwo = true;
	GI_AltarArtifacts = 0;
	GI_PlayerArtifacts = 0;
	GI_PlayerHealth = 3;
}