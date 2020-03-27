// Fill out your copyright notice in the Description page of Project Settings.
#include "FollowAI.h"
#include "Perception/PawnSensingComponent.h"
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "TimerManager.h"
#include "ProjectCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Environment/EnvironmentalObjects.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/AudioComponent.h"

// Sets default values
AFollowAI::AFollowAI()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("SensingComp"));
	PawnSensingComp->OnSeePawn.AddDynamic(this, &AFollowAI::OnPawnSeen);
	PawnSensingComp->OnHearNoise.AddDynamic(this, &AFollowAI::OnHearPawn);

	FootstepAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("FootstepAudioComp"));
	
	FeetOutline = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FeetOutline"));
	FeetOutline->SetupAttachment(GetMesh());

	DetectionAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("DetectionAudioComp"));

	StencilValue = 2;

	bPlayHearNoise = true;
}

// Called when the game starts or when spawned
void AFollowAI::BeginPlay()
{
	Super::BeginPlay();

	NbrOfPatrolPoints = PatrolPoints.Num() - 1;
	CurrentPatrolIndex = 0;

	AILocation = GetActorLocation();
	bIsPatroling = true;
	Patrol();

	// from the start, will check to see if the AI has stopped moving
	FTimerHandle TimerHandle_CheckNotMoving;
	GetWorldTimerManager().SetTimer(TimerHandle_CheckNotMoving, this, &AFollowAI::CheckNotMoving, 1.0f, true);

	PlayerReference = Cast<AProjectCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (PlayerReference)
		PlayerReference->FollowAI = this;

	// if I'm in debug mode then keep the AI's footsteps visible at all times
	// else start a timer to check the distance between the AI and the player to display their footsteps
	if (PlayerReference && PlayerReference->bDebugMode)
	{
		FeetOutline->SetRenderCustomDepth(true);
		FeetOutline->SetCustomDepthStencilValue(2);
	}
	else
	{
		// starts a timer to check to see how close the AI is to the player every second
		FTimerHandle TimerHandle_ShowFootsteps;
		GetWorldTimerManager().SetTimer(TimerHandle_ShowFootsteps, this, &AFollowAI::ShowFootsteps, 1.0f, true);
	}

	DefaultSightRadius = PawnSensingComp->SightRadius;
}

void AFollowAI::Patrol()
{
	if (bDebugMessages)
		UE_LOG(LogTemp, Log, TEXT("Patrol"));

	// want the AI to patrol very slowly rather than sprinting around
	GetCharacterMovement()->MaxWalkSpeed = PatrolWalkSpeed;
	UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), PatrolPoint());
	if (bIsPatroling)
	{
		CurrentPatrolIndex++;
		if (CurrentPatrolIndex > NbrOfPatrolPoints)
		{
			CurrentPatrolIndex = 0;
		}

		FTimerHandle TimerHandle_PatrolTimer;
		GetWorldTimerManager().SetTimer(TimerHandle_PatrolTimer, this, &AFollowAI::PatrolTimerEnd, 3.0f, false);
	}
}

void AFollowAI::PatrolTimerEnd()
{
	if (bIsPatroling)
	{
		if (FVector::Dist(GetActorLocation(), CurrentPatrolPointLocation) <= 100.0f)
		{
			Patrol();
			if (bDebugMessages)
				UE_LOG(LogTemp, Log, TEXT("Close enough to point to transition"));
		}
		else
		{
			FTimerHandle TimerHandle_PatrolTimer;
			GetWorldTimerManager().SetTimer(TimerHandle_PatrolTimer, this, &AFollowAI::PatrolTimerEnd, 0.5f, false);
			if (bDebugMessages)
				UE_LOG(LogTemp, Log, TEXT("Not close enough to point to transition"));
		}
	}
}

FVector AFollowAI::PatrolPoint()
{
	return CurrentPatrolPointLocation = PatrolPoints[CurrentPatrolIndex]->GetActorLocation();
}

// when we lose the player, we want the AI to patrol around the player within a certain radius to wait to see if the player comes out of hiding
// before returning to their default patrol route
void AFollowAI::PatrolAroundPlayer()
{
	UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), UNavigationSystemV1::GetRandomReachablePointInRadius(this, PlayerLocation, PatrolRadius));
	GetWorldTimerManager().SetTimer(TimerHandle_PatrolAroundPlayer, this, &AFollowAI::PatrolAroundPlayerTimerEnd, FMath::RandRange(PatrolRandMin, PatrolRandMax), false);
}

void AFollowAI::PatrolAroundPlayerTimerEnd()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_PatrolAroundPlayer);
	// how many times do we want the AI to patrol around the players last seen position
	if (PatrolAroundPlayerTick > MaxPatrolAroundPlayerTicks)
	{
		Patrol();
		return;
	}

	PatrolAroundPlayer();
	PatrolAroundPlayerTick++;
}

void AFollowAI::CheckNotMoving()
{
	if (GetWorldTimerManager().IsTimerActive(TimerHandle_NotMoving))
	{
		if (bDebugMessages)
			UE_LOG(LogTemp, Log, TEXT("Timer is active"));
		return;
	}

	if (bSawPlayer)
	{
		// starts timer to see if the AI location has been within a certain distance for more than 3 seconds
		// this issue was only happening after following the player, so only run this if they player has been seen
		AILocation = GetActorLocation();
		GetWorldTimerManager().SetTimer(TimerHandle_NotMoving, this, &AFollowAI::CheckLocation, 0.01f, false, 3.0f);
	}
}

void AFollowAI::CheckLocation()
{
	if (bDebugMessages)
		UE_LOG(LogTemp, Log, TEXT("Check location"));
	// if the AI has been in a similar location for more than 3 seconds, patrol again
	if (FVector::Dist(GetActorLocation(), AILocation) <= 50)
	{
		// if the player isn't beeing seen but was seen in the previous interval, then I need to patrol around the last point the player was seen
		bSawPlayer = false;
		bLostPlayer = true;
		bIsPatroling = true;
		//Patrol();
		PatrolAroundPlayer();
		GetWorldTimerManager().ClearTimer(TimerHandle_NotMoving);
		if (bDebugMessages)
			UE_LOG(LogTemp, Log, TEXT("Start patrol again"));
	}
}

void AFollowAI::ClearSeenPlayerTimer()
{
	if (SeenPlayer)
		SeenPlayer = nullptr;
	PawnSensingComp->SightRadius = DefaultSightRadius;
	GetWorldTimerManager().ClearTimer(TimerHandle_SeenPlayerTimer);
}

void AFollowAI::OnPawnSeen(APawn* OtherActor)
{
	if (bDebugMessages)
		UE_LOG(LogTemp, Log, TEXT("OnPawnSeen"));
	SeenPlayer = Cast<AProjectCharacter>(OtherActor);
	if (SeenPlayer)
	{
		// if the player is hiding, ignore them and continue patroling around the current position
		if (SeenPlayer->bIsHiding)
		{
			// if the FollowAI is already patroling while the player is hiding, then don't keep setting these things
			if (bIsPatroling)
			{
				return;
			}

			bIsPatroling = true;
			//bSawPlayer = true;
			//bLostPlayer = true;
			//Patrol();
			GetCharacterMovement()->MaxWalkSpeed = PatrolWalkSpeed;
			PatrolAroundPlayer();
			return;
		}

		// currently plays a heartbeat sound. I want this to contiuiously play as the AI is following the AI
		PlaySeenDetectionNoise();

		// if the player has been seen by the AI, then the AI needs to chase after the player
		// play sound effect to note detection
		bIsPatroling = false;
		bSawPlayer = true;
		bLostPlayer = false;
		PlayerLocation = SeenPlayer->GetActorLocation();
		// Since there isn't a function for when the AI doesn't see any pawn or loses a pawn
		// I want to start a timer that will make sure certain settings are being set back when the AI doesn't see the player anymore
		// it'll be constantly reset every time the AI detects that they see the player
		if (GetWorldTimerManager().IsTimerActive(TimerHandle_SeenPlayerTimer))
			ClearSeenPlayerTimer();
		GetWorldTimerManager().SetTimer(TimerHandle_SeenPlayerTimer, this, &AFollowAI::ClearSeenPlayerTimer, SeenPlayerTimerLength);
		GetCharacterMovement()->MaxWalkSpeed = ChaseWalkSpeed;
		PawnSensingComp->SightRadius = FollowSightRadius;
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), PlayerLocation);
	}
}

void AFollowAI::OnHearPawn(APawn* OtherActor, const FVector& Location, float Volume)
{
	if (bDebugMessages)
		UE_LOG(LogTemp, Log, TEXT("Heard something"));
	bIsPatroling = false;
	if (!bSawPlayer)
		GetController()->StopMovement();

	// Currently plays a fear sound when being heard. Only want this to play once until the AI is lost again
	if (!DetectionAudioComp->IsPlaying() && bPlayHearNoise)
		PlayHearDetectionNoise();

	FVector LookAtLocation = Location - GetActorLocation();
	LookAtLocation.Normalize();

	FRotator NewLookAt = FRotationMatrix::MakeFromX(LookAtLocation).Rotator();
	NewLookAt.Pitch = 0.0f;
	NewLookAt.Roll = 0.0f;

	SetActorRotation(NewLookAt);

	if (bLostPlayer)
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), Location);
	}
}

void AFollowAI::PlaySeenDetectionNoise()
{
	DetectionAudioComp->SetBoolParameter(FName{ TEXT("Seen") }, true);
	DetectionAudioComp->Play();
}

void AFollowAI::PlayHearDetectionNoise()
{
	if (GetWorldTimerManager().IsTimerActive(TimerHandle_HearNoiseTimer))
	{
		return;
	}
	
	DetectionAudioComp->SetBoolParameter(FName{ TEXT("Seen") }, false);
	DetectionAudioComp->Play();
	bPlayHearNoise = false;
	GetWorldTimerManager().SetTimer(TimerHandle_HearNoiseTimer, this, &AFollowAI::CanPlayHearNoiseAgain, HearNoiseTimerDuration);
}

void AFollowAI::CanPlayHearNoiseAgain()
{
	bPlayHearNoise = true;
	GetWorldTimerManager().ClearTimer(TimerHandle_HearNoiseTimer);
}

void AFollowAI::ShowFootsteps()
{
	// if the AI is within a certain distance of the player, then turn on the outline of the footsteps, so the player has an idea of where the AI is
	if (FVector::Dist(GetActorLocation(), PlayerReference->GetActorLocation()) <= ShowFootstepsDistance)
	{
		if (bAIOutlined)
		{
			return;
		}

		OutlineFeet(true);
	}
	else
	{
		OutlineFeet(false);
	}
}

void AFollowAI::OutlineAI(bool bOutlineAI)
{
	switch (bOutlineAI)
	{
	case true:
		GetMesh()->SetRenderCustomDepth(true);
		GetMesh()->SetCustomDepthStencilValue(StencilValue);
		if (!PlayerReference->bDebugMode)
			OutlineFeet(false);
		bAIOutlined = true;
		break;
	case false:
		GetMesh()->SetRenderCustomDepth(false);
		GetMesh()->SetCustomDepthStencilValue(0);
		bAIOutlined = false;
		break;
	}
}

void AFollowAI::OutlineFeet(bool bOutlineFeet)
{
	switch (bOutlineFeet)
	{
	case true:
		FeetOutline->SetRenderCustomDepth(true);
		FeetOutline->SetCustomDepthStencilValue(StencilValue);
		break;
	case false:
		FeetOutline->SetRenderCustomDepth(false);
		FeetOutline->SetCustomDepthStencilValue(0);
		break;
	}
}
