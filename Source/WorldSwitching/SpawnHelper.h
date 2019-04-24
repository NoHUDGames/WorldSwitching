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

	UFUNCTION(BlueprintImplementableEvent)
	void AttachPickupToThis(AActor* Pickup);

	UPROPERTY(EditAnywhere)
	bool bAttachPickupToThis = false;

	USceneComponent* SceneComponent = nullptr;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Mesh = nullptr;

	UPROPERTY(EditAnywhere)
	ESpawnHelperType SpawnHelperType;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
