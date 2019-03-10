// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PWorldActor.generated.h"

UCLASS()
class WORLDSWITCHING_API APWorldActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APWorldActor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WorldChange)
		bool bOptOutOfCollisionChange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WorldChange)
		bool bOptOutOfVisibilityChange;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
