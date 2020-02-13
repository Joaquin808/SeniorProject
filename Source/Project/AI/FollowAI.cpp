// Fill out your copyright notice in the Description page of Project Settings.
#include "FollowAI.h"
#include "Perception/PawnSensingComponent.h"
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "TimerManager.h"

// Sets default values
AFollowAI::AFollowAI()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("SensingComp"));
	PawnSensingComp->OnSeePawn.AddDynamic(this, &AFollowAI::OnPawnSeen);
	PawnSensingComp->OnHearNoise.AddDynamic(this, &AFollowAI::OnHearPawn);

}

// Called when the game starts or when spawned
void AFollowAI::BeginPlay()
{
	Super::BeginPlay();

	PatrolStartLocation = GetActorLocation();
	bIsPatroling = true;
	Patrol();
}

void AFollowAI::Patrol()
{
	UE_LOG(LogTemp, Log, TEXT("Patrol"));
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

void AFollowAI::OnPawnSeen(APawn* OtherActor)
{

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

