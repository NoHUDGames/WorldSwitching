// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInstance.h"
#include "PSWorldActor.h"
#include "PWorldActor.h"
#include "SWorldActor.h"
#include "Altar.h"
#include "SpringArmSensor.generated.h"

UENUM()
enum OneOrTwoMeshComponent
{
	One,
	Two
};

UCLASS()
class WORLDSWITCHING_API ASpringArmSensor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpringArmSensor();

	UPROPERTY(EditAnywhere)
	UBoxComponent* Collider = nullptr;

	UMaterialInstanceDynamic* DynamicMaterial;

	TArray<UMaterialInterface*> OriginalMaterialsPrimary;
	TArray<UMaterialInterface*> OriginalMaterialsSecondary;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* DummyMaterial = nullptr;


	UFUNCTION(BlueprintImplementableEvent)
		void FadeDownTrigger();

	UFUNCTION(BluePrintCallable)
		void FadeDown(float TimeLine);

	UFUNCTION(BlueprintImplementableEvent)
		void FadeUpTrigger();

	UFUNCTION(BluePrintCallable)
		void FadeUp(float TimeLine);

	float FadeUpStart = 0.f;
	float FadeUpEnd = 1.f;
	float FadeDownStart = 1.f;
	float FadeDownEnd = 0.f;

	void ReapplyOriginalMaterials();

	//Return true if Dynamic Materials have been created
	bool CheckForPSWorldActor(AActor* OtherActor);
	bool CheckForPWorldActor(AActor* OtherActor);
	bool CheckForSWorldActor(AActor* OtherActor);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
