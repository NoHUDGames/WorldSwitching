// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialLayersFunctions.h"
#include "Materials/MaterialInstance.h"
#include "Math/Color.h"
#include "OurEnums.h"
#include "PSWorldActor.generated.h"

UCLASS()
class WORLDSWITCHING_API APSWorldActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APSWorldActor();

	UPROPERTY(EditAnywhere)
	USceneComponent* SceneRoot = nullptr;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* MeshPhysical = nullptr;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* MeshSpirit = nullptr;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* MeshBoth = nullptr;

	//The below is used if WorldChangeType is set to EWorldChange::SetMaterial. It was used in the beginning,
	//to make trees change appearances between levels, but it isn't used anymore, after we learned about
	//foliage. Now we have one material with many textures assigned to UStaticMesh trees that are foliaged out, 
	//where we toggle the visibility of each texture with a MaterialParameterCollection object. See 
	//AWorldSwitchingGameModeBase::ToggleFoliageMaterialProperties()

	UMaterialInstanceDynamic* DynamicMaterial = nullptr;

	UPROPERTY(EditAnywhere, Category = "Dynamic Material Settings")
	bool bCanChangeMaterial;

	UPROPERTY(EditAnywhere, Category = WorldChange)
	EWorldChangeType WorldChangeType;

	UPROPERTY(EditAnywhere, Category = WorldChange)
	TArray<UMaterialInterface*> PhysicalMaterials;
	UPROPERTY(EditAnywhere, Category = WorldChange)
	TArray<UMaterialInterface*> SpiritMaterials;

	int NumberOfMaterials;

	FLinearColor PhysicalColor;
	float PhysicalGlow;
	float PhysicalOpacity;

	UPROPERTY(EditAnywhere, Category = "Dynamic Material Settings")
	FLinearColor SpiritColor;
	
	UPROPERTY(EditAnywhere, Category = "Dynamic Material Settings")
	float SpiritGlow;

	UPROPERTY(EditAnywhere, Category = "Dynamic Material Settings")
	float SpiritOpacity;

	void ActivatePhysicalMaterialProperties();

	void ActivateSpiritMaterialProperties();

	void AssignSpiritMaterials();
	void AssignPhysicalMaterials();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
