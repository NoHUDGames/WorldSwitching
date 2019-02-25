// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/BoxComponent.h"
#include "SpiritTest.generated.h"

UCLASS()
class WORLDSWITCHING_API ASpiritTest : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASpiritTest();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	/// Start of variables and functions that BP_Character uses to kill the enemy
	int Lives{ 3 };

	void KillingEnemy();

	void DecrementingLives();
	/// End of variables and functions that BP_Character uses to kill the enemy

	/// Start of variables and functions for the enemy attack
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UBoxComponent* BoxCollider { nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USceneComponent* WeaponRotation { nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* WeaponVisual { nullptr };


	/// End of variables and functions for the enemy attack


};
