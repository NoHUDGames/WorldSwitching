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
	if (movementAnim_BlendSpace.Object)
	{
		MovementAnimBlendSpace = movementAnim_BlendSpace.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimationAsset> idle_Anim
	(TEXT("AnimSequence'/Game/Meshes/Characters/SpiritEnemy/Animations/Lil_Blub_Idle_V2.Lil_Blub_Idle_V2'"));
	if (idle_Anim.Object)
	{
		IdleAnim = idle_Anim.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimSequence> attack_Anim
	(TEXT("AnimSequence'/Game/Meshes/Characters/SpiritEnemy/Animations/Lil_Blub_Attack.Lil_Blub_Attack'"));
	if (attack_Anim.Object)
	{
		AttackAnim = attack_Anim.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimationAsset> walking_Anim
	(TEXT("AnimSequence'/Game/Meshes/Characters/SpiritEnemy/Animations/Lil_Blub_Walk.Lil_Blub_Walk'"));
	if (walking_Anim.Object)
	{
		WalkingAnim = walking_Anim.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimationAsset> takingDamage_Anim
	(TEXT("AnimSequence'/Game/Meshes/Characters/SpiritEnemy/Animations/Lil_Blub_Damage.Lil_Blub_Damage'"));
	if (takingDamage_Anim.Object)
	{
		TakingDamageAnim = takingDamage_Anim.Object;
	}
	/// finished setting up animation variables


	/// Creates and sets up the necessary settings for the enemy head that shows up after the enemy dies
	HeadAfterDeath = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EnemyHead"));
	HeadAfterDeath->SetupAttachment(RootComponent);
	HeadAfterDeath->Mobility = EComponentMobility::Movable;
	HeadAfterDeath->SetHiddenInGame(true);
	HeadAfterDeath->SetSimulatePhysics(false);
	/// Finished setting up settings for enemy head that shows up after the enemy dies
	
	/// creates and sets up the settings for the blue smoke that starts playing on enemy death
	BlueSmoke = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("BlueSmokeOnDeath"));
	BlueSmoke->SetupAttachment(RootComponent);

	/// creates and sets up the settings for the fireflies particle system that plays when the player is in the physical world
	FireFlies = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("FireFliesInPhysicalWorld"));
	FireFlies->SetupAttachment(RootComponent);

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

	///Attaches the weapon collider to the right socket
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

/// Function that starts any appropriate animations
/// it is called every tick
void ASpiritTest::PlayingAnimations()
{

	/// The animation started array is a set of bool variables that checks whether or not an animation is started
	/// It is implemented to ensure that an animation isn't started when it is already running
	/// the RunningAnimations enum is checking what animation is allowed to run. 
	/// This variable is given different values based on actions the player make
	if (AnimationStarted[0] == false && RunningAnimations == EAnimations::TAKINGDAMAGE)
	{
		GetMesh()->PlayAnimation(TakingDamageAnim, false);

		ChangingAnimationStarted(0);

		/// The waiting time set for when the function should run is based on the duration of the animation
		GetWorldTimerManager().SetTimer(TakingDamageTimerHandler, this, &ASpiritTest::TurnOffTakingDamageAnim, 0.766667f, false);
	}
	else if (AnimationStarted[1] == false && RunningAnimations == EAnimations::ATTACKING)
	{
		GetMesh()->PlayAnimation(AttackAnim, false);

		ChangingAnimationStarted(1);
	}
	else if (AnimationStarted[2] == false && RunningAnimations == EAnimations::MOVEMENT)
	{
		GetMesh()->PlayAnimation(MovementAnimBlendSpace, true);

		ChangingAnimationStarted(2);
	}
}

///This function changes the AnimationStarted array. Only one index should be true at all times, which is the index parameters value
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


///This tests whether or not the enemy is ready to be killed
/// it first sets the enemy to be hidden and then start a timer that spawns the enemy head
/// after a while i destroys the entire actor
void ASpiritTest::KillingEnemy()
{
	if (Lives <= 0)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), DeathSound);

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

///This function sets the enemy head to be visible in the game, makes it drop to the ground and allows the player to kick it around
void ASpiritTest::SpawnHead()
{	
	HeadAfterDeath->SetHiddenInGame(false);
	HeadAfterDeath->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	HeadAfterDeath->SetSimulatePhysics(true);

	BlueSmoke->Deactivate();
}

///This function kills the actor. 
///Since we can't call the Destroy() directly through the SetTimer function, we ahd to create our own and call that function instead
void ASpiritTest::DestroyActor()
{
	if (APawn::Controller == nullptr)
	{
		Destroy();
	}
		

}

/// The function that turns off the takingDamage anim and allows the player to move again
void ASpiritTest::TurnOffTakingDamageAnim()
{
	RunningAnimations = EAnimations::MOVEMENT;

	AnimationStarted[4] = false;
}

///Runs the knockback effect. it is called through the DecrementingLives function
///The parameter is a result of the angle that the player is hitting the enemy from and the knockbackForce variable in the BP_Character class
void ASpiritTest::KnockbackEffect(FVector KnockbackDirection)
{
	StartKnockbackLocation = GetActorLocation();
	EndKnockbackLocation = StartKnockbackLocation + KnockbackDirection;
	KnockbackTimeline->PlayFromStart();
}

///Just like the MovementAnimationTesting in BP_Character class, this function determines what animation should be run in the BlendSpace
/// It is called in tick
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

///The function that determines where the player is during the knockback effect
void ASpiritTest::KnockbackTimelineFloatReturn(float value)
{
	SetActorLocation(FMath::Lerp(StartKnockbackLocation, EndKnockbackLocation, value));
}


///decrements the health of the enemy when the player hits it
void ASpiritTest::DecrementingLives(FVector KnockbackDirection)
{
	UGameplayStatics::PlaySound2D(GetWorld(), TakeDamageSound);

	if (Lives > 0)
	{
		--Lives;

		RunningAnimations = EAnimations::TAKINGDAMAGE;
		KnockbackEffect(KnockbackDirection);
	}
	KillingEnemy();	
	
}

///This is a part of the special ability for the spirit enemy
///It is called through the AI system for the spirit enemy
///The function slowly decrements the health of the player
void ASpiritTest::DamageOverTimeAttack()
{
	if (bCanPerformNextDamageOverTime == true)
	{
		bCanPerformNextDamageOverTime = false;
		GetWorldTimerManager().SetTimer(DamageOverTimeHandler, this, &ASpiritTest::CallingPlayerDecrementLivesFunction, DamageOverTimeCooldown, false);
	}
}

///Since the SetTimer can't be stopped based on things that happen after the timer starts, we can't directly call the DecrementingLives function
///This function therefore tests whether or not the player has been able to break the special ability. If not, the function decrements the players health
void ASpiritTest::CallingPlayerDecrementLivesFunction()
{
	ABP_Character * PlayerCharacter = Cast<ABP_Character>(GetWorld()->GetFirstPlayerController()->GetPawn());

	if (PlayerCharacter->bIsSpiritWorld == false && PlayerCharacter->Lives > 0)
	{
		PlayerCharacter->DecrementingLives();
	}
	
	bCanPerformNextDamageOverTime = true;
	
}

///The function for collision testing, checking whether or not the enemy has been able to hit the player
void ASpiritTest::HittingPlayer(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor->IsA(ABP_Character::StaticClass()))
	{
		ABP_Character* PlayerCharacter = Cast<ABP_Character>(OtherActor);

		PlayerCharacter->DecrementingLives();

		///Turns off  the weaponCollider if the enemy hits the player one time
		BoxCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}


