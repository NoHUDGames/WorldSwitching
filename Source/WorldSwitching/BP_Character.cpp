// Fill out your copyright notice in the Description page of Project Settings.

#include "BP_Character.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "SpiritTest.h"
#include "PShamanEnemy.h"
#include "Altar.h"
#include "Artifacts.h"
#include "S_PickupShield.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"
#include "Components/TimelineComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "WorldSwitchingGameModeBase.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimSingleNodeInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "PS_Portal.h"

// Sets default values
ABP_Character::ABP_Character()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Head = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CharacterHead"));
	Head->SetupAttachment(GetMesh());
	Head->SetMobility(EComponentMobility::Movable);

	Mask = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CharacterMask"));
	Mask->SetupAttachment(GetMesh());
	Mask->SetMobility(EComponentMobility::Movable);


	/// kicking collider, the collision sphere that damages the enemies when kicking
	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("FootCollider"));
	BoxCollider->SetupAttachment(GetMesh());
	BoxCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	RespawnLocation = { 0.f, 0.f, 0.f };
	NumberOfHoldingArtifacts = 0;


	/// Sets up the timeline for dashing
	DashingTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("TimelineForDashing"));

	InterpDashingFunction.BindUFunction(this, FName("DashingTimelineFloatReturn"));
	DashingTimelineFinished.BindUFunction(this, FName("OnDashingTimelineFinished"));

	/// Finished setting up the timeline for dashing


	/// Setting up animation variables


	static ConstructorHelpers::FObjectFinder<UAnimationAsset> kicking_Anim
	(TEXT("AnimSequence'/Game/Meshes/Characters/PlayerCharacter/Animations/Player_Kick_1.Player_Kick_1'"));
	KickingAnim = kicking_Anim.Object;

	static ConstructorHelpers::FObjectFinder<UAnimationAsset> death_Anim
	(TEXT("AnimSequence'/Game/Meshes/Characters/PlayerCharacter/Animations/Player_Death.Player_Death'"));
	DyingAnim = death_Anim.Object;

	static ConstructorHelpers::FObjectFinder<UAnimationAsset> dashing_Anim
	(TEXT("AnimSequence'/Game/Meshes/Characters/PlayerCharacter/Animations/Player_Dash.Player_Dash'"));
	DashingAnim = dashing_Anim.Object;

	static ConstructorHelpers::FObjectFinder<UBlendSpace> movement_BlendSpace
	(TEXT("BlendSpace'/Game/Meshes/Characters/PlayerCharacter/Animations/BS_Movement.BS_Movement'"));
	MovementAnimBlendSpace = movement_BlendSpace.Object;

	static ConstructorHelpers::FObjectFinder<UAnimationAsset> idle_Anim
	(TEXT("AnimSequence'/Game/Meshes/Characters/PlayerCharacter/Animations/Player_Idle.Player_Idle'"));
	IdleAnim = idle_Anim.Object;

	static ConstructorHelpers::FObjectFinder<UAnimationAsset> BeingHitAnim_Anim
	(TEXT("AnimSequence'/Game/Meshes/Characters/PlayerCharacter/Animations/Main_BeingHit.Main_BeingHit'"));
	BeingHitAnim = BeingHitAnim_Anim.Object;
	
	/// finished setting up animation variables


	/// Sets up the timeline for floating head
	FloatingHeadTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("TimelineForFloatingHead"));

	InterpHeadFloatingFunction.BindUFunction(this, FName("FloatingHeadTimelineFloatReturn"));
	HeadFloatingTimelineFinished.BindUFunction(this, FName("OnFloatingHeadTimelineFinished"));

	HeadFloatOffset = 20.f;
	/// finished setting up the timeline for floating head

	DeathSmoke = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("BlueSmokeOnDeath"));
	DeathSmoke->SetupAttachment(RootComponent);
	

	
}

// Called when the game starts or when spawned
void ABP_Character::BeginPlay()
{
	Super::BeginPlay();

	CurrentWorld = GetWorld();

	BoxCollider->OnComponentBeginOverlap.AddDynamic(this, &ABP_Character::HittingEnemy);

	/// Tests if the player overlaps with an artifact
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ABP_Character::PickingUpArtifacts);

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ABP_Character::DeliveringArtifacts);
	
	GameModeRef = Cast<AWorldSwitchingGameModeBase>(GetWorld()->GetAuthGameMode());

	RespawnLocation = GetActorLocation();

	BoxCollider->AttachToComponent(GetMesh(),
		FAttachmentTransformRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepRelative, true),
		FName("KickingRotation"));

	/// Sets up the BeginPlay values for the dashing timeline
	if (fDashingCurve)
	{
		/// Add the float curve to the timeline and connect it to the interpfunctions delegate
		DashingTimeline->AddInterpFloat(fDashingCurve, InterpDashingFunction, FName("Alpha"));
		//Add our timeline finished function
		DashingTimeline->SetTimelineFinishedFunc(DashingTimelineFinished);

		/// Setting our timeline settings before we start it
		DashingTimeline->SetLooping(false);
		DashingTimeline->SetIgnoreTimeDilation(true);

	}
	/// Done setting up the BeginPlay values for the dashing timeline

	/// Setting up BeginPlay values for world switching
	/// Sets up the BeginPlay values for the floating head timeline
	if (fFloatingHeadCurve)
	{
		FloatingHeadTimeline->AddInterpFloat(fFloatingHeadCurve, InterpHeadFloatingFunction, FName("Alpha"));

		FloatingHeadTimeline->SetLooping(true);
		FloatingHeadTimeline->SetIgnoreTimeDilation(true);

		if (bIsSpiritWorld)
		{
			Mask->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, false), FName("HeadSocket"));
		}
		else
		{
			Head->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, false), FName("HeadSocket"));
		}

		TrailingHeadGoalLocation = TrailingHeadLocation + FVector(0.f, 0.f, HeadFloatOffset);
		
	}
	///Finished setting up the BeginPlay values for the floating head timeline


	/// Makes sure that the right object is placed at the location of the head socket
	/// If the player is currently in the spiritworld when it is created, the mask is at the head socket location
	/// if the player is currently in the physical world when the actor is created, the head is at the head socket location
	if (bIsSpiritWorld)
	{
		Mask->SetRelativeLocation(HeadSocketLocation);
		Head->SetRelativeLocation(TrailingHeadLocation);
	}
	else
	{
		Mask->SetRelativeLocation(TrailingHeadLocation);
		Head->SetRelativeLocation(HeadSocketLocation);
	}

	/// Finished setting up the BeginPlay values for world switching
	
	/// Gets the Game instance. We use this class to store game-wide information that is only reset when the player starts a new game
	GameInstance = Cast<UWorldSwitchingGameInstance>(GetWorld()->GetGameInstance());

	/// Gets the first player controller
	PlayerController = Cast<AOurPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	/// We are manipulating the player speed through the spirit enemy special ability. 
	/// Therefore, we make sure that the MaxWalkSpeed is always 600.f when the character is spawned in the game
	if (GetCharacterMovement()->GetMaxSpeed() != NormalSpeed)
	{
		GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	}

	/// Adds the artifact widget to the Viewport. This displays the amount of artifacts the player has
	/// It is hidden for the player at all times, except for when the lpayer picks up an artifact
	/// The actual class has a function that moves out to the visible area of the player screen when an artifact is picked up
	ArtifactDisplay = CreateWidget<UUserWidget>(GameInstance, ArtifactWidget);
	ArtifactDisplay->AddToViewport();

	/// Makes sure that the DeathSmoke particle effect is deactivated when the character is spawned into the game
	DeathSmoke->Deactivate();
}

// Called every frame
void ABP_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PlayingAnimations();
	
	FloatingHeadTimeline->Play();
}

/// A virtual function inherited from the character class
/// it is called each time the character changes movement mode
void ABP_Character::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	
	///Checks if the new movement mode is falling. If it is, we find the time where the player started to fall
	/// When the player starts to walk again, we get the time again and calculate the fall duration
	/// The fall duration function determines whether or not the player should die from the fall
	if (GetCharacterMovement()->GetMovementName() == "Falling")
	{
		startedFalling = UGameplayStatics::GetRealTimeSeconds(GetWorld());
	}
	else if (GetCharacterMovement()->GetMovementName() == "Walking")
	{
		endedFalling = UGameplayStatics::GetRealTimeSeconds(GetWorld());	

		CalculateFallDuration();
	}
}

void ABP_Character::CalculateFallDuration()
{
	float TimeFalled = endedFalling - startedFalling;

	/// If the player falls longer than the allowed time, it will die
	/// it is only calculated after the player is done falling.
	/// If the player somehow falls through the ground or something, they can kill themselves in the pause menu instead
	if (TimeFalled > FallDurationForDeath)
	{

		DeathSequence(false);
	}
}

/// Function that starts any appropriate animations
/// it is called every tick
void ABP_Character::PlayingAnimations()
{
	/// The animation started array is a set of bool variables that checks whether or not an animation is started
	/// It is implemented to ensure that an animation isn't started when it is already running
	/// the RunningAnimations enum is checking what animation is allowed to run. 
	/// This variable is given different values based on actions the player make
	if (AnimationStarted[1] == false && RunningAnimations == EAnimations::ATTACKING)
	{
		GetMesh()->PlayAnimation(KickingAnim, false);

		ChangingAnimationStarted(1);
	}
	else if (AnimationStarted[0] == false && RunningAnimations == EAnimations::MOVEMENT)
	{
		GetMesh()->PlayAnimation(MovementAnimBlendSpace, true);

		ChangingAnimationStarted(0);
	}
	else if (AnimationStarted[2] == false && RunningAnimations == EAnimations::DASHING)
	{
			GetMesh()->PlayAnimation(DashingAnim, false);

		ChangingAnimationStarted(2);
	}
	else if (AnimationStarted[3] == false && RunningAnimations == EAnimations::DYING)
	{
			GetMesh()->PlayAnimation(DyingAnim, false);

		ChangingAnimationStarted(3);
	}
	else if (AnimationStarted[4] == false && RunningAnimations == EAnimations::IDLE)
	{
		/// The idle animation is usually played through MovementAnimBlendSpace,
		/// but the blendspace didn't work properly when you ride an elevator
		GetMesh()->PlayAnimation(IdleAnim, true);

		ChangingAnimationStarted(4);
	}
	else if (AnimationStarted[5] == false && RunningAnimations == EAnimations::TAKINGDAMAGE)
	{
		GetMesh()->PlayAnimation(BeingHitAnim, false);

		ChangingAnimationStarted(5);
	}
	
}

///This function changes the AnimationStarted array. Only one index should be true at all times, which is the index parameters value
void ABP_Character::ChangingAnimationStarted(int index)
{
	AnimationStarted[index] = true;
	for (int i{ 0 }; i < 6; ++i)
	{
		if (i != index)
		{
			AnimationStarted[i] = false;
		}
	}
}

// Called to bind functionality to input
void ABP_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveUp", this, &ABP_Character::MoveUp);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABP_Character::MoveRight);
	PlayerInputComponent->BindAction("Kicking", IE_Pressed, this, &ABP_Character::Kicking);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ABP_Character::Interact);
	PlayerInputComponent->BindAction("Interact", IE_Released, this, &ABP_Character::StopInteracting);
	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &ABP_Character::Dashing);
	PlayerInputComponent->BindAction("SenseWorld", IE_Pressed, this, &ABP_Character::SenseWorld);

}

///The move up and down function (always along the x axis with our current camera settings)
void ABP_Character::MoveUp(float AxisValue)
{
	AddMovementInput(FVector(50.f, 0.f, 0.f), AxisValue);
	
	MovementAnimationTesting(AxisValue);
}

///The sideways function (always along the y axis with our current camera settings)
void ABP_Character::MoveRight(float AxisValue)
{
	AddMovementInput(FVector(0.f, 50.f, 0.f), AxisValue);

	MovementAnimationTesting(AxisValue);
}


void ABP_Character::ResetArtifactDisplayValues()
{
	LastTimePickedUpArtifact = UGameplayStatics::GetRealTimeSeconds(GetWorld());
	PickedUpArtifact = false;
}

/// This function makes sure that the right animation is running in the movement blend space
/// it is called in both movement functions
void ABP_Character::MovementAnimationTesting(float AxisValue)
{

	if (AxisValue != 0 && !CurrentlyKicking && RunningAnimations != EAnimations::DASHING)
	{
		RunningAnimations = EAnimations::MOVEMENT;
	}
	
	float rotationDegreeAngle = 
		GetMesh()->GetSingleNodeInstance()->CalculateDirection(GetVelocity(), GetActorRotation());

	/// Parameters that determines the blend properties for the Movement animation
	/// X-parameter defines the direction you move, so if the character should strafe or walk forward
	/// Y-Parameter defines the speed of the character, so whether or not the idle animation should run
	FVector BlendParams(rotationDegreeAngle, GetVelocity().Size(), 0.f);

	GetMesh()->GetSingleNodeInstance()->SetBlendSpaceInput(BlendParams);

}

/// This is the main function for kicking
/// We have implemented a kicking combo functionality here (through the use of the NumberOfKicks variable), 
/// but since we didn't get any animation that gave the player enough feedback, we decided to leave it commented out in the game
/// It is bound to the left mouse button
void ABP_Character::Kicking()
{
	RunningAnimations = EAnimations::ATTACKING;

	if (!CurrentlyKicking)
	{
		/// Plays the kicking sound
		UGameplayStatics::PlaySound2D(CurrentWorld, KickingSoundDefault);

		/// Makes it impossible for the player to kick while it's already kicking
		CurrentlyKicking = true;

		if (NumberOfKicks <= 2)
		{
			++NumberOfKicks;
			/// Resets the kick combo after 1.5 seconds
			GetWorldTimerManager().SetTimer(ComboDurationTimer, this, &ABP_Character::ResetKickingCombo, 1.5f, false);
		}
		else
		{
			/// Resets the kick combo if the player has reached 3 kicks
			ResetKickingCombo();
		}

		/// Turns on overlapping with other pawns for the kick box collider
		BoxCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		if (bIsSpiritWorld)
		{
			BoxCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Overlap);
		}
		else
		{
			BoxCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Overlap);
		}

		///This determines the kicking cooldown, which is on 0.6f seconds
		///It is also the function that finished off the kick and turns the collision settings off for the BoxCollider
		GetWorldTimerManager().SetTimer(KickingDurationTimer, this, &ABP_Character::KickingFinished, 0.6f, false);
	}
	
}

void ABP_Character::ResetKickingCombo()
{
	if (NumberOfKicks != 0)
	{
		NumberOfKicks = 0;
	}	
}

/// The function that resets the kicking, turns it off and allows the player to kick again
void ABP_Character::KickingFinished()
{
	CurrentlyKicking = false;
	RunningAnimations = EAnimations::MOVEMENT;
	BoxCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BoxCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
	BoxCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
}

/// This is the function that allows the player to control the character again after being hit by an enemy
void ABP_Character::TakingHitAnimationOver()
{

	RunningAnimations = EAnimations::MOVEMENT;
	EnableInput(GetWorld()->GetFirstPlayerController());
	
	/// Had some issues where this value kept on staying true if the player tried to kick and got hit at the same time
	/// This somehow fixed the issue
	if (CurrentlyKicking)
	{
		CurrentlyKicking = false;
	}
}

/// The interact function. This is used to drop off artifacts and turn on torches
/// it could also be used to perform other similar actions if we were to expand the game (Like open a door etc.)
/// it is bound to the E key
void ABP_Character::Interact()
{
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel4, ECollisionResponse::ECR_Overlap);
}

/// Stops the interacting
/// it is bound to releasing the the E key
void ABP_Character::StopInteracting()
{
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel4, ECollisionResponse::ECR_Ignore);
}

///This decrement the health of the player. 
/// It also runs a test for how much life the player has
/// if the player has less than or equal to 0 lives, it will run the function that makes sure the player dies
void ABP_Character::DecrementingLives()
{
	UGameplayStatics::PlayWorldCameraShake(GetWorld(), TakingDamageCameraShake, GetActorLocation(), 10.f, 1500.f);
	UGameplayStatics::PlaySound2D(CurrentWorld, TakeDamageSound);
	DisableInput(GetWorld()->GetFirstPlayerController());

	if (GetShields() > 0)
	{
		--NumberOfShields;
	}
	else
	{
		--Lives;

		if (Lives <= 0)
		{
			CurrentlyDying();
		}
		else
		{

			RunningAnimations = EAnimations::TAKINGDAMAGE;
			GetWorldTimerManager().SetTimer(KickingDurationTimer, this, &ABP_Character::TakingHitAnimationOver, 0.45f, false);

			
		}
	}
}

/// This function is the first step in the player death sequence of functions
/// it starts the dying animation and queues a couple of other functions that are about to happen
void ABP_Character::CurrentlyDying()
{
	RunningAnimations = EAnimations::DYING;
	GetWorldTimerManager().SetTimer(ActivatingDeathSmokeTimer, this, &ABP_Character::ActivateDeathSmoke, 1.f, false);
	GetWorldTimerManager().SetTimer(DeathAnimationTimer, this, &ABP_Character::DeathSequenceProxy, 2.f, false);
	UGameplayStatics::PlaySound2D(CurrentWorld, DeathSound);
}

/// This is a collision test to check if the player is colliding with an artifact or a shield
void ABP_Character::PickingUpArtifacts(UPrimitiveComponent * OverlappedComp, AActor * OtherActor,
	UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	//Sent to GameMode for testing before physical reentry
	if ((OtherActor->IsA(ASWorldActor::StaticClass()) || OtherActor->IsA(APWorldActor::StaticClass())) &&
		(!OtherActor->IsA(AArtifacts::StaticClass()) && !OtherActor->IsA(AS_PickupShield::StaticClass())))
	{
		OtherActorForPhysicalTest = OtherActor;
	}

	/// This is the part of the collision test that checks if it is an artifact that the player is colliding with
	if (Cast<AArtifacts>(OtherActor))
	{
		OtherActor->SetActorEnableCollision(false);
		AArtifacts* PickedUpActor = Cast<AArtifacts>(OtherActor);

		++NumberOfHoldingArtifacts;

		PickedUpArtifactsIndexes.Add(PickedUpActor->GetArrayIndex());

		if (PickedUpActor->bKeepTrackOf)
		GameInstance->RegisterPickedUp(PickedUpActor->GetArrayIndex(), OtherActor);

		PickedUpActor->PickupFeedback();

		/// This helps the artifact display move smoothly out from the side
		if (PickedUpArtifact == false)
		{
			LastTimePickedUpArtifact = UGameplayStatics::GetRealTimeSeconds(GetWorld());
			PickedUpArtifact = true;
		}
		
		/// after the pickedUpArtifact boolean has been true for 3 seconds, it will be reset and the artifact display moves back out of the screen
		GetWorldTimerManager().SetTimer(ArtifactDisplayResetTimer, this, &ABP_Character::ResetArtifactDisplayValues, 3.f, false);
	}

	/// This is the part of the collision test that checks if it is a shield that the player is colliding with
	else if (Cast<AS_PickupShield>(OtherActor) && GetShields() < 3)
	{
		OtherActor->SetActorEnableCollision(false);
		AS_PickupShield* PickedUpActor = Cast<AS_PickupShield>(OtherActor);

		if (PickedUpActor->bKeepTrackOf)
		GameInstance->RegisterPickedUp(PickedUpActor->GetArrayIndex(), OtherActor);;

		PickedUpActor->PickupFeedback();
		++NumberOfShields;
	}
}

///Collision test for kicking an enemy
void ABP_Character::HittingEnemy(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, 
	UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	///This checks if it is colliding with a spirit enemy
	if (OtherActor->IsA(ASpiritTest::StaticClass()))
	{
		ASpiritTest* Spirit = Cast<ASpiritTest>(OtherActor);

		if (NumberOfKicks <= 2)
		{
			Spirit->DecrementingLives(GetActorForwardVector()*KnockbackForce);
		}
		else
		{
			Spirit->DecrementingLives(GetActorForwardVector()*KnockbackForce);
			/*

			///Code meant for combo ability.
			///currently makes the game crash because it adds the enemy twice to the pending kill list, 
			/// depending on if the second life taken is the killing blow or not
			/// won't work on this until we have an animation for it
			for (int i{ 0 }; i < 2; ++i)
			{
				UE_LOG(LogTemp, Warning, TEXT("Double damage"))
				Spirit->DecrementingLives();
			}
			*/
			
		}
		
		/// This resets the targeting of the enemy if it dies
		if (Spirit->Lives <= 0 && isTargetingEnemy == true)
		{
			isTargetingEnemy = false;
		}
	}

	/// The same code is applied to a shaman
	else if (OtherActor->IsA(APShamanEnemy::StaticClass()))
	{
			APShamanEnemy* Shaman = Cast<APShamanEnemy>(OtherActor);
		if (NumberOfKicks <= 2)
		{
			Shaman->DecrementingLives(GetActorForwardVector()*KnockbackForce);
		}
		else
		{
			Shaman->DecrementingLives(GetActorForwardVector()*KnockbackForce);

			/*
			///Code meant for combo ability.
			///currently makes the game crash because it adds the enemy twice to the pending kill list, 
			/// depending on if the second life taken is the killing blow or not
			/// won't work on this until we have an animation for it
			for (int i{ 0 }; i < 2; ++i)
			{

				
				UE_LOG(LogTemp, Warning, TEXT("Double damage"))
				Shaman->DecrementingLives();
			}
			*/
			
		}
		if (Shaman->Lives <= 0 && isTargetingEnemy == true)
		{
			isTargetingEnemy = false;
		}
	}

	/// This turns off the kicking collider after the collision has started one time.
	/// This makes sure that the player doesn't take more than one damage from an enemy
	BoxCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BoxCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
	BoxCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
}


/// This is the second stage after the player dies
/// It is called from CurrentlyDying function and resets the artifacts if the player is killed by an enemy and drops them of next to the death location
/// Since we don't actually kill the instance of BP_Character when it is killed, we just hide it and teleport it to the last known checkpoint
/// Therefore we hide the actor instead
void ABP_Character::DeathSequence(bool bWithArtifactLoss)
{
	GetMesh()->SetHiddenInGame(true);
	Head->SetHiddenInGame(true);
	Mask->SetHiddenInGame(true);
	SetActorEnableCollision(false);
	DeathSmoke->Deactivate();

	if (bWithArtifactLoss)
	{
		UWorld* World = GetWorld();

		FVector ArtifactSpawnArea = GetActorLocation();

		for (int i = 0; i < NumberOfHoldingArtifacts; ++i)
		{
			AArtifacts* SpawnedArtifact = nullptr;
			ArtifactSpawnArea.X += FMath::RandRange(-200.f, 200.f);
			ArtifactSpawnArea.Y += FMath::RandRange(-200.f, 200.f);

			SpawnedArtifact = World->SpawnActor<AArtifacts>(ArtifactsToSpawn, ArtifactSpawnArea, FRotator(0.f, 0.f, 0.f));
			if (PickedUpArtifactsIndexes.IsValidIndex(i))
			{
				SpawnedArtifact->SetArrayIndex(PickedUpArtifactsIndexes[i]);
				UE_LOG(LogTemp, Warning, TEXT("PickedUpArtifactsIndex is valid with content %i"), PickedUpArtifactsIndexes[i])
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("PickedUpArtifactsIndex is NOT valid"))
					UE_LOG(LogTemp, Warning, TEXT("PickedUpArtifactsIndex contains %i elements"), PickedUpArtifactsIndexes.Num())

			}

			SpawnedArtifact->bKeepTrackOf = true;
			//GameInstance->SetArtifactPickedUp(PickedUpArtifactsIndexes[i]);

		}
		NumberOfHoldingArtifacts = 0;
		PickedUpArtifactsIndexes.Empty();

	}
		if ((WorldRequiredOnRespawn == EWorld::Physical && bIsSpiritWorld) ||
		(WorldRequiredOnRespawn == EWorld::Spirit && !bIsSpiritWorld))
	{
			GameModeRef->ChangeWorlds(false);
	}	


		
	GetWorldTimerManager().SetTimer(DeathSequenceTimer, this, &ABP_Character::RespawnSequence, 3.f, false);
}


void ABP_Character::DeathSequenceProxy()
{
	DeathSequence();
}

///This resets the variables for the player to default and respwns the player at the last known checkpoint
void ABP_Character::RespawnSequence()
{

	Lives = 3;
	SetShields(0);
	GetMesh()->SetHiddenInGame(false);
	Head->SetHiddenInGame(false);
	Mask->SetHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorLocation(RespawnLocation);
	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	isTargetingEnemy = false;
	EnableInput(GetWorld()->GetFirstPlayerController());
	RunningAnimations = EAnimations::MOVEMENT;
	if (bIsSpiritWorld)
	{
		Mask->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, false), FName("HeadSocket"));
	}
	else
	{
		Head->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, false), FName("HeadSocket"));
	}
	
}


/// This function is bound to the interact button E
/// This drops off artifacts at the altar
void ABP_Character::DeliveringArtifacts(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, 
	UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor->IsA(AAltar::StaticClass()) && NumberOfHoldingArtifacts > 0 && bIsSpiritWorld == true)
	{

		Cast<AAltar>(OtherActor)->ReceivingArtifacts(NumberOfHoldingArtifacts);

		NumberOfHoldingArtifacts = 0;
	}
}


AActor* ABP_Character::GetOtherActorForPhysicalTest()
{
	AActor* temp = OtherActorForPhysicalTest;

	OtherActorForPhysicalTest = nullptr;

	return temp;
}

///This is a function that sets the new last known checkpoint for the player
void ABP_Character::SetRespawnLocation(FVector NewSaveLocation)
{
	RespawnLocation = NewSaveLocation;
}

/// This activates the deathsmoke particle effect for the player right before it is hidden by the DeathSequence function
void ABP_Character::ActivateDeathSmoke()
{
	DeathSmoke->Activate();
	UGameplayStatics::PlaySound2D(CurrentWorld, DeathSmokeSound);
}

void ABP_Character::SetLives(int NewHealth)
{
	Lives = NewHealth;
}
void ABP_Character::SetArtifacts(int NewArtifacts)
{
	NumberOfHoldingArtifacts = NewArtifacts;
}

void ABP_Character::SetbIsSpiritWorld(bool state)
{
	bIsSpiritWorld = state;
}

/// A function determining the location of the currently floating head
void ABP_Character::FloatingHeadTimelineFloatReturn(float value)
{
	if (bIsSpiritWorld)
	{
		Head->SetRelativeLocation(FMath::Lerp(TrailingHeadLocation, TrailingHeadGoalLocation, value));
	}
	else
	{
		Mask->SetRelativeLocation(FMath::Lerp(TrailingHeadLocation, TrailingHeadGoalLocation, value));
	}
}

/// THis function switches the heads when the player changes worlds
void ABP_Character::SwitchingHead()
{
	

	if (bIsSpiritWorld)
	{
		Head->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true)); // resets the head and makes it not snap to the socket

		Head->SetRelativeLocation(TrailingHeadLocation);


		Mask->SetRelativeLocation(HeadSocketLocation);
		Mask->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, true), FName("HeadSocket"));

	}
	else
	{
		Mask->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true)); // resets the mask and makes it not snap to the socket

		Mask->SetRelativeLocation(TrailingHeadLocation);
		Head->SetRelativeLocation(HeadSocketLocation);

		Head->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, true), FName("HeadSocket"));
	}

	
}

/// The function that triggers the dashing ability
/// it is bound to Left Shift
void ABP_Character::Dashing()
{
	/// This if statement makes sure that the player is allowed to dash.
	/// The Currently Dashing variable is making sure that the player waits until the dashing cooldown is finished
	/// the bIsDashingLocked variable is making sure that the player isn't dashing before they have unlocked the ability
	if (CurrentlyDashing == false && GameInstance->bIsDashingLocked == false)
	{
		RunningAnimations = EAnimations::DASHING;
		/// Setting vectors
		ActorLocation = GetActorLocation();

		FVector CharacterAcceleration = GetCharacterMovement()->GetCurrentAcceleration().GetSafeNormal(1.0f);

		GoalLocation = ActorLocation + (CharacterAcceleration * DashingDistance);

		
		ReverseCurrentlyDashing();

		DashingTimeline->PlayFromStart();

	}
	
}

/// A function that determines the players current location during the dash
void ABP_Character::DashingTimelineFloatReturn(float value)
{
	SetActorLocation(FMath::Lerp(ActorLocation, GoalLocation, value), true);

}

/// What happens after the dashing timeline is over
void ABP_Character::OnDashingTimelineFinished()
{
	RunningAnimations = EAnimations::MOVEMENT;
	GetWorldTimerManager().SetTimer(DashCooldown, this, &ABP_Character::ReverseCurrentlyDashing, 1.0f, false);

}

/// The function that sends out the sensing sphere that lights up all objects in the parallell world
void ABP_Character::SenseWorld()
{
	if (GameInstance->bIsSensingLocked) return;

	if (!GameModeRef->GetSphereIsRunning() && GameModeRef->GetLitUpBySensing() == 0 && !GameModeRef->bIsLightUpEffectRunning)
	{

		GetWorld()->SpawnActor<ASensingSphere>(SensingSphereToSpawn, GetActorLocation() + FVector(0.f, 0.f,85.f), FRotator(0, 0, 0));
		UGameplayStatics::PlaySound2D(CurrentWorld, SensingSound);
	}
}