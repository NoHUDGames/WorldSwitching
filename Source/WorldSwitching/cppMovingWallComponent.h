// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "cppMovingWallComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WORLDSWITCHING_API UcppMovingWallComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UcppMovingWallComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UStaticMeshComponent* StaticMeshWallComponent{ nullptr };

	UPROPERTY()
		FVector StartLocation;

	UPROPERTY(EditAnywhere)
		FVector EndLocation;

	UPROPERTY(EditAnywhere)
		float MovementSpeed;

	UPROPERTY(EditAnywhere)
		float PauseDuration;

	UPROPERTY(VisibleAnywhere)
		FVector MovementDirection;

	void MovingActor();

	void setIsReverse();

private:
	bool isReverse{ false };
	FTimerHandle PauseTimerHandler;
	bool TimerRunning{ false };
		
};
