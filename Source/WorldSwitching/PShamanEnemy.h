// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PWorldCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/BoxComponent.h"
#include "PShamanEnemy.generated.h"

/**
 * 
 */
UCLASS()
class WORLDSWITCHING_API APShamanEnemy : public APWorldCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APShamanEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/// Start of variables and functions for the enemy attack
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UBoxComponent* BoxCollider {
		nullptr
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USceneComponent* WeaponRotation {
		nullptr
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* WeaponVisual {
		nullptr
	};

	/// End of variables and functions for the enemy attack

	int Lives{ 3 };

	void DecrementingLives();

	void KillingEnemy();

	UFUNCTION()
		void HittingPlayer(UPrimitiveComponent * OverlappedComp, AActor * OtherActor,
			UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
};
