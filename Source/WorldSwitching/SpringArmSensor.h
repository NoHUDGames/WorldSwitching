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
#include "SpringArmSensor.generated.h"

UCLASS()
class WORLDSWITCHING_API ASpringArmSensor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpringArmSensor();

	UPROPERTY(EditAnywhere)
	UBoxComponent* Collider = nullptr;

	TArray<UMaterialInstanceDynamic*> DynamicMaterialsPrimary;
	TArray<UMaterialInstanceDynamic*> DynamicMaterialsSecondary;

	TArray<UMaterialInterface*> OriginalMaterials;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* DummyMaterial = nullptr;


	void FadeDownProxy(UPrimitiveComponent * OverlappedComp, AActor * OtherActor,
		UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	
	void FadeUpProxy(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintImplementableEvent)
		void FadeDown();

	UFUNCTION(BlueprintImplementableEvent)
		void FadeUp();


	APSWorldActor* CheckForPSWorldActor(AActor* OtherActor);

	void test();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
