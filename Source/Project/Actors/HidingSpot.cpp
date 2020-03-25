// Fill out your copyright notice in the Description page of Project Settings.
#include "HidingSpot.h"
#include "Components/BoxComponent.h"
#include "ProjectCharacter.h"
#include "Camera/CameraComponent.h"

// Sets default values
AHidingSpot::AHidingSpot()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	DefaultRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultRoot"));
	RootComponent = DefaultRoot;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	BoxComp->SetupAttachment(DefaultRoot);
	BoxComp->OnComponentBeginOverlap.AddDynamic(this, &AHidingSpot::OnOverlapBegin);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(DefaultRoot);

}

// Called when the game starts or when spawned
void AHidingSpot::BeginPlay()
{
	Super::BeginPlay();
	
}

void AHidingSpot::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto Player = Cast<AProjectCharacter>(OtherActor);
	if (Player && !Player->bIsHiding)
	{
		Player->Hide(this);
	}
}
