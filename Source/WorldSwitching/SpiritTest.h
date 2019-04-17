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


	/// Start of variables and functions that BP_Character uses to kill the enemy
	int Lives{ 3 };

	void KillingEnemy();

	void DecrementingLives(FVector KnockbackDirection = FVector(0.f, 0.f, 0.f));

	FTimerHandle DamageOverTimeHandler;
	bool bCanPerformNextDamageOverTime{ true };
	float DamageOverTimeCooldown{ 3.f };
	
	UFUNCTION(BlueprintCallable)
		void DamageOverTimeAttack();
	
	void CallingPlayerDecrementLivesFunction();

	UFUNCTION()
		void HittingPlayer(UPrimitiveComponent * OverlappedComp, AActor * OtherActor,
			UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	/// End of variables and functions that BP_Character uses to kill the enemy

	/// Start of variables and functions for the enemy attack
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UBoxComponent* BoxCollider { nullptr };


	/// End of variables and functions for the enemy attack

	void PlayingAnimations();
	void ChangingAnimationStarted(int index);

	/// 0 = TakingDamageAnim, 1 = AttackAnim, 2 = MovementAnim,
	bool AnimationStarted[3] = { false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		EAnimations RunningAnimations = EAnimations::MOVEMENT;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* HeadAfterDeath {nullptr};
	void SpawnHead();
	void DestroyActor();
	FTimerHandle SpawningHeadTimerHandler;
	FTimerHandle DestroyingActorTimerHandler;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UParticleSystemComponent* BlueSmoke = nullptr;

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

	void KnockbackEffect(FVector KnockbackDirection);

	void MovementAnimationTesting();

	FVector StartKnockbackLocation;
	FVector EndKnockbackLocation;
	

};
