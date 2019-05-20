// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "PWorldActor.generated.h"


//Basis for all objects only visible in physical world 

UCLASS()
class WORLDSWITCHING_API APWorldActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APWorldActor();

	//Is tested agains in Toggle-functions in game mode. Good for allowing behaviour outside of normal toggling
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WorldChange)
		bool bOptOutOfCollisionChange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WorldChange)
		bool bOptOutOfVisibilityChange;
	
	//Must be true for sensingsphere to trigger light up
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WorldChange)
		bool bCanBeSensed = false;

	class AWorldSwitchingGameModeBase* GameModeRef = nullptr;

	//Because we started out adding meshcomponents in blueprints (D'oh!), we get the reference here
	UStaticMeshComponent* MeshRef = nullptr;

	UFUNCTION(BlueprintCallable)
		void SetMeshRef(UStaticMeshComponent* MeshToGet) { MeshRef = MeshToGet; }

	UFUNCTION(BlueprintImplementableEvent)
		void TriggerSetMeshRef();

	UStaticMeshComponent* GetMeshRef() { return MeshRef; }

	int NumberOfMaterials = 0;

	//Used to light up actor when sensed
	UFUNCTION(BlueprintImplementableEvent)
		void TimeLineLightUpTrigger();

	UFUNCTION(BlueprintCallable)
		void TimeLineLightUp(float TimeLine);

	UFUNCTION(BlueprintCallable)
		void ReapplyOriginalMaterials();

	//Manage materials while lighting up. Set for objects with several material elements, but in the end
	//only used with objects with one element
	TArray<UMaterialInterface*> OriginalMaterial;
	TArray<UMaterialInstanceDynamic*> DynamicMaterials;


	//Must ble blend mode translucent
	UPROPERTY(EditAnywhere)
	UMaterialInterface* DummyLightUpMaterial = nullptr;


	bool bCurrentIsSpiritWorld = false;

	//Called from sensing sphere. AArtifact has a slightly different implementation, hence I used polymorphy
	virtual void LightUpActorWhenSensed();

	void ApplyDynamicMaterials();

	void GetOriginalMaterials();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	

};
