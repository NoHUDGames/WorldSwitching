// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "OurPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class WORLDSWITCHING_API AOurPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int Health = 3;

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int Artifacts = 0;


	void DecrementHealth();
	void SetHealth(int NewHealth);
	int GetHealth();

	void IncrementArtifacts();
};
