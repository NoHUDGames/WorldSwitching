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
#include "Animation/BlendSpace.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "BP_Character.generated.h"

class AArtifacts;


UCLASS()
class WORLDSWITCHING_API ABP_Character : public ACharacter
{
	GENERATED_BODY()

	/// timelines for dashing and the movement of the floating head
	class UTimelineComponent* DashingTimeline;
	class UTimelineComponent* FloatingHeadTimeline;

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

	/// Calculates how long the player is falling
	void CalculateFallDuration();

	///Plays the correct animations
	void PlayingAnimations();

	///Changes the values of AnimationStarted, To make sure that the currently playing animation are only set to play once
	void ChangingAnimationStarted(int index);

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/// These functions makes it possible for the player to move
	void MoveUp(float AxisValue);
	void MoveRight(float AxisValue);

	/// The function for dashing. Is bound to Left Shift
	void Dashing();
	bool GetCurrentlyDashing() { return CurrentlyDashing; };
	void ReverseCurrentlyDashing() { CurrentlyDashing = !CurrentlyDashing; };

	///Values that sets the start location of the dash and the goal location
	FVector ActorLocation;
	FVector GoalLocation;

	UWorld* CurrentWorld = nullptr;
	
	/// The curve for the dashing ability to move smoothly
	UPROPERTY(EditAnywhere, Category = "Movement")
		class UCurveFloat* fDashingCurve;

	/// declare our delegate function to be binded with TimelineFloatReturn()
	FOnTimelineFloat InterpDashingFunction{};

	/// Declare our delegate function to be binded with OnTimelineFinished()
	FOnTimelineEvent DashingTimelineFinished{};

	///The function that calculates the current location of the character while it is dashing
	UFUNCTION()
		void DashingTimelineFloatReturn(float value);

	///What happens when the dashing timeline is over
	UFUNCTION()
		void OnDashingTimelineFinished();

	/// end of movement functions and variables


	/// All these functions and variables deal with the players combat
	
	///The main kicking function. it is bound to left mouse button
	void Kicking();

	///Resets the kicking combo
	///The kicking combo isn't completely implemented and doesn't actually do anything because we didn't get any animation for it
	void ResetKickingCombo();

	///The sound played when the player is kicking
	UPROPERTY(EditAnywhere, Category = Sound)
	USoundBase* KickingSoundDefault = nullptr;
	
	///All time handlers that are used for managing a call function delay through the use of SetTimer()
	FTimerHandle KickingDurationTimer;
	FTimerHandle ComboDurationTimer;
	FTimerHandle DeathSequenceTimer;
	FTimerHandle DashCooldown;
	FTimerHandle DeathAnimationTimer;
	FTimerHandle ActivatingDeathSmokeTimer;
	FTimerHandle ArtifactDisplayResetTimer;

	
	///The kicking collision test for hitting enemies
	UFUNCTION()
		void HittingEnemy(UPrimitiveComponent * OverlappedComp, AActor * OtherActor,
			UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	///The boxCollider used for kicking collision
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kicking")
		UBoxComponent* BoxCollider {
		nullptr
	};
	
	///Checks whether or not the player is targeting an enemy
	///Is used in the blueprint version of the character (BP_Character_BP)
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Combat")
	bool isTargetingEnemy{ false };

	///The function that says what happens when the kicking is over
	void KickingFinished();

	///The function that says what happens if the takingHit animation is over
	void TakingHitAnimationOver();

	/// end of functions and variables for combat


	/// These functions turns on and off the collision with objects that have the Interaction collision channel on them
	/// This is how we will test if it's possible for the player to interact with something
	void Interact();
	void StopInteracting();
	/// end of functions for interacting

	///Decrements the health of the player when hit by an enemy
	void DecrementingLives();

	///Function that starts the death process
	void CurrentlyDying();

	///The sound playing when the player dies
	UPROPERTY(EditAnywhere, Category = Sound)
	USoundBase* DeathSound = nullptr;

	///The sound playing when the DeathSmoke is active
	UPROPERTY(EditAnywhere, Category = Sound)
	USoundBase* DeathSmokeSound = nullptr;

	///The sound playing when the player takes damage
	UPROPERTY(EditAnywhere, Category = Sound)
	USoundBase* TakeDamageSound = nullptr;

	///The sound playing when the player tries to sense
	UPROPERTY(EditAnywhere, Category = Sound)
	USoundBase* SensingSound = nullptr;

	///The function for sensing objects in the parallell world
	void SenseWorld();

	///The function that kills the enemy, after the CurrentlyDying has started the sequence
	UFUNCTION(BlueprintCallable)
	void DeathSequence(bool bWithArtifactLoss = true);

	void DeathSequenceProxy();

	///Respawns the player after dying.
	///Respawns at the last checkpoint
	void RespawnSequence();

	///The particle system for the smoke that appears when the player dies
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UParticleSystemComponent* DeathSmoke = nullptr;

	///The reference to the GameMode
	class AWorldSwitchingGameModeBase* GameModeRef = nullptr;

	AActor* OtherActorForPhysicalTest = nullptr;
	AActor* GetOtherActorForPhysicalTest();

	///The reference to the GameInstance
	UWorldSwitchingGameInstance* GameInstance = nullptr;

	///The Reference to the player controller
	AOurPlayerController* PlayerController = nullptr;


	/// Start of functions and variables dealing with artifact picking
	
	///Collision test function for picking up artifacts and shields
	UFUNCTION()
	void PickingUpArtifacts(UPrimitiveComponent * OverlappedComp, AActor * OtherActor,
		UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	///Collision test, checking if the player is allowed to drop off artifacts at the goddess altar
	/// The test starts to run when the player clicks on the interact button (E)
	UFUNCTION()
		void DeliveringArtifacts(UPrimitiveComponent * OverlappedComp, AActor * OtherActor,
			UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	///How many artifacts the player is holding
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collecting")
	int NumberOfHoldingArtifacts;
	
	/// End of function and variables dealing with artifact picking

	///Variable telling a number of functions what world the player currently is in
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldChanging")
	bool bIsSpiritWorld{ false };

	bool bLastCheckpointWasOnSpiritGround{ false };

	EWorld WorldRequiredOnRespawn = EWorld::None;

	///The health of the player
	///Loses one life each time they are hit by an enemy
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	int Lives{ 3 };

	UPROPERTY(EditAnywhere)
	TArray<UMaterialInterface*> CapeHP;


	UPROPERTY(EditAnywhere, Category = "Collecting")
	TSubclassOf<AArtifacts> ArtifactsToSpawn;

	UPROPERTY(EditAnywhere, Category = SensingSphere)
	TSubclassOf<ASensingSphere> SensingSphereToSpawn;

	void SetRespawnLocation(FVector NewSaveLocation);

	FVector GetRespawnLocation() {return RespawnLocation;};


	//Is set from PS_SavePoint
	FVector RespawnLocation;

	///Activates the deathSmoke after a while from the time the player is killed
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
	
	///How long the player is allowed to fall before they die
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float FallDurationForDeath{ 1.f };


	/// Components, variables and functions related to head changing

	///The head mesh for the player
	///Trails behind the player when in spirit world, is attached to the HeadSocket when in physicalworld
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldSwitching")
		UStaticMeshComponent* Head {
		nullptr
	};

	///The mask mesh for the player
	///Trails behind the player when in physical world, is attached to the HeadSocket when in spirit world
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldSwitching")
		UStaticMeshComponent* Mask {
		nullptr
	};

	///The curve for the movement of the trailing head
	UPROPERTY(EditAnywhere, Category = "WorldSwitching")
		class UCurveFloat* fFloatingHeadCurve;

	/// How much the trailing head moves up and down behind the player
	UPROPERTY(EditAnywhere, Category = "WorldSwitching")
		float HeadFloatOffset;

	/// declare our delegate function to be binded with TimelineFloatReturn()
	FOnTimelineFloat InterpHeadFloatingFunction{};

	/// Declare our delegate function to be binded with OnTimelineFinished()
	FOnTimelineEvent HeadFloatingTimelineFinished{};

	///Sets the current location of the trailing head
	UFUNCTION()
		void FloatingHeadTimelineFloatReturn(float value);

	///Function for switching heads on the player
	void SwitchingHead();


	/// End of components, variable and functions related to head changing

	///The current running animation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		EAnimations RunningAnimations {EAnimations::MOVEMENT};
	
	///How much enemies are knocked back when attacked by the player
	UPROPERTY(EditAnywhere, Category = "Combat")
		float KnockbackForce{200.f};

	///The display of artifacts in the hud
	UPROPERTY(EditAnywhere, Category = "Collecting")
	TSubclassOf<class UUserWidget> ArtifactWidget;
	UUserWidget* ArtifactDisplay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collecting")
		bool PickedUpArtifact = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collecting")
		float LastTimePickedUpArtifact;

	/// This function makes sure that the display retracts back to ouside the viewport
	void ResetArtifactDisplayValues();

	///The camera shake that occurs when the player is hit by an enemy
	UPROPERTY(EditAnywhere, Category = "Combat")
		TSubclassOf<UCameraShake> TakingDamageCameraShake;
private:
	
	/// Variables and enums that are related to animations
	UAnimationAsset* KickingAnim;
	UAnimationAsset* DashingAnim;
	UAnimationAsset* DyingAnim;
	UAnimationAsset* IdleAnim;
	UAnimationAsset* BeingHitAnim;

	UBlendSpace* MovementAnimBlendSpace;

	///Determines which animation that have started running, to make sure that it doesn't run again
	/// 0 = MovementAnimBlendSpace, 1 = KickingAnim, 2 = DashingAnim, 3 = DyingAnim, 4 = IdleAnim, 5 = TakingDamage
	bool AnimationStarted[6] = { false };
	
	/// end of variable and enums that are related to animations

	///How many shields the player has, can maximum be 3
	int NumberOfShields{ 0 };

	bool CurrentlyDashing{ false };

	///How far the dash ability mvoes the player
	float DashingDistance{ 500.f };
	
	///The normal movement speed. The player movement speed is set to half if the spirit enemy is using its special ability
	float NormalSpeed{ 600.f };

	void MovementAnimationTesting(float AxisValue);

	
	///When the player started falling
	float startedFalling;

	///When the player stopped falling
	float endedFalling;

	///How many consecutive kicks the player has performed
	///Only applies to the not-active functionality of the kicking combo
	int NumberOfKicks{ 0 };

	///Whether or not the player is currently kicking
	bool CurrentlyKicking{ false };

	///The relative location of where the trailing head goes to when it moves up and down
	///It moves from the TrailingHeadLocation to the TrailingHeadGoalLocation with the help of a timeline
	FVector TrailingHeadGoalLocation;

	///The head socket location
	FVector HeadSocketLocation = FVector(0.f, 0.f, 0.f);

	///The location of the trailing head
	FVector TrailingHeadLocation = FVector(0.f, -43.f, 120.f);

	bool bFellToDeath{ false };
};
