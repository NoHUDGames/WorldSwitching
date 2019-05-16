// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PWorldCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/BoxComponent.h"
#include "SpiritTest.h"
#include "BP_Character.h"
#include "OurEnums.h"
#include "PShamanEnemy.generated.h"


/**
 * 
 */
UCLASS()
class WORLDSWITCHING_API APShamanEnemy : public APWorldCharacter
{
	GENERATED_BODY()

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* Spear { nullptr };

	/// End of variables and functions for the enemy attack

	int Lives{ 3 };

	void DecrementingLives(FVector KnockbackDirection = FVector(0.f, 0.f, 0.f));

	void KillingEnemy();

	UFUNCTION()
		void HittingPlayer(UPrimitiveComponent * OverlappedComp, AActor * OtherActor,
			UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ASpiritTest> SpiritOfShaman;*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bTimeToSpawnSpirit = false;

	void PlayingAnimations();
	void ChangingAnimationStarted(int index);

	/// 0 = IdleAnim, 1 = AttackAnim, 2 = WalkingAnim, 3 = DeathAnim
	bool AnimationStarted[5] = { false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		EAnimations RunningAnimations = EAnimations::IDLE;

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
	UAnimationAsset* AttackAnim;
	UAnimationAsset* WalkingAnim;
	UAnimationAsset* DeathAnim;

	/// end of variable and enums that are related to animations

	void KnockbackEffect(FVector KnockbackDirection);

	FVector StartKnockbackLocation;
	FVector EndKnockbackLocation;

	FTimerHandle DestroyingShamanTimer;

	void DestroyingEnemyProxy();
};
