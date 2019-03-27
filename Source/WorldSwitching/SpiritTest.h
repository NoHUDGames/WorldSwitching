// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/BoxComponent.h"
#include "OurEnums.h"
#include "Engine/StaticMeshActor.h"
#include "Particles/ParticleSystemComponent.h"
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

	/// 0 = IdleAnim, 1 = AttackAnim, 2 = WalkingAnim, 3 = DeathAnim, 4 = TakingDamageAnim
	bool AnimationStarted[5] = { false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		EAnimations RunningAnimations = EAnimations::IDLE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* HeadAfterDeath {nullptr};
	void SpawnHead();
	void DestroyActor();
	FTimerHandle SpawningHeadTimerHandler;
	FTimerHandle DestroyingActorTimerHandler;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UParticleSystemComponent* BlueSmoke = nullptr;

	UFUNCTION(BlueprintCallable)
		UAnimationAsset* GetAttackAnim() { return AttackAnim; };

private:
	/// Variables that are related to animations
	UAnimationAsset* IdleAnim;
	UAnimationAsset* AttackAnim;
	UAnimationAsset* WalkingAnim;
	UAnimationAsset* DeathAnim;
	UAnimationAsset* TakingDamageAnim;

	FTimerHandle TakingDamageTimerHandler;
	void TurnOffTakingDamageAnim();
	/// end of variables that are related to animations

};
