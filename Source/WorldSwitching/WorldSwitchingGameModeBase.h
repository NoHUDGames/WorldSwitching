// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/CapsuleComponent.h"
#include "EngineUtils.h"
#include "TimerManager.h"
#include "BP_Character.h"
#include "Altar.h"
#include "LevelCamera.h"
#include "OurEnums.h"
#include "Engine/StaticMesh.h"
#include "WorldSwitchingGameInstance.h"
#include "WorldSwitchingGameModeBase.generated.h"

/**
 * 
 */


class UCameraComponent;


UCLASS()
class WORLDSWITCHING_API AWorldSwitchingGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

		AWorldSwitchingGameModeBase();

		virtual void BeginPlay() override;

		virtual void Tick(float DeltaTime) override;
		
public:

	UFUNCTION(BlueprintCallable)
	void ChangeWorlds(bool bShowTransitionEffects = true);

	void ToggleAll();

	//Because input cannot trigger functions with parameters?
	void ChangeWorldsProxy();

	//Returns true if collision occurred
	bool TestWorldChangeOverlaps();

	//Not going to deny reentry if colliding with Spirit Objects.
	//void TestSpiritOverlaps();

	UPROPERTY(BlueprintReadOnly)
	bool bIsSpiritWorld;
	
	
	UCameraComponent* CameraComponent = nullptr;

	ALevelCamera* FreeLevelCamera = nullptr;

	void GatherLevelCameras();

	ALevelCamera* GetFreeLevelCamera() { return FreeLevelCamera; }

	APlayerController* PlayerController = nullptr;

	UCapsuleComponent* PlayerCapsuleCollision = nullptr;

	ABP_Character* PlayerPawn = nullptr;

	AActor* OtherActorPhysicalTest = nullptr;


	UWorldSwitchingGameInstance* GameInstance = nullptr;

	UWorld* CurrentWorld = nullptr;
	FString CurrentMapName;



	UPROPERTY(EditAnywhere, Category ="Error Sound")
	USoundBase* CannotReturnToPhysical = nullptr;

	UFUNCTION(BlueprintCallable)
		UCameraComponent* GetPawnCameraComponent()
	{
		return CameraComponent;
	}

	//Gir ut actoren man kolliderte med når 
	UFUNCTION(BlueprintCallable)
		AActor* GetOtherActorPhysicalTest();

	UFUNCTION(BlueprintImplementableEvent)
	void LightUpCollidingActor(ASWorldActor* SWordActor = nullptr, APWorldActor* PWorldActor = nullptr);

	UFUNCTION(BlueprintImplementableEvent)
	void WorldTransitionEffects();

	UFUNCTION(BlueprintImplementableEvent)
	void DeniedPhysicalReentryEffects();

	void TogglePhysicalWorldActors();
	void ToggleSpiritWorldActors();
	void ToggleLastingCameraEffects();

	//For ParticleEffects that have a spiritworld and physicalworld component
	void ToggleParticleEffects();

	void TogglePhysicalSpiritActors();

	UFUNCTION(BlueprintImplementableEvent)
	void ToggleLandscapes();

	//Landscape variables are in the blueprint. Unreal wound not allow BlueprintReadWriteable ALandscape*
	UFUNCTION(BlueprintImplementableEvent)
	void SetLandscapesReferences();

	

	//SensingSphereStuff
	int LitUpBySensing = 0;
	int GetLitUpBySensing() { return LitUpBySensing; }
	void IncrementLitUpBySensing() 
	{ 
		++LitUpBySensing;
		UE_LOG(LogTemp, Warning, TEXT("GAME MODE: Objects Lit up: %i"), LitUpBySensing)
	}

	void DecrementLitUpBySensing() 
	{	
		if(LitUpBySensing > 0)	--LitUpBySensing;
		
		UE_LOG(LogTemp, Warning, TEXT("GAME MODE: Objects Lit up: %i"), LitUpBySensing)
	}

	bool bSphereIsRunning = false;
	bool GetSphereIsRunning() { return bSphereIsRunning; }
	void SetSphereIsRunning(bool state) { bSphereIsRunning = state; }
	//SensingSphereStuff


	//FoliageMeshes
	UPROPERTY(EditAnywhere, Category = Foliage)
	TArray<UStaticMesh*> Meshes;

	UPROPERTY(EditAnywhere, Category = Foliage)
	TArray<int> NumberOfMaterials;

	UPROPERTY(EditAnywhere, Category = Foliage)
	TArray<UMaterialInterface*> PhysicalMaterials;

	UPROPERTY(EditAnywhere, Category = Foliage)
	TArray<UMaterialInterface*> SpiritMaterials;

	void ToggleInstanceMeshes();

	UPROPERTY(EditAnywhere, Category = EnemyMaterials)
	TArray<UMaterialInterface*> SpiritEnemyPhysicalWorldMaterials;
	UPROPERTY(EditAnywhere, Category = EnemyMaterials)
	TArray<UMaterialInterface*> SpiritEnemySpiritWorldMaterials;

};


