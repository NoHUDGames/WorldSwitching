// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/BoxComponent.h"
#include "OurEnums.h"
#include "Engine/StaticMeshActor.h"
#include "Particles/ParticleSystemComponent.h"
#include "Animation/AnimSequence.h"
#include "Components/TimelineComponent.h"
#include "Animation/BlendSpace1D.h"
#include "SpiritTest.generated.h"

UCLASS()
class WORLDSWITCHING_API ASpiritTest : public ACharacter
{
	GENERATED_BODY()

	///Timeline component for handling the knockback effect that is triggered when the enemy is hit by the player
	class UTimelineComponent* KnockbackTimeline;
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
	
	///Holds the health for the enemy
	int Lives{ 3 };

	///This tests whether or not the enemy is ready to be killed
	void KillingEnemy();

	///Decrements the enemy health
	void DecrementingLives(FVector KnockbackDirection = FVector(0.f, 0.f, 0.f));

	///Start of variables and functions related to the spirit enemy special attack
	FTimerHandle DamageOverTimeHandler;
	bool bCanPerformNextDamageOverTime{ true };
	float DamageOverTimeCooldown{ 3.f };
	
	UFUNCTION(BlueprintCallable)
		void DamageOverTimeAttack();
	
	void CallingPlayerDecrementLivesFunction();
	///End of variables and functions related to the spirit enemy special attack

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundBase* TakeDamageSound = nullptr;

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundBase* DeathSound = nullptr;

	///The collision testing to check whether or not the enemy is hitting the player
	UFUNCTION()
		void HittingPlayer(UPrimitiveComponent * OverlappedComp, AActor * OtherActor,
			UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	///The weaponCollider
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UBoxComponent* BoxCollider { nullptr };

	///Plays the right animation
	void PlayingAnimations();

	///Changes the array that makes sure that the right animation is running only once
	void ChangingAnimationStarted(int index);

	/// 0 = TakingDamageAnim, 1 = AttackAnim, 2 = MovementAnim,
	bool AnimationStarted[3] = { false };

	///The enum that makes sure that the right animation is queued to start playing
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		EAnimations RunningAnimations = EAnimations::MOVEMENT;

	///The head that spawns after the enemy is killed
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* HeadAfterDeath {nullptr};
	
	///Spawns the head
	void SpawnHead();

	///Destroys the actor after it died
	void DestroyActor();
	FTimerHandle SpawningHeadTimerHandler;
	FTimerHandle DestroyingActorTimerHandler;

	/// Particle system intended for when the character dies
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UParticleSystemComponent* BlueSmoke = nullptr;

	/// Particle system intended for showing that the character isn't in the physical world
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UParticleSystemComponent* FireFlies = nullptr;

	UFUNCTION(BlueprintCallable)
		UAnimSequence* GetAttackAnim() { return AttackAnim; };

	/// functions and variables for knockback timeline
	UPROPERTY(EditAnywhere, Category = "Combat")
		class UCurveFloat* fKnockbackCurve;

	/// declare our delegate function to be binded with TimelineFloatReturn()
	FOnTimelineFloat InterpKnockbackFunction{};

	UFUNCTION()
		void KnockbackTimelineFloatReturn(float value);

	/// end of functions and variables for knockback timeline

	///This variable is used if it should trigger some effect of some sort after it has died
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzles")
	bool bIsPuzzleEnemy = false;

private:
	/// Variables that are related to animations
	UAnimationAsset* IdleAnim;
	UAnimSequence* AttackAnim;
	UAnimationAsset* WalkingAnim;
	UAnimationAsset* TakingDamageAnim;

	UBlendSpace1D* MovementAnimBlendSpace;

	FTimerHandle TakingDamageTimerHandler;
	void TurnOffTakingDamageAnim();
	/// end of variables that are related to animations

	///The function to run the knockback effect
	void KnockbackEffect(FVector KnockbackDirection);

	///Makes sure the right animation inside the movementblendspace is running
	void MovementAnimationTesting();

	///Locations for knockback handling
	FVector StartKnockbackLocation;
	FVector EndKnockbackLocation;
	

};
