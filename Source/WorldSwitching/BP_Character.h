// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "TimerManager.h"
#include "Components/TimelineComponent.h"
#include "WorldSwitchingGameInstance.h"
#include "OurPlayerController.h"
#include "BP_Character.generated.h"

class AArtifacts;


UCLASS()
class WORLDSWITCHING_API ABP_Character : public ACharacter
{
	GENERATED_BODY()

		class UTimelineComponent* KickingTimeline;

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
	FTimerHandle DeathSequenceTimer;

	bool CurrentlyKicking{ false };

	UFUNCTION()
		void HittingEnemy(UPrimitiveComponent * OverlappedComp, AActor * OtherActor,
			UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kicking")
		UBoxComponent* BoxCollider {
		nullptr
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kicking")
		USceneComponent* KickingRotation {
		nullptr
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kicking")
		UStaticMeshComponent* SphereVisual {
		nullptr
	};


	UPROPERTY(EditAnywhere, Category = "Kicking")
		class UCurveFloat* fKickingCurve;

	UPROPERTY()
		FRotator StartRotationOfKicking;

	UPROPERTY()
		FRotator EndRotationOfKicking;

	UPROPERTY(EditAnywhere, Category = "Kicking")
		float PitchOffset;
	
	/// declare our delegate function to be binded with TimelineFloatReturn()
	FOnTimelineFloat InterpFunction{};

	/// Declare our delegate function to be binded with OnTimelineFinished()
	FOnTimelineEvent TimelineFinished{};

	UFUNCTION()
		void KickingTimelineFloatReturn(float value);

	UFUNCTION()
		void OnKickingTimelineFinished();

	/// end of functions and variables for kicking

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystem* DeathExplosion = nullptr;


	/// These functions turns on and off the collision with objects that have the Interaction collision channel on them
	/// This is how we will test if it's possible for the player to interact with something
	void Interact();
	void StopInteracting();
	/// end of functions for interacting

	void DecrementingLives();


	UFUNCTION(BlueprintCallable)
	void DeathSequence();


	void RespawnSequence();


	AActor* OtherActorForPhysicalTest = nullptr;
	AActor* GetOtherActorForPhysicalTest();

	UWorldSwitchingGameInstance* GameInstance = nullptr;

	AOurPlayerController* PlayerController = nullptr;


	/// Start of functions and variables dealing with artifact picking
	UFUNCTION()
	void PickingUpArtifacts(UPrimitiveComponent * OverlappedComp, AActor * OtherActor,
		UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
		void DeliveringArtifacts(UPrimitiveComponent * OverlappedComp, AActor * OtherActor,
			UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int NumberOfHoldingArtifacts;

	/// End of function and variables dealing with artifact picking

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsSpiritWorld{ false };
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Lives{ 3 };

	UPROPERTY(EditAnywhere)
	TSubclassOf<AArtifacts> ArtifactsToSpawn;

	void SetRespawnLocation(FVector NewSaveLocation);

	FVector GetRespawnLocation();


	//Is set from PS_SavePoint
	FVector RespawnLocation;

	
	int GetLives();
	int GetArtifacts();

	void SetLives(int NewHealth);
	void SetArtifacts(int NewArtifacts);
	void SetbIsSpiritWorld(bool state);




};
