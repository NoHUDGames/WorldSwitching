// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "WorldSwitchingGameModeBase.h"
#include "Sphere_WorldChange.generated.h"

//This is purely a VFX object. A sphere with flipped normals and an animated material,
//that expands or contracts depending on worlds 

UCLASS()
class WORLDSWITCHING_API ASphere_WorldChange : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASphere_WorldChange();

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* BallMesh = nullptr;

	UMaterialInstanceDynamic* DynamicMaterial = nullptr;

	AWorldSwitchingGameModeBase* GameModeRef = nullptr;

	UFUNCTION(BlueprintImplementableEvent)
	void Fade(UMaterialInstanceDynamic* Material);

	bool bIsSpiritWorld;

	FVector StartScale;
	FVector EndScale;

	//Triggers timeline in blueprint that drives values into Morph();
	UFUNCTION(BlueprintImplementableEvent)
		void TriggerMorph(bool bIsSpiritWorld);
	UFUNCTION(BlueprintCallable)
		void Morph(float TimeLine);

	FTimerHandle KillTimer;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
