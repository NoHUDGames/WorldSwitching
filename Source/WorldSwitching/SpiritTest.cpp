// Fill out your copyright notice in the Description page of Project Settings.

#include "SpiritTest.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "BP_Character.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"
#include "Runtime/Engine/Classes/Animation/AnimSingleNodeInstance.h"
#include "Engine/Engine.h"


// Sets default values
ASpiritTest::ASpiritTest()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/// Weapon collider, the collision sphere that damages the player when using the weapon
	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponCollider"));
	BoxCollider->SetupAttachment(GetMesh());


	/// Setting up animation variables

	static ConstructorHelpers::FObjectFinder<UBlendSpace1D> movementAnim_BlendSpace
	(TEXT("BlendSpace1D'/Game/Meshes/Characters/SpiritEnemy/Animations/SpiritMovementBlendSpace.SpiritMovementBlendSpace'"));
	MovementAnimBlendSpace = movementAnim_BlendSpace.Object;


	static ConstructorHelpers::FObjectFinder<UAnimationAsset> idle_Anim
	(TEXT("AnimSequence'/Game/Meshes/Characters/SpiritEnemy/Animations/Lil_Blub_Idle_V2.Lil_Blub_Idle_V2'"));
	if (idle_Anim.Object)
	{
		IdleAnim = idle_Anim.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimSequence> attack_Anim
	(TEXT("AnimSequence'/Game/Meshes/Characters/SpiritEnemy/Animations/Lil_Blub_Attack.Lil_Blub_Attack'"));
	AttackAnim = attack_Anim.Object;

	static ConstructorHelpers::FObjectFinder<UAnimationAsset> walking_Anim
	(TEXT("AnimSequence'/Game/Meshes/Characters/SpiritEnemy/Animations/Lil_Blub_Walk.Lil_Blub_Walk'"));
	WalkingAnim = walking_Anim.Object;

	static ConstructorHelpers::FObjectFinder<UAnimationAsset> takingDamage_Anim
	(TEXT("AnimSequence'/Game/Meshes/Characters/SpiritEnemy/Animations/Lil_Blub_Damage.Lil_Blub_Damage'"));
	TakingDamageAnim = takingDamage_Anim.Object;
	/// finished setting up animation variables


	HeadAfterDeath = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EnemyHead"));
	HeadAfterDeath->SetupAttachment(RootComponent);
	HeadAfterDeath->Mobility = EComponentMobility::Movable;
	HeadAfterDeath->SetHiddenInGame(true);
	HeadAfterDeath->SetSimulatePhysics(false);
	
	BlueSmoke = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("BlueSmokeOnDeath"));
	BlueSmoke->SetupAttachment(RootComponent);

	/// Sets up the timeline for knockback effect
	KnockbackTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("TimelineForKnockbackEffect"));

	InterpKnockbackFunction.BindUFunction(this, FName("KnockbackTimelineFloatReturn"));
	/// finished setting up the timeline for knockback effect
}

// Called when the game starts or when spawned
void ASpiritTest::BeginPlay()
{
	Super::BeginPlay();

	BoxCollider->OnComponentBeginOverlap.AddDynamic(this, &ASpiritTest::HittingPlayer);

	FName SocketName = TEXT("WeaponRotation");
	BoxCollider->AttachToComponent(GetMesh(), 
		FAttachmentTransformRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepRelative, true),
		SocketName);

	/// Sets up the BeginPlay values for the knockback timeline
	if (fKnockbackCurve)
	{
		/// Add the float curve to the timeline and connect it to the interpfunctions delegate
		KnockbackTimeline->AddInterpFloat(fKnockbackCurve, InterpKnockbackFunction, FName("Alpha"));

		/// Setting our timeline settings before we start it
		KnockbackTimeline->SetLooping(false);
		KnockbackTimeline->SetIgnoreTimeDilation(true);

	}
	/// Done setting up the BeginPlay values for the knockback timeline
}

// Called every frame
void ASpiritTest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	MovementAnimationTesting();
	PlayingAnimations();
	
}

void ASpiritTest::PlayingAnimations()
{

	
	if (AnimationStarted[0] == false && RunningAnimations == EAnimations::TAKINGDAMAGE)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Running TakingDamageAnim"));
		GetMesh()->PlayAnimation(TakingDamageAnim, false);

		ChangingAnimationStarted(0);

		/// The waiting time set for when the function should run is based on the duration of the animation
		GetWorldTimerManager().SetTimer(TakingDamageTimerHandler, this, &ASpiritTest::TurnOffTakingDamageAnim, 0.766667f, false);
	}
	else if (AnimationStarted[1] == false && RunningAnimations == EAnimations::ATTACKING)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("AttackAnim"));
		GetMesh()->PlayAnimation(AttackAnim, false);

		ChangingAnimationStarted(1);
	}
	else if (AnimationStarted[2] == false && RunningAnimations == EAnimations::MOVEMENT)
	{
		GetMesh()->PlayAnimation(MovementAnimBlendSpace, true);

		ChangingAnimationStarted(2);
	}
}

void ASpiritTest::ChangingAnimationStarted(int index)
{
	AnimationStarted[index] = true;
	for (int i{ 0 }; i < 3; ++i)
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

		GetMesh()->SetHiddenInGame(true);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		BoxCollider->DestroyComponent(true);

		/// Kills off the controller of the pawn
		GetController()->Destroy();		

		BlueSmoke->Activate();

		GetWorldTimerManager().SetTimer(SpawningHeadTimerHandler, this, &ASpiritTest::SpawnHead, 1.f, false);
		
		GetWorldTimerManager().SetTimer(DestroyingActorTimerHandler, this, &ASpiritTest::DestroyActor, 3.f, false);
	}
}

void ASpiritTest::SpawnHead()
{	
	HeadAfterDeath->SetHiddenInGame(false);
	HeadAfterDeath->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	HeadAfterDeath->SetSimulatePhysics(true);

	BlueSmoke->Deactivate();
}

void ASpiritTest::DestroyActor()
{
	if (APawn::Controller == nullptr)
	{
		Destroy();
	}
		

}

void ASpiritTest::TurnOffTakingDamageAnim()
{
	RunningAnimations = EAnimations::MOVEMENT;

	AnimationStarted[4] = false;
}

void ASpiritTest::KnockbackEffect(FVector KnockbackDirection)
{
	StartKnockbackLocation = GetActorLocation();
	EndKnockbackLocation = StartKnockbackLocation + KnockbackDirection;
	KnockbackTimeline->PlayFromStart();
}

void ASpiritTest::MovementAnimationTesting()
{
	/// SetBlendSpaceInput faar spillet til aa krasje av en eller annen fucka grunn. 
	/// Gjor akkurat det samme i spillerklassen og der funekr det
	if (RunningAnimations == EAnimations::MOVEMENT)
	{
		FVector BlendParam(GetVelocity().Size(), 0.f, 0.f);
		if (GetMesh() != nullptr && GetMesh()->GetSingleNodeInstance() != nullptr)
			GetMesh()->GetSingleNodeInstance()->SetBlendSpaceInput(BlendParam);
	}
	

}

void ASpiritTest::KnockbackTimelineFloatReturn(float value)
{
	SetActorLocation(FMath::Lerp(StartKnockbackLocation, EndKnockbackLocation, value));
}

void ASpiritTest::DecrementingLives(FVector KnockbackDirection)
{
	if (Lives > 0)
	{
		--Lives;
		UE_LOG(LogTemp, Warning, TEXT("Enemy has %i lives left"), Lives)

		RunningAnimations = EAnimations::TAKINGDAMAGE;
		KnockbackEffect(KnockbackDirection);
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
	}
}


