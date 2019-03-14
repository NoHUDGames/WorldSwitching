// Fill out your copyright notice in the Description page of Project Settings.

#include "WorldSwitchingGameInstance.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "SpawnHelper.h"
#include "Kismet/GameplayStatics.h"


UWorldSwitchingGameInstance::UWorldSwitchingGameInstance()
{

}

void UWorldSwitchingGameInstance::BeginGame()
{

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
				Level_1ArtifactLocations.Add(SpawnHelper->GetActorLocation());
				Level_1ArtifactPickedUp.Add(false);
			}
			else if (CurrentLoadedLevel == ECurrentLoadedLevel::Level_2)
			{
				Level_2ArtifactLocations.Add(SpawnHelper->GetActorLocation());
				Level_2ArtifactPickedUp.Add(false);
			}
		}
		else if (SpawnHelper->SpawnHelperType == ESpawnHelperType::Shield)
		{
			if (CurrentLoadedLevel == ECurrentLoadedLevel::Level_1)
			{
				Level_1ShieldLocations.Add(SpawnHelper->GetActorLocation());
				Level_1ShieldPickedUp.Add(false);
			}
			else if (CurrentLoadedLevel == ECurrentLoadedLevel::Level_2)
			{
				Level_2ShieldLocations.Add(SpawnHelper->GetActorLocation());
				Level_2ShieldPickedUp.Add(false);
			}
		}
	}
}

void UWorldSwitchingGameInstance::ManageLevelPickups()
{
	GetCurrentLevel();

	if ((CurrentLoadedLevel == ECurrentLoadedLevel::Level_1 && bIsFirsTimeLoadingLevelOne) ||
		(CurrentLoadedLevel == ECurrentLoadedLevel::Level_2 && bIsFirsTimeLoadingLevelTwo))
	{
		SetIsFirstTimeLoadingLevel();
		GatherSpawnLocations();
		SpawnPickups();
		return;
	}
	

	SpawnPickups();

}

void UWorldSwitchingGameInstance::GetCurrentLevel()
{
	CurrentMapName = GetWorld()->GetMapName();

	if (CurrentMapName.Contains("Level_1")) CurrentLoadedLevel = ECurrentLoadedLevel::Level_1;
	else if (CurrentMapName.Contains("Level_2")) CurrentLoadedLevel = ECurrentLoadedLevel::Level_2;
	else if (CurrentMapName.Contains("Hub")) CurrentLoadedLevel = ECurrentLoadedLevel::Hub;
}

void UWorldSwitchingGameInstance::SetIsFirstTimeLoadingLevel()
{
	switch (CurrentLoadedLevel)	
	{
		case ECurrentLoadedLevel::Level_1:
			bIsFirsTimeLoadingLevelOne = false;


		case ECurrentLoadedLevel::Level_2:
			bIsFirsTimeLoadingLevelTwo = false;
			break;
	}
}

void UWorldSwitchingGameInstance::SpawnPickups()
{
	UWorld* World = GetWorld();

	if (CurrentLoadedLevel == ECurrentLoadedLevel::Level_1)
	{
		int NumberOfArtifactsToSpawn = Level_1ArtifactLocations.Num();

		for (int i = 0; i < NumberOfArtifactsToSpawn; ++i)
		{

			if (!Level_1ArtifactPickedUp[i])
			{
				AArtifacts* Artifact = World->SpawnActor<AArtifacts>(ArtifactToSpawn, Level_1ArtifactLocations[i], FRotator(0));
				Artifact->SetArrayIndex(i);
			}
		}
		int NumberOfShieldsToSpawn = Level_1ShieldLocations.Num();

		for (int i = 0; i < NumberOfShieldsToSpawn; ++i)
		{
			if (!Level_1ShieldPickedUp[i])
			{
				AS_PickupShield* Shield = World->SpawnActor<AS_PickupShield>(ShieldToSpawn, Level_1ShieldLocations[i], FRotator(0));
				Shield->SetArrayIndex(i);
			}
		}
		return;
	}

	if (CurrentLoadedLevel == ECurrentLoadedLevel::Level_2)
	{
		int NumberOfArtifactsToSpawn = Level_2ArtifactLocations.Num();

		for (int i = 0; i < NumberOfArtifactsToSpawn; ++i)
		{
			if (!Level_2ArtifactPickedUp[i])
			{
				AArtifacts* Artifact = World->SpawnActor<AArtifacts>(ArtifactToSpawn, Level_2ArtifactLocations[i], FRotator(0));
				Artifact->SetArrayIndex(i);
			}
		}

		int NumberOfShieldsToSpawn = Level_2ShieldLocations.Num();

		for (int i = 0; i < NumberOfShieldsToSpawn; ++i)
		{

			if (!Level_2ShieldPickedUp[i])
			{
				AS_PickupShield* Shield = World->SpawnActor<AS_PickupShield>(ShieldToSpawn, Level_2ShieldLocations[i], FRotator(0));
				Shield->SetArrayIndex(i);
			}
		}
	}
}

void UWorldSwitchingGameInstance::RegisterPickedUpArtifact(int index)
{
	switch (CurrentLoadedLevel)
	{
	case ECurrentLoadedLevel::Level_1:

		Level_1ArtifactPickedUp[index] = true;
		break;

	case ECurrentLoadedLevel::Level_2:
		Level_2ArtifactPickedUp[index] = true;
		break;
	}
}

void UWorldSwitchingGameInstance::RegisterPickedUpShield(int index)
{
	switch (CurrentLoadedLevel)
	{
	case ECurrentLoadedLevel::Level_1:

		Level_1ShieldPickedUp[index] = true;
		break;

	case ECurrentLoadedLevel::Level_2:
		Level_2ShieldPickedUp[index] = true;
		break;
	}
}

void UWorldSwitchingGameInstance::FetchPlayerHealth(int PlayerHealth)
{
	GI_PlayerHealth = PlayerHealth;
}

void UWorldSwitchingGameInstance::FetchPlayerArtifacts(int PlayerArtifacts)
{
	GI_PlayerArtifacts = PlayerArtifacts;
}

int UWorldSwitchingGameInstance::FeedPlayerHealth()
{
	return GI_PlayerHealth;
}

int UWorldSwitchingGameInstance::FeedPlayerArtifacts()
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