// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWorldActor.h"
#include "OurEnums.h"
#include "S_PickupShield.generated.h"

/**
 * 
 */
UCLASS()
class WORLDSWITCHING_API AS_PickupShield : public ASWorldActor
{
	GENERATED_BODY()

		virtual void Tick(float DeltaTime) override;
public:

	UFUNCTION(BlueprintImplementableEvent)
		void PickupFeedback();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PickupType)
		EPickupTypes PickupType;
};
