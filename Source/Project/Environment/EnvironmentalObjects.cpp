// Fill out your copyright notice in the Description page of Project Settings.
#include "EnvironmentalObjects.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
AEnvironmentalObjects::AEnvironmentalObjects()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));

	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComp"));
	CollisionComp->SetupAttachment(StaticMesh);
}

// Called when the game starts or when spawned
void AEnvironmentalObjects::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnvironmentalObjects::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

