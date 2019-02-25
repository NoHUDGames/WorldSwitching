// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "TimerManager.h"
#include "BP_Character.generated.h"




UCLASS()
class WORLDSWITCHING_API ABP_Character : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABP_Character();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/// These functions makes it possible for the player to move
	void MoveUp(float AxisValue);
	void MoveRight(float AxisValue);
	/// end of movement functions


	/// All these functions and variables deal with the players kicking
	void Kicking();
	void StopKicking();
	void ResetKickingCombo();
	
	int NumberOfKicks{ 0 };

	FTimerHandle KickingDurationTimer;
	FTimerHandle ComboDurationTimer;

	bool CurrentlyKicking{ false };

	UFUNCTION()
		void HittingEnemy(UPrimitiveComponent * OverlappedComp, AActor * OtherActor,
			UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UBoxComponent* BoxCollider {
		nullptr
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USceneComponent* KickingRotation {
		nullptr
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* SphereVisual {
		nullptr
	};
	/// end of functions and variables for kicking


	/// These functions turns on and off the collision with objects that have the Interaction collision channel on them
	/// This is how we will test if it's possible for the player to interact with something
	void Interact();
	void StopInteracting();
	/// end of functions for interacting


	AActor* OtherActorForPhysicalTest = nullptr;
	AActor* GetOtherActorForPhysicalTest();


	/// Start of functions and variables dealing with artifact picking
	UFUNCTION()
	void PickingUpArtifacts(UPrimitiveComponent * OverlappedComp, AActor * OtherActor,
		UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
		void DeliveringArtifacts(UPrimitiveComponent * OverlappedComp, AActor * OtherActor,
			UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int NumberOfHoldingArtifacts{ 0 };
	/// End of function and variables dealing with artifact picking

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsSpiritWorld{ false };
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Lives{ 3 };

	void SetRespawnLocation(FVector NewSaveLocation);

	FVector GetRespawnLocation();


	//Is set from PS_SavePoint
	FVector RespawnLocation;

};
