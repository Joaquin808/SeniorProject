// Fill out your copyright notice in the Description page of Project Settings.
#include "DoorTravelPoints.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"
#include "AI/FollowAI.h"
#include "Environment/EnvironmentalObjects.h"
#include "Components/SphereComponent.h"

// Sets default values
ADoorTravelPoints::ADoorTravelPoints()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DefaultRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultRoot"));
	RootComponent = DefaultRoot;

	Point1Collider = CreateDefaultSubobject<USphereComponent>(TEXT("Point1Collider"));
	Point1Collider->SetupAttachment(DefaultRoot);
	Point1Collider->OnComponentBeginOverlap.AddDynamic(this, &ADoorTravelPoints::OnOverlap1Begin);

	Point1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Point1"));
	Point1->SetupAttachment(Point1Collider);

	Point2Collider = CreateDefaultSubobject<USphereComponent>(TEXT("Point2Collider"));
	Point2Collider->SetupAttachment(DefaultRoot);
	Point2Collider->OnComponentBeginOverlap.AddDynamic(this, &ADoorTravelPoints::OnOverlap2Begin);

	Point2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Point2"));
	Point2->SetupAttachment(Point2Collider);

}

// Called when the game starts or when spawned
void ADoorTravelPoints::BeginPlay()
{
	Super::BeginPlay();
	
	Point1Location = Point1->GetComponentLocation();
	Point2Location = Point2->GetComponentLocation();
}

void ADoorTravelPoints::OverlapTimerEnd()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_OverlapTimer);
}

// Called every frame
void ADoorTravelPoints::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADoorTravelPoints::OnOverlap1Begin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (GetWorldTimerManager().IsTimerActive(TimerHandle_OverlapTimer))
	{
		return;
	}

	auto AI = Cast<AFollowAI>(OtherActor);
	if (AI && Door)
	{
		if (Door->bDoorIsOpen)
		{
		
		}
		else
		{
			Door->OpenDoor();
		}

		GetWorldTimerManager().SetTimer(TimerHandle_OverlapTimer, this, &ADoorTravelPoints::OverlapTimerEnd, OverlapTimer);
		AI->SetActorLocation(Point2Location);
	}
}

void ADoorTravelPoints::OnOverlap2Begin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (GetWorldTimerManager().IsTimerActive(TimerHandle_OverlapTimer))
	{
		return;
	}

	auto AI = Cast<AFollowAI>(OtherActor);
	if (AI && Door)
	{
		if (Door->bDoorIsOpen)
		{

		}
		else
		{
			Door->OpenDoor();
		}

		GetWorldTimerManager().SetTimer(TimerHandle_OverlapTimer, this, &ADoorTravelPoints::OverlapTimerEnd, OverlapTimer);
		AI->SetActorLocation(Point1Location);
	}
}