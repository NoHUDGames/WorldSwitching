// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialLayersFunctions.h"
#include "Math/Color.h"
#include "PSWorldActor.generated.h"

UCLASS()
class WORLDSWITCHING_API APSWorldActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APSWorldActor();

	
	USceneComponent* SceneRoot = nullptr;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Mesh = nullptr;

	UMaterialInstanceDynamic* DynamicMaterial = nullptr;


	UPROPERTY(EditAnywhere, Category = "Dynamic Material Settings")
	bool bCanChangeMaterial;

	
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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
