// Fill out your copyright notice in the Description page of Project Settings.
#include "FollowAI.h"
#include "Perception/PawnSensingComponent.h"
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "TimerManager.h"
#include "ProjectCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AFollowAI::AFollowAI()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("SensingComp"));
	PawnSensingComp->OnSeePawn.AddDynamic(this, &AFollowAI::OnPawnSeen);
	PawnSensingComp->OnHearNoise.AddDynamic(this, &AFollowAI::OnHearPawn);

	// the AI speed needs to be slower than the player, so they have the ability to actually run away
	GetCharacterMovement()->MaxWalkSpeed = 450.0f;
}

// Called when the game starts or when spawned
void AFollowAI::BeginPlay()
{
	Super::BeginPlay();

	AILocation = GetActorLocation();
	bIsPatroling = true;
	Patrol();

	// from the start, will check to see if the AI has stopped moving
	FTimerHandle TimerHandle_CheckNotMoving;
	GetWorldTimerManager().SetTimer(TimerHandle_CheckNotMoving, this, &AFollowAI::CheckNotMoving, 1.0f, true);
}

void AFollowAI::Patrol()
{
	UE_LOG(LogTemp, Log, TEXT("Patrol"));
	// change patrol radius and starting location based off whether or not the player was seen, but lost
	switch (bLostPlayer)
	{
	case true:
		PatrolRadius = DetectedPatrolRadius;
		PatrolStartLocation = PlayerLocation;
		break;
	case false:
		PatrolRadius = NonDetectedPatrolRadius;
		PatrolStartLocation = AILocation;
		break;
	}

	UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), UNavigationSystemV1::GetRandomReachablePointInRadius(this, PatrolStartLocation, PatrolRadius));
	if (bIsPatroling)
	{
		FTimerHandle TimerHandle_PatrolTimer;
		GetWorldTimerManager().SetTimer(TimerHandle_PatrolTimer, this, &AFollowAI::PatrolTimerEnd, FMath::RandRange(1, 4), false);
	}
}

void AFollowAI::PatrolTimerEnd()
{
	if (bIsPatroling)
	{
		Patrol();
	}
}

void AFollowAI::CheckNotMoving()
{
	if (GetWorldTimerManager().IsTimerActive(TimerHandle_NotMoving))
	{
		UE_LOG(LogTemp, Log, TEXT("Timer is active"));
		return;
	}

	if (bSawPlayer)
	{
		// starts timer to see if the AI location has been within a certain distance for more than 3 seconds
		// this issue was only happening after following the player, so only run this if they player has been seen
		if (FVector::Dist(GetActorLocation(), PlayerLocation) < 50)
		{
			UE_LOG(LogTemp, Log, TEXT("Within distance"));
			AILocation = GetActorLocation();
			GetWorldTimerManager().SetTimer(TimerHandle_NotMoving, this, &AFollowAI::CheckLocation, 0.01f, false, 3.0f);
		}
	}
}

void AFollowAI::CheckLocation()
{
	UE_LOG(LogTemp, Log, TEXT("Check location"));
	// if the AI has been in a similar location for more than 3 seconds, patrol again
	if (FVector::Dist(GetActorLocation(), AILocation) <= 50)
	{
		// if the player isn't beeing seen but was seen in the previous interval, then I need to patrol around the last point the player was seen
		bSawPlayer = false;
		bLostPlayer = true;
		bIsPatroling = true;
		Patrol();
		GetWorldTimerManager().ClearTimer(TimerHandle_NotMoving);
		UE_LOG(LogTemp, Log, TEXT("Start patrol again"));
	}
}

void AFollowAI::OnPawnSeen(APawn* OtherActor)
{
	UE_LOG(LogTemp, Log, TEXT("OnPawnSeen"));
	auto Player = Cast<AProjectCharacter>(OtherActor);
	if (Player)
	{
		// if the player has been seen by the AI, then the AI needs to chase after the player
		// play sound effect to note detection
		bIsPatroling = false;
		bSawPlayer = true;
		bLostPlayer = false;
		PlayerLocation = Player->GetActorLocation();
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), PlayerLocation);
	}
}

void AFollowAI::OnHearPawn(APawn* OtherActor, const FVector& Location, float Volume)
{

}

// Called every frame
void AFollowAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AFollowAI::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

