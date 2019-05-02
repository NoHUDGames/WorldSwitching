// Fill out your copyright notice in the Description page of Project Settings.

#include "Artifacts.h"
#include "WorldSwitchingGameModeBase.h"

AArtifacts::AArtifacts()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AArtifacts::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AddActorLocalRotation(FRotator(0.f, 50.f, 0.f)*DeltaTime);

}


void AArtifacts::SetArrayIndex(int index)
{
	ArrayIndex = index;
}

int AArtifacts::GetArrayIndex()
{
	return ArrayIndex;
}


void AArtifacts::LightUpActorWhenSensed()
{
	UE_LOG(LogTemp, Warning, TEXT("Aritifact: LightUpActorWhenSensed was called through polimorphy"))
	m_bIsSpiritWorld = Cast<AWorldSwitchingGameModeBase>(GetWorld()->GetAuthGameMode())->bIsSpiritWorld;

	

	if (ArtifactMesh)
	{
		ArtifactMesh->SetMaterial(0, ArtifactLightUpMaterial);
		ArtifactDynamicMaterial = ArtifactMesh->CreateDynamicMaterialInstance(0);
		
		SetActorHiddenInGame(false);

		TriggerArtifactLightUpTimeline();
		UE_LOG(LogTemp, Warning, TEXT("ARTIFACT:Triggered ArtifactLightUp Successfully"))
	}
	else UE_LOG(LogTemp, Warning, TEXT("ARTIFACT:LIGHTUP:WE DO NOT HAVE ARTIFACTMESH"))

	
}

//Called in blueprint in above function
void AArtifacts::LightUpArtifact(float TimeLine)
{
	ArtifactDynamicMaterial->SetScalarParameterValue("Opacity", TimeLine);
}

void AArtifacts::ResetAfterLightUp()
{
	ArtifactMesh->SetMaterial(0, ArtifactOriginalMaterial);
	SphereCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}