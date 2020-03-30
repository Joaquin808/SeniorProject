// Fill out your copyright notice in the Description page of Project Settings.
#include "DoorTravelPoints.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"
#include "AI/FollowAI.h"
#include "Environment/EnvironmentalObjects.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectCharacter.h"

// Sets default values
ADoorTravelPoints::ADoorTravelPoints()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

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

	RoomBox = CreateDefaultSubobject<UBoxComponent>(TEXT("RoomBox"));
	RoomBox->SetupAttachment(DefaultRoot);
	RoomBox->OnComponentBeginOverlap.AddDynamic(this, &ADoorTravelPoints::OnOverlapBoxBegin);
	RoomBox->OnComponentEndOverlap.AddDynamic(this, &ADoorTravelPoints::OnOverlapBoxEnd);

}

// Called when the game starts or when spawned
void ADoorTravelPoints::BeginPlay()
{
	Super::BeginPlay();
	
	Point1Location = Point1->GetComponentLocation();
	Point2Location = Point2->GetComponentLocation();

	PlayerReference = Cast<AProjectCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
}

void ADoorTravelPoints::CheckIfOverlapping()
{
	if (AI && Point1Collider->IsOverlappingActor(AI))
	{
		UE_LOG(LogTemp, Log, TEXT("Overlapped point 1"));
		// only teleport into the room if the player is in that room
		if (Door && bPlayerIsInRoom && !bAIIsInRoom && !bTravelingThroughDoor)
		{
			bTravelingThroughDoor = true;
			EnterRoom(AI);
			StopOverlapTimer();
		}
	}

	if (AI && Point2Collider->IsOverlappingActor(AI))
	{
		UE_LOG(LogTemp, Log, TEXT("Overlapped point 2"));
		// only teleport into the room if the player is in that room
		if (Door && !bPlayerIsInRoom && bAIIsInRoom && !bTravelingThroughDoor)
		{
			bTravelingThroughDoor = true;
			ExitRoom(AI);
			StopOverlapTimer();
		}
	}
}

void ADoorTravelPoints::StartOverlapTimer()
{
	UE_LOG(LogTemp, Log, TEXT("Start Overlap Timer"));
	GetWorldTimerManager().SetTimer(TimerHandle_OverlapTimer, this, &ADoorTravelPoints::CheckIfOverlapping, 0.1f, true);
}

void ADoorTravelPoints::StopOverlapTimer()
{
	UE_LOG(LogTemp, Log, TEXT("Stop Overlap Timer"));
	GetWorldTimerManager().ClearTimer(TimerHandle_OverlapTimer);
}

void ADoorTravelPoints::StopDoorOpeningTimer()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_DoorOpeningTimer);
}

void ADoorTravelPoints::EnterRoom(AFollowAI* AI)
{
	if (Door->bDoorIsOpen)
	{
		AI->SetActorLocation(Point2Location);
	}
	else
	{
		Door->OpenDoor();
		GetWorldTimerManager().SetTimer(TimerHandle_DoorOpeningTimer, this, &ADoorTravelPoints::StopDoorOpeningTimer, DoorOpeningTimer);
		TeleportLocation = Point2Location;
	}

	StartTravelTimer();

	if (!GetWorldTimerManager().IsTimerActive(TimerHandle_OverlapTimer))
	{
		StartOverlapTimer();
	}
	else
	{
		StopOverlapTimer();
	}
}

void ADoorTravelPoints::ExitRoom(AFollowAI* AI)
{
	if (Door->bDoorIsOpen)
	{
		AI->SetActorLocation(Point1Location);
	}
	else
	{
		Door->OpenDoor();
		GetWorldTimerManager().SetTimer(TimerHandle_DoorOpeningTimer, this, &ADoorTravelPoints::StopDoorOpeningTimer, DoorOpeningTimer);
		TeleportLocation = Point1Location;
	}

	StartTravelTimer();

	if (!GetWorldTimerManager().IsTimerActive(TimerHandle_OverlapTimer))
	{
		StartOverlapTimer();
	}
	else
	{
		StopOverlapTimer();
	}
}

// these functions are necessary because when the AI loses the player and starts patroling again and they had to go through a door
// they would non stop teleport in and out of the room where the travel points were placed
void ADoorTravelPoints::StartTravelTimer()
{
	GetWorldTimerManager().SetTimer(TimerHandle_TravelTimer, this, &ADoorTravelPoints::ClearTravelTimer, TravelTimer);
}

void ADoorTravelPoints::ClearTravelTimer()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_TravelTimer);
	bTravelingThroughDoor = false;
}

void ADoorTravelPoints::OnOverlap1Begin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AI = Cast<AFollowAI>(OtherActor);
	// only teleport into the room if the player is in that room
	if (AI && Door && bPlayerIsInRoom && !bAIIsInRoom && !bTravelingThroughDoor)
	{
		bTravelingThroughDoor = true;
		EnterRoom(AI);
	}

	// let the AI go in and out of rooms if they're patroling
	if (AI && AI->bIsPatroling && !bTravelingThroughDoor)
	{
		bTravelingThroughDoor = true;
		EnterRoom(AI);
	}

	// let the AI go in and out of rooms if they're going after heard player sounds
	if (AI && AI->bGoingAfterHeardPlayer && !bTravelingThroughDoor)
	{
		bTravelingThroughDoor = true;
		EnterRoom(AI);
	}
}

void ADoorTravelPoints::OnOverlap2Begin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto AI = Cast<AFollowAI>(OtherActor);
	if (AI && Door && !bPlayerIsInRoom && bAIIsInRoom && !bTravelingThroughDoor)
	{
		bTravelingThroughDoor = true;
		ExitRoom(AI);
	}

	// let the AI go in and out of rooms if they're patroling
	if (AI && AI->bIsPatroling && !bTravelingThroughDoor)
	{
		bTravelingThroughDoor = true;
		ExitRoom(AI);
	}

	// let the AI go in and out of rooms if they're going after heard player sounds
	if (AI && AI->bGoingAfterHeardPlayer && !bTravelingThroughDoor)
	{
		bTravelingThroughDoor = true;
		ExitRoom(AI);
	}
}

void ADoorTravelPoints::OnOverlapBoxBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (PlayerReference && PlayerReference == OtherActor)
	{
		bPlayerIsInRoom = true;
	}

	if (AI && AI == OtherActor)
	{
		bAIIsInRoom = true;
	}
}

void ADoorTravelPoints::OnOverlapBoxEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (PlayerReference && PlayerReference == OtherActor)
	{
		bPlayerIsInRoom = false;
	}

	if (AI && AI == OtherActor)
	{
		bAIIsInRoom = false;
	}
}
