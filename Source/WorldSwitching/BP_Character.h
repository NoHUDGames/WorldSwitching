// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/BoxComponent.h"
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

	void MoveUp(float AxisValue);
	///void MoveDown(float AxisValue);
	///void MoveLeft(float AxisValue);
	void MoveRight(float AxisValue);

	void Kicking();
	void StopKicking();
	void ResetKickingCombo();

	UFUNCTION()
	void HittingEnemy(UPrimitiveComponent * OverlappedComp, AActor * OtherActor,
		UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	bool CurrentlyKicking{ false };

	FTimerHandle KickingDurationTimer;
	FTimerHandle ComboDurationTimer;

	int NumberOfKicks{ 0 };


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UBoxComponent* BoxCollider {nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USceneComponent* KickingRotation {nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* SphereVisual{ nullptr };

	
	

};
