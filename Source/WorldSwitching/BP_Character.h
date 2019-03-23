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
	class UTimelineComponent* DashingTimeline;

public:
	// Sets default values for this character's properties
	ABP_Character();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void PlayingAnimations();

	void ChangingAnimationStarted(int index);

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/// These functions makes it possible for the player to move
	void MoveUp(float AxisValue);
	void MoveRight(float AxisValue);

	void Dashing();
	bool GetCurrentlyDashing() { return CurrentlyDashing; };
	void ReverseCurrentlyDashing() { CurrentlyDashing = !CurrentlyDashing; };

	FVector ActorLocation;
	FVector GoalLocation;
	
	UPROPERTY(EditAnywhere, Category = "Movement")
		class UCurveFloat* fDashingCurve;

	/// declare our delegate function to be binded with TimelineFloatReturn()
	FOnTimelineFloat InterpDashingFunction{};

	/// Declare our delegate function to be binded with OnTimelineFinished()
	FOnTimelineEvent DashingTimelineFinished{};

	UFUNCTION()
		void DashingTimelineFloatReturn(float value);

	UFUNCTION()
		void OnDashingTimelineFinished();
	/// end of movement functions


	/// All these functions and variables deal with the players kicking
	void Kicking();
	void ResetKickingCombo();
	
	int NumberOfKicks{ 0 };

	FTimerHandle KickingDurationTimer;
	FTimerHandle ComboDurationTimer;
	FTimerHandle DeathSequenceTimer;
	FTimerHandle DashCooldown;

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
	FOnTimelineFloat InterpKickingFunction{};

	/// Declare our delegate function to be binded with OnTimelineFinished()
	FOnTimelineEvent KickingTimelineFinished{};

	UFUNCTION()
		void KickingTimelineFloatReturn(float value);

	UFUNCTION()
		void OnKickingTimelineFinished();

	/// end of functions and variables for kicking



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

	FVector GetRespawnLocation() {return RespawnLocation;};


	//Is set from PS_SavePoint
	FVector RespawnLocation;

	
	int GetLives() {return Lives;};
	int GetArtifacts() {return NumberOfHoldingArtifacts;};
	
	UFUNCTION(BlueprintCallable)
	int GetShields() { return NumberOfShields; };

	void SetLives(int NewHealth);
	void SetArtifacts(int NewArtifacts);
	void SetShields(int NewShields) {NumberOfShields = NewShields;};
	void SetbIsSpiritWorld(bool state);
	
	


private:
	
	/// Variables and enums that are related to animations
	UAnimSequence* IdleAnim;
	UAnimSequence* KickingAnim;
	UAnimSequence* WalkingAnim;
	UAnimSequence* DashingAnim;
	UAnimSequence* StrifingAnim;

	/// 0 = IdleAnim, 1 = KickingAnim, 2 = WalkingAnim, 3 = StrifingAnim, 4 = DashingAnim
	bool AnimationStarted[5] = {false};

	enum Animations {
		WALKINGFORWARD, STRIFING, KICKING, IDLE, DASHING
	};

	Animations RunningAnimations{ IDLE };
	/// end of variable and enums that are related to animations

	int NumberOfShields{ 0 };

	bool CurrentlyDashing{ false };
	float DashingDistance{ 500.f };

	void MovementAnimationTesting(float AxisValue, float ForwardVector);
	

};
