// Fill out your copyright notice in the Description page of Project Settings.

#include "PShamanEnemy.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"

APShamanEnemy::APShamanEnemy()
{
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

	
	static ConstructorHelpers::FObjectFinder<UClass> SpiritBlueprint(TEXT("Blueprint'/Game/Blueprints/BP_SpiritTest.BP_SpiritTest_C'"));
	if (SpiritBlueprint.Object) {
		SpiritOfShaman = SpiritBlueprint.Object;
	}
	
	/// Setting up animation variables
	static ConstructorHelpers::FObjectFinder<UAnimationAsset> idle_Anim
	(TEXT("AnimSequence'/Game/Meshes/Characters/SpiritEnemy/Animations/Lil_Blub_Idle_V2.Lil_Blub_Idle_V2'"));
	if (idle_Anim.Object)
	{
		IdleAnim = idle_Anim.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimationAsset> attack_Anim
	(TEXT("AnimSequence'/Game/Meshes/Characters/SpiritEnemy/Animations/Lil_Blub_Idle.Lil_Blub_Idle'"));
	AttackAnim = attack_Anim.Object;

	static ConstructorHelpers::FObjectFinder<UAnimationAsset> walking_Anim
	(TEXT("AnimSequence'/Game/Meshes/Characters/SpiritEnemy/Animations/Lil_Blub_Walk.Lil_Blub_Walk'"));
	WalkingAnim = walking_Anim.Object;
	/// finished setting up animation variables

}

void APShamanEnemy::BeginPlay()
{
	Super::BeginPlay();

	BoxCollider->OnComponentBeginOverlap.AddDynamic(this, &APShamanEnemy::HittingPlayer);
}

void APShamanEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PlayingAnimations();

	
}

void APShamanEnemy::PlayingAnimations()
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

void APShamanEnemy::ChangingAnimationStarted(int index)
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


void APShamanEnemy::SetupPlayerInputComponent(UInputComponent * PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void APShamanEnemy::DecrementingLives()
{
	if (Lives > 0)
	{
		--Lives;
		UE_LOG(LogTemp, Warning, TEXT("Enemy has %i lives left"), Lives)

	}
	KillingEnemy();
}

void APShamanEnemy::KillingEnemy()
{
	if (Lives <= 0)
	{
		FVector DeathLocation = GetActorLocation() + FVector(70.f, 70.f, 0.f);
		FRotator DeathRotation = GetActorRotation();
		UE_LOG(LogTemp, Warning, TEXT("Enemy is dying!"))
		
		ASpiritTest* NewSpiritEnemy = GetWorld()->SpawnActor<ASpiritTest>(SpiritOfShaman, DeathLocation, DeathRotation);
		NewSpiritEnemy->SetActorHiddenInGame(true);
		NewSpiritEnemy->SpawnDefaultController();
		
		Destroy();
		
	}
}


void APShamanEnemy::HittingPlayer(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, 
	UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
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


	
