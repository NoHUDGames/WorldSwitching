// Fill out your copyright notice in the Description page of Project Settings.

#include "S_PickupShield.h"
#include "WorldSwitchingGameModeBase.h"

void AS_PickupShield::Tick(float DeltaTime)
{
	AddActorLocalRotation(FRotator(0.f, 50.f, 0.f)*DeltaTime);


}

void AS_PickupShield::SetArrayIndex(int index)
{
	ArrayIndex = index;
}

int AS_PickupShield::GetArrayIndex()
{
	return ArrayIndex;
}

void AS_PickupShield::LightUpActorWhenSensed()
{
	UE_LOG(LogTemp, Warning, TEXT("Shield: LightUpActorWhenSensed was called through poliymorphy"))
		m_bIsSpiritWorld = Cast<AWorldSwitchingGameModeBase>(GetWorld()->GetAuthGameMode())->bIsSpiritWorld;



	if (ShieldMesh)
	{
		ShieldMesh->SetMaterial(0, ShieldLightUpMaterial);
		ShieldDynamicMaterial = ShieldMesh->CreateDynamicMaterialInstance(0);

		SetActorHiddenInGame(false);

		TriggerShieldLightUpTimeline();
		UE_LOG(LogTemp, Warning, TEXT("ARTIFACT:Triggered ArtifactLightUp Successfully"))
	}
	else UE_LOG(LogTemp, Warning, TEXT("ARTIFACT:LIGHTUP:WE DO NOT HAVE ARTIFACTMESH"))


}

//Called in blueprint in above function
void AS_PickupShield::LightUpShield(float TimeLine)
{
	ShieldDynamicMaterial->SetScalarParameterValue("Opacity", TimeLine);
}

void AS_PickupShield::ResetAfterLightUp()
{
	ShieldMesh->SetMaterial(0, ShieldOriginalMaterial);
	SphereCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}