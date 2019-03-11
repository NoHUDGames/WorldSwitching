// Fill out your copyright notice in the Description page of Project Settings.

#include "WorldSwitchingGameInstance.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"


UWorldSwitchingGameInstance::UWorldSwitchingGameInstance()
{

}


void UWorldSwitchingGameInstance::ManageLevelArtifacts()
{

	GetCurrentLevel();

	if ((CurrentLoadedLevel == ECurrentLevelLoaded::Level_1 && bIsFirsTimeLoadingLevelOne) ||
		(CurrentLoadedLevel == ECurrentLevelLoaded::Level_2 && bIsFirsTimeLoadingLevelTwo))
	{
		ManageFirstTimeLevelLoad();
		SpawnArtifacts();
		return;
	}

	SpawnArtifacts();

}

void UWorldSwitchingGameInstance::GetCurrentLevel()
{
	CurrentMapName = GetWorld()->GetMapName();

	if (CurrentMapName.Contains("Level_1")) CurrentLoadedLevel = ECurrentLevelLoaded::Level_1;
	else if (CurrentMapName.Contains("Level_2")) CurrentLoadedLevel = ECurrentLevelLoaded::Level_2;
	else if (CurrentMapName.Contains("Hub")) CurrentLoadedLevel = ECurrentLevelLoaded::Hub;
}

void UWorldSwitchingGameInstance::ManageFirstTimeLevelLoad()
{
	switch (CurrentLoadedLevel)	
	{
		case ECurrentLevelLoaded::Level_1:
			
			bIsFirsTimeLoadingLevelOne = false;
			Level_1ArtifactLocations_Continuing = Level_1ArtifactLocations_Start;
			//UE_LOG(LogTemp, Warning, TEXT("Level_1ArtifactLocations_Start has %i elements"), Level_1ArtifactLocations_Start.Num())
			break;

		case ECurrentLevelLoaded::Level_2:
			bIsFirsTimeLoadingLevelTwo = false;
			Level_2ArtifactLocations_Continuing = Level_2ArtifactLocations_Start;
			break;
	}
	
}

void UWorldSwitchingGameInstance::SpawnArtifacts()
{
	
	UWorld* World = GetWorld();
	int length;
	switch (CurrentLoadedLevel)
	{
	case ECurrentLevelLoaded::Level_1:
		length = Level_1ArtifactLocations_Continuing.Num();

		for (int i = 0; i < length; ++i)
		{
			//Save temporary pointer to spawned object
			if (Level_1ArtifactLocations_Continuing[i].X != -1000)
			{
				AArtifacts* Artifact = World->SpawnActor<AArtifacts>(ArtifactToSpawn, Level_1ArtifactLocations_Continuing[i], FRotator(0));
				Artifact->SetArrayIndex(i);
			}
		}
		break;

	case ECurrentLevelLoaded::Level_2:
		length = Level_2ArtifactLocations_Continuing.Num();

		for (int i = 0; i < length; ++i)
		{
			//Save temporary pointer to spawned object
			if (Level_2ArtifactLocations_Continuing[i].X != -1000.f)
			{
				AArtifacts* Artifact = World->SpawnActor<AArtifacts>(ArtifactToSpawn, Level_2ArtifactLocations_Continuing[i], FRotator(0));
				Artifact->SetArrayIndex(i);
			}
		}
		break;

	case ECurrentLevelLoaded::Hub:
		break;
	}
}

void UWorldSwitchingGameInstance::RemovePickedUpArtifact(int index)
{
	switch (CurrentLoadedLevel)
	{
	case ECurrentLevelLoaded::Level_1:

		Level_1ArtifactLocations_Continuing[index].X = -1000.f;
		break;

	case ECurrentLevelLoaded::Level_2:
		Level_2ArtifactLocations_Continuing[index].X = -1000.f;
		break;
	}

}