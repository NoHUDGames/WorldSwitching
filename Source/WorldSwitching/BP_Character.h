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
#include "SensingSphere.h"
#include "OurEnums.h"
#include "BP_Character.generated.h"

class AArtifacts;


UCLASS()
class WORLDSWITCHING_API ABP_Character : public ACharacter
{
	GENERATED_BODY()

	class UTimelineComponent* DashingTimeline;
	class UTimelineComponent* FloatingHeadTimeline;
	class UTimelineComponent* SwitchingHeadTimeline;

public:
	// Sets default values for this character's properties
	ABP_Character();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;

	void CalculateFallDuration();

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

	float NormalSpeed{ 600.f };
	/// end of movement functions


	/// All these functions and variables deal with the players combat
	void Kicking();
	void ResetKickingCombo();
	
	int NumberOfKicks{ 0 };

	FTimerHandle KickingDurationTimer;
	FTimerHandle ComboDurationTimer;
	FTimerHandle DeathSequenceTimer;
	FTimerHandle DashCooldown;
	FTimerHandle DeathAnimationTimer;
	FTimerHandle ActivatingDeathSmokeTimer;

	bool CurrentlyKicking{ false };

	UFUNCTION()
		void HittingEnemy(UPrimitiveComponent * OverlappedComp, AActor * OtherActor,
			UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kicking")
		UBoxComponent* BoxCollider {
		nullptr
	};
	

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Combat")
	bool isTargetingEnemy{ false };

	void KickingFinished();

	/// end of functions and variables for combat


	/// These functions turns on and off the collision with objects that have the Interaction collision channel on them
	/// This is how we will test if it's possible for the player to interact with something
	void Interact();
	void StopInteracting();
	/// end of functions for interacting

	void DecrementingLives();

	void SenseWorld();

	UFUNCTION(BlueprintCallable)
	void DeathSequence();


	void RespawnSequence();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UParticleSystemComponent* DeathSmoke = nullptr;

	class AWorldSwitchingGameModeBase* GameModeRef = nullptr;

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collecting")
	int NumberOfHoldingArtifacts;

	/// End of function and variables dealing with artifact picking

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldChanging")
	bool bIsSpiritWorld{ false };
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	int Lives{ 3 };

	UPROPERTY(EditAnywhere, Category = "Collecting")
	TSubclassOf<AArtifacts> ArtifactsToSpawn;

	UPROPERTY(EditAnywhere, Category = SensingSphere)
	TSubclassOf<ASensingSphere> SensingSphereToSpawn;

	void SetRespawnLocation(FVector NewSaveLocation);

	FVector GetRespawnLocation() {return RespawnLocation;};


	//Is set from PS_SavePoint
	FVector RespawnLocation;

	void ActivateDeathSmoke();

	
	int GetLives() {return Lives;};
	int GetArtifacts() {return NumberOfHoldingArtifacts;};
	TArray<int> PickedUpArtifactsIndexes;

	UFUNCTION(BlueprintCallable)
	int GetShields() { return NumberOfShields; };

	void SetLives(int NewHealth);
	void SetArtifacts(int NewArtifacts);
	void SetShields(int NewShields) {NumberOfShields = NewShields;};
	void SetbIsSpiritWorld(bool state);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float FallDurationForDeath{ 1.f };


	/// Components, variables and functions related to head changing
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldSwitching")
		UStaticMeshComponent* Head {
		nullptr
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldSwitching")
		UStaticMeshComponent* Mask {
		nullptr
	};

	UPROPERTY(EditAnywhere, Category = "WorldSwitching")
		class UCurveFloat* fFloatingHeadCurve;

	UPROPERTY(EditAnywhere, Category = "WorldSwitching")
		float HeadFloatOffset;

	/// declare our delegate function to be binded with TimelineFloatReturn()
	FOnTimelineFloat InterpHeadFloatingFunction{};

	/// Declare our delegate function to be binded with OnTimelineFinished()
	FOnTimelineEvent HeadFloatingTimelineFinished{};

	UFUNCTION()
		void FloatingHeadTimelineFloatReturn(float value);

	void SwitchingHead();

	UPROPERTY(EditAnywhere, Category = "WorldSwitching")
		class UCurveFloat* fHeadSwitchingCurve;

	UPROPERTY(EditAnywhere, Category = "WorldSwitching")
		float HeadSwitchingOffset;

	/// declare our delegate function to be binded with TimelineFloatReturn()
	FOnTimelineFloat InterpHeadSwitchingFunction{};

	/// Declare our delegate function to be binded with OnTimelineFinished()
	FOnTimelineEvent HeadSwitchingTimelineFinished{};

	UFUNCTION()
		void SwitchingHeadTimelineFloatReturn(float value);

	UFUNCTION()
		void OnHeadSwitchingTimelineFinished();


	/// End of components, variable and functions related to head changing

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		EAnimations RunningAnimations {EAnimations::IDLE};

private:
	
	/// Variables and enums that are related to animations
	UAnimationAsset* IdleAnim;
	UAnimationAsset* KickingAnim;
	UAnimationAsset* WalkingAnim;
	UAnimationAsset* DashingAnim;
	UAnimationAsset* StrifingAnim;
	UAnimationAsset* DyingAnim;

	/// 0 = IdleAnim, 1 = KickingAnim, 2 = WalkingAnim, 3 = StrifingAnim, 4 = DashingAnim, 5 = DyingAnim
	bool AnimationStarted[6] = { false };

	enum Animations {
		WALKINGFORWARD, STRIFING, KICKING, IDLE, DASHING
	};
	
	/// end of variable and enums that are related to animations

	int NumberOfShields{ 0 };

	bool CurrentlyDashing{ false };
	float DashingDistance{ 500.f };

	void MovementAnimationTesting(float AxisValue, float ForwardVector);

	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<UCameraShake> TakingDamageCameraShake;
	
	float startedFalling;
	float endedFalling;

	FVector FloatingHeadStartLocation;
	FVector FloatingHeadGoalLocation;

	FVector HeadSocketLocation;
	FVector TrailingHeadLocation;
};
