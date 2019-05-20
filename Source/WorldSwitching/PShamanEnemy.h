// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PWorldCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/BoxComponent.h"
#include "SpiritTest.h"
#include "BP_Character.h"
#include "OurEnums.h"
#include "Animation/BlendSpace1D.h"
#include "Animation/AnimSequence.h"
#include "PShamanEnemy.generated.h"


/**
 * 
 */
UCLASS()
class WORLDSWITCHING_API APShamanEnemy : public APWorldCharacter
{
	GENERATED_BODY()

	///Timeline component for handling the knockback effect that is triggered when the enemy is hit by the player
	class UTimelineComponent* KnockbackTimeline;

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
	
	///The weapon collider
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UBoxComponent* BoxCollider {
		nullptr
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* Spear { nullptr };

	/// End of variables and functions for the enemy attack

	///Holds the health for the enemy
	int Lives{ 3 };

	///Decrements the enemy lives
	void DecrementingLives(FVector KnockbackDirection = FVector(0.f, 0.f, 0.f));

	///This tests whether or not the enemy is ready to be killed
	void KillingEnemy();

	///Collision testing to check whether or not the enemy is hitting the player
	UFUNCTION()
		void HittingPlayer(UPrimitiveComponent * OverlappedComp, AActor * OtherActor,
			UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	///Checks if it is time to spawn the spirit of the shaman
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bTimeToSpawnSpirit = false;

	///Plays the right animation
	void PlayingAnimations();

	///Changes the array that makes sure that the right animation is running only once
	void ChangingAnimationStarted(int index);

	
	/// 0 = AttackAnim, 1 = MovementBlendSpace
	bool AnimationStarted[2] = { false };

	UFUNCTION(BlueprintCallable)
		UAnimSequence* GetAttackAnim() { return AttackAnim; };

	///The enum that makes sure that the right animation is queued to start playing
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		EAnimations RunningAnimations = EAnimations::MOVEMENT;

	/// functions and variables for knockback timeline
	UPROPERTY(EditAnywhere, Category = "Combat")
		class UCurveFloat* fKnockbackCurve;

	/// declare our delegate function to be binded with TimelineFloatReturn()
	FOnTimelineFloat InterpKnockbackFunction{};

	UFUNCTION()
		void KnockbackTimelineFloatReturn(float value);

	/// end of functions and variables for knockback timeline

private:
	/// Variables and enums that are related to animations
	UAnimationAsset* IdleAnim;
	UAnimSequence* AttackAnim;
	UAnimationAsset* WalkingAnim;
	UAnimationAsset* DeathAnim;
	
	UBlendSpace1D* MovementAnimBlendSpace;

	void MovementAnimationTesting();

	/// end of variable and functions that are related to animations

	

	///The function to run the knockback effect
	void KnockbackEffect(FVector KnockbackDirection);

	///Locations for knockback handling
	FVector StartKnockbackLocation;
	FVector EndKnockbackLocation;

	FTimerHandle DestroyingShamanTimer;

	void DestroyingEnemyProxy();
};
