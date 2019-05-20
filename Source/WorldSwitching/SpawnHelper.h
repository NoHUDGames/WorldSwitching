// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "OurEnums.h"
#include "SpawnHelper.generated.h"

UCLASS()
class WORLDSWITCHING_API ASpawnHelper : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnHelper();

	//Tried to connect spawnhelper and spawned pickup, but this reference would not be valid between levels.
	//Mainly I wanted to have pickups placed on moving objects to be kept track of between levels, but this
	//would require reference between them. Hence, artifacts on moving platforms are not kept track of.
	//But all other pickups are (See GameInstance for this system)
	UFUNCTION(BlueprintImplementableEvent)
	void AttachPickupToThis(AActor* Pickup);

	UPROPERTY(EditAnywhere)
	bool bAttachPickupToThis = false;
	//-----------------------------------------------

	USceneComponent* SceneComponent = nullptr;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Mesh = nullptr;

	//Determines what they will help spawn
	UPROPERTY(EditAnywhere)
	ESpawnHelperType SpawnHelperType;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
