// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "PWorldActor.generated.h"

UCLASS()
class WORLDSWITCHING_API APWorldActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APWorldActor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WorldChange)
		bool bOptOutOfCollisionChange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WorldChange)
		bool bOptOutOfVisibilityChange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WorldChange)
		bool bCanBeSensed = false;

	class AWorldSwitchingGameModeBase* GameModeRef = nullptr;

	UStaticMeshComponent* MeshRef = nullptr;

	UFUNCTION(BlueprintCallable)
		void SetMeshRef(UStaticMeshComponent* MeshToGet) { MeshRef = MeshToGet; }

	UFUNCTION(BlueprintImplementableEvent)
		void TriggerSetMeshRef();

	UStaticMeshComponent* GetMeshRef() { return MeshRef; }

	int NumberOfMaterials = 0;

	UFUNCTION(BlueprintImplementableEvent)
		void TimeLineLightUpTrigger();

	UFUNCTION(BlueprintCallable)
		void TimeLineLightUp(float TimeLine);

	UFUNCTION(BlueprintCallable)
		void ReapplyOriginalMaterials();

	TArray<UMaterialInterface*> OriginalMaterial;
	TArray<UMaterialInstanceDynamic*> DynamicMaterials;


	//Must ble blend mode translucent
	UPROPERTY(EditAnywhere)
		UMaterialInterface* DummyLightUpMaterial = nullptr;


	bool bCurrentIsSpiritWorld = false;

	//MAIN FUNCTION
	void LightUpActorWhenSensed();

	void ApplyDynamicMaterials();

	void GetOriginalMaterials();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
