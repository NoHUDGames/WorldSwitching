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
#include "Materials/MaterialParameterCollection.h"
#include "Kismet/GameplayStatics.h"
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

	//Main function to change worlds
	UFUNCTION(BlueprintCallable)
	void ChangeWorlds(bool bShowTransitionEffects = true);

	void ToggleAll();

	//Because input cannot trigger functions with parameters. Calls ChangeWorlds(bool bShowTransitionEffects = true)
	void ChangeWorldsProxy();

	//Tests collisions, triggers "no-return" effect and returns true if collision occurred
	bool TestWorldChangeOverlaps();

	//Used to test against in all toggle functions
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

	//Gir ut actoren man kolliderte med  
	UFUNCTION(BlueprintCallable)
	AActor* GetOtherActorPhysicalTest();

	//Start | Collision while changing world
	//The proper object-oriented way would be that objects light up themselves. Next time I would do that
	
	UPROPERTY(EditAnywhere)
	UMaterialInterface* RedLightupMaterial = nullptr;

	bool bIsLightUpEffectRunning = false;
	int NumberOfMaterials = 0;

	TArray<UMaterialInterface*> OriginalMaterials;
	TArray<UMaterialInstanceDynamic*> DynamicMaterials;
	AActor* m_CollidingActor = nullptr;
	UStaticMeshComponent* MeshComp = nullptr;

	void LightUpCollidingActor(AActor* CollidingActor);

	UFUNCTION(BlueprintImplementableEvent)
	void TL_TriggerActorLightup();

	UFUNCTION(BlueprintCallable)
	void FadeInOut(float TimeLine);

	//Reapplies original materials and resets a bunch of variables used in the process
	UFUNCTION(BlueprintCallable)
	void CleanUp();

	//End | Collision while changing world

	//For debugging. Press O to unlock all abilities in tutorial level
	void UnlockAllAbilities();
	
	//Camera focal effect during world change
	UFUNCTION(BlueprintImplementableEvent)
	void WorldTransitionEffects();

	//Camera shake and sound
	UFUNCTION(BlueprintImplementableEvent)
	void DeniedPhysicalReentryEffects();

	//All toggle functions related to world change

	void TogglePhysicalWorldActors();
	void ToggleSpiritWorldActors();
	void ToggleLastingCameraEffects();

	//For ParticleEffects that have a spiritworld and physicalworld component
	void ToggleParticleEffects();

	void TogglePhysicalSpiritActors();

	UFUNCTION(BlueprintImplementableEvent)
	void ToggleLandscapes();

	//Landscape variables are in the blueprint. Unreal would not allow BlueprintReadWriteable ALandscape*
	UFUNCTION(BlueprintImplementableEvent)
	void SetLandscapesReferences();

	

	//SensingSphereStuff

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundBase* SensingLightUpSound = nullptr;

	//Player only allowed to Sense again if all objects are done lighting up from last sensing
	int LitUpBySensing = 0;
	int GetLitUpBySensing() { return LitUpBySensing; }
	void IncrementLitUpBySensing() 
	{ 
		++LitUpBySensing;

		if (LitUpBySensing == 1)
		{
			UGameplayStatics::PlaySound2D(CurrentWorld, SensingLightUpSound);
		}

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


	//Start FoliageMeshes

	UMaterialInstanceDynamic* Tree1Bark = nullptr;
	UMaterialInstanceDynamic* Tree1Leaves = nullptr;

	UMaterialInstanceDynamic* Tree2Bark = nullptr;
	UMaterialInstanceDynamic* Tree2Leaves = nullptr;


	UMaterialParameterCollection* FoliageTrunk1Parameters = nullptr;
	UMaterialParameterCollection* FoliageTrunk2Parameters = nullptr;
	UMaterialParameterCollection* FoliageLeaves1Parameters = nullptr;
	UMaterialParameterCollection* FoliageLeaves2Parameters = nullptr;
	
	void SetFoliageMaterialParameterCollectionReferences();

	void ToggleFoliageMaterialProperties();

	float PhysicalTrunk[3]{ 1,0,0 };
	float SpiritTrunk1[3]{ 0,1,0 };
	float SpiritTrunk2[3]{ 0,0,1 };

	float PhysicalLeaves[4]{ 1,0,0,0 };
	float SpiritLeaves1[4]{ 0,1,0,0 };
	float SpiritLeaves2[4]{ 0,0,1,0 };
	float SpiritLeaves3[4]{ 0,0,0,1 };


	//End FoliageMeshes



	//Toggled on spirit enemies to give different looks in different worlds
	UPROPERTY(EditAnywhere, Category = EnemyMaterials)
	TArray<UMaterialInterface*> SpiritEnemyPhysicalWorldMaterials;
	UPROPERTY(EditAnywhere, Category = EnemyMaterials)
	TArray<UMaterialInterface*> SpiritEnemySpiritWorldMaterials;

	UPROPERTY(EditAnywhere, Category = EnemyMaterials)
		TArray<UMaterialInterface*> ShamanEnemyPhysicalWorldMaterials;
	UPROPERTY(EditAnywhere, Category = EnemyMaterials)
		TArray<UMaterialInterface*> ShamanEnemySpiritWorldMaterials;

};


