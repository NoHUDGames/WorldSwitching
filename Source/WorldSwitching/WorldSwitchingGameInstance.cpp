// Fill out your copyright notice in the Description page of Project Settings.

#include "WorldSwitchingGameInstance.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"


UWorldSwitchingGameInstance::UWorldSwitchingGameInstance()
{

}


void UWorldSwitchingGameInstance::ManageLevelArtifactsState()
{
	GetCurrentLevel();

	if (bIsFirsTimeLoadingLevelOne || bIsFirsTimeLoadingLevelOne)
	ManageFirstTimeLevelLoad();



}

void UWorldSwitchingGameInstance::GetCurrentLevel()
{
	CurrentMapName = GetWorld()->GetMapName();

	if (CurrentMapName.Contains("Level_1")) CurrentLevelLoaded = ECurrentLevelLoaded::Level_1;
	else if (CurrentMapName.Contains("Level_2")) CurrentLevelLoaded = ECurrentLevelLoaded::Level_2;
}

void UWorldSwitchingGameInstance::ManageFirstTimeLevelLoad()
{
	
	if (CurrentMapName.Contains("Level_1"))
	{
		bIsFirsTimeLoadingLevelOne = false;

		for (TActorIterator<AArtifacts> ArtifactItr(GetWorld()); ArtifactItr; ++ArtifactItr)
		{
			AArtifacts* Artifact = *ArtifactItr;
			Level_1Artifacts.Add(Artifact);
		}
		Level_1Artifacts.Sort();
		//AllArtifactsState.Append(Level_1Artifacts);

		for (auto x : Level_1Artifacts)
		{
			UE_LOG(LogTemp, Warning, TEXT("Artifact in level1: %s"), *x->GetActorLabel())
		}
	}

	else if (CurrentMapName.Contains("Level_2"))
	{
		bIsFirsTimeLoadingLevelTwo = false;

		for (TActorIterator<AArtifacts> ArtifactItr(GetWorld()); ArtifactItr; ++ArtifactItr)
		{
			AArtifacts* Artifact = *ArtifactItr;
			Level_2Artifacts.Add(Artifact);
		}
		Level_2Artifacts.Sort();
		//AllArtifactsState.Append(Level_2Artifacts);

		for (auto x : Level_2Artifacts)
		{
			UE_LOG(LogTemp, Warning, TEXT("Artifact in level2: %s"), *x->GetActorLabel())
		}
	}
}
