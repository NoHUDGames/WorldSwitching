// Fill out your copyright notice in the Description page of Project Settings.

#include "SpiritTest.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "BP_Character.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
ASpiritTest::ASpiritTest()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponRotation = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponRotation"));
	WeaponRotation->SetupAttachment(RootComponent);
	WeaponRotation->Mobility = EComponentMobility::Movable;

	WeaponVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TempWeapon"));
	WeaponVisual->SetupAttachment(WeaponRotation);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> WeaponVisualAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (WeaponVisualAsset.Succeeded())
	{
		/// WeaponVisual->bVisualizeComponent = true;
		WeaponVisual->SetStaticMesh(WeaponVisualAsset.Object);
		WeaponVisual->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));
		WeaponVisual->Mobility = EComponentMobility::Movable;
		WeaponVisual->SetWorldScale3D(FVector(0.2f));
	}

	/// Weapon collider, the collision sphere that damages the player when using the weapon
	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponCollider"));
	BoxCollider->SetupAttachment(WeaponVisual);

	/// Setting up animation variables
	static ConstructorHelpers::FObjectFinder<UAnimationAsset> idle_Anim
	(TEXT("AnimSequence'/Game/Meshes/Characters/SpiritEnemy/Animations/Lil_Blub_Idle_V2.Lil_Blub_Idle_V2'"));
	if (idle_Anim.Object)
	{
		IdleAnim = idle_Anim.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimationAsset> attack_Anim
	(TEXT("AnimSequence'/Game/Meshes/Characters/SpiritEnemy/Animations/Lil_Blub_Attack.Lil_Blub_Attack'"));
	AttackAnim = attack_Anim.Object;

	static ConstructorHelpers::FObjectFinder<UAnimationAsset> walking_Anim
	(TEXT("AnimSequence'/Game/Meshes/Characters/SpiritEnemy/Animations/Lil_Blub_Walk.Lil_Blub_Walk'"));
	WalkingAnim = walking_Anim.Object;
	/// finished setting up animation variables

}

// Called when the game starts or when spawned
void ASpiritTest::BeginPlay()
{
	Super::BeginPlay();

	BoxCollider->OnComponentBeginOverlap.AddDynamic(this, &ASpiritTest::HittingPlayer);
	
}

// Called every frame
void ASpiritTest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PlayingAnimations();

}

void ASpiritTest::PlayingAnimations()
{

	if (AnimationStarted[0] == false && RunningAnimations == EAnimations::IDLE && GetVelocity() == FVector(0.f, 0.f, 0.f))
	{

		GetMesh()->PlayAnimation(IdleAnim, true);

		ChangingAnimationStarted(0);

	}
	else if (AnimationStarted[1] == false && RunningAnimations == EAnimations::ATTACKING)
	{

		GetMesh()->PlayAnimation(AttackAnim, false);

		ChangingAnimationStarted(1);
	}
	else if (AnimationStarted[2] == false && RunningAnimations == EAnimations::WALKINGFORWARD)
	{
		GetMesh()->PlayAnimation(WalkingAnim, true);

		ChangingAnimationStarted(2);
	}
	else if (AnimationStarted[3] == false && RunningAnimations == EAnimations::DYING)
	{
		GetMesh()->PlayAnimation(WalkingAnim, true);

		ChangingAnimationStarted(3);

	}
}

void ASpiritTest::ChangingAnimationStarted(int index)
{
	AnimationStarted[index] = true;
	for (int i{ 0 }; i < 4; ++i)
	{
		if (i != index)
		{
			AnimationStarted[i] = false;
		}
	}
}

// Called to bind functionality to input
void ASpiritTest::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}



void ASpiritTest::KillingEnemy()
{
	if (Lives <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Enemy is dying!"))
			Destroy();
	}
}

void ASpiritTest::DecrementingLives()
{
	if (Lives > 0)
	{
		--Lives;
		UE_LOG(LogTemp, Warning, TEXT("Enemy has %i lives left"), Lives)
		
	}
	KillingEnemy();	
}

void ASpiritTest::DamageOverTimeAttack()
{
	if (bCanPerformNextDamageOverTime == true)
	{
		bCanPerformNextDamageOverTime = false;
		GetWorldTimerManager().SetTimer(DamageOverTimeHandler, this, &ASpiritTest::CallingPlayerDecrementLivesFunction, DamageOverTimeCooldown, false);
	}
}

void ASpiritTest::CallingPlayerDecrementLivesFunction()
{
	ABP_Character * PlayerCharacter = Cast<ABP_Character>(GetWorld()->GetFirstPlayerController()->GetPawn());

	PlayerCharacter->DecrementingLives();
	bCanPerformNextDamageOverTime = true;
	
}

void ASpiritTest::HittingPlayer(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor->IsA(ABP_Character::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("The enemy are hitting you"))

			ABP_Character* PlayerCharacter = Cast<ABP_Character>(OtherActor);

		PlayerCharacter->DecrementingLives();

		BoxCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		if (PlayerCharacter->Lives <= 0)
		{
			PlayerCharacter->DeathSequence();
		}
	}
}


