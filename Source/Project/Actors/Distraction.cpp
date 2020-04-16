// Fill out your copyright notice in the Description page of Project Settings.
#include "Distraction.h"
#include "Components/PawnNoiseEmitterComponent.h"
#include "Components/AudioComponent.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
ADistraction::ADistraction()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));

	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComp"));
	CollisionComp->SetupAttachment(StaticMesh);

	NoiseEmitterComp = CreateDefaultSubobject<UPawnNoiseEmitterComponent>(TEXT("NoiseEmitterComp"));

	DistractionAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("DistractionAudio"));

	NoiseLevel = 1.0f;
	NoiseRange = 20000.0f;

	StencilValue = 3;
}

// Called when the game starts or when spawned
void ADistraction::BeginPlay()
{
	Super::BeginPlay();
	
	Player = Cast<AProjectCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
}

void ADistraction::Distract()
{
	DistractionAudio->SetIntParameter(FName{TEXT("Type")}, ParamNumber);
	DistractionAudio->Play();
}

void ADistraction::CreateNoise()
{
	MakeNoise(NoiseLevel, this, GetActorLocation(), NoiseRange);
}

void ADistraction::StartDistraction()
{
	bIsDistracting = true;
	GetWorldTimerManager().SetTimer(TimerHandle_DistractionTimer, this, &ADistraction::Distract, DistractionAudio->Sound->GetDuration(), true);
	Player->ActiveDistractions.Add(this);
	GetWorldTimerManager().SetTimer(TimerHandle_MakeNoiseTimer, this, &ADistraction::CreateNoise, 0.1f, true);
}

void ADistraction::StopDistraction()
{
	bIsDistracting = false;
	GetWorldTimerManager().ClearTimer(TimerHandle_DistractionTimer);
	Player->ActiveDistractions.Remove(this);
	GetWorldTimerManager().ClearTimer(TimerHandle_MakeNoiseTimer);
	RemoveOutlineEffect();
}

void ADistraction::EnableOutlineEffect()
{
	StaticMesh->SetRenderCustomDepth(true);
	StaticMesh->SetCustomDepthStencilValue(StencilValue);

	// if we add extra static meshes onto our environmental object (ex: doors for a cabinet), then check to see
	// if we do, then render the outline effect on them as well
	TArray<USceneComponent*> Children;
	StaticMesh->GetChildrenComponents(true, Children);
	if (Children.Num() >= 1)
	{
		for (int i = 0; i < Children.Num(); i++)
		{
			UStaticMeshComponent* Mesh = Cast<UStaticMeshComponent>(Children[i]);
			if (Mesh)
			{
				Mesh->SetRenderCustomDepth(true);
				Mesh->SetCustomDepthStencilValue(StencilValue);
			}
		}
	}
}

void ADistraction::RemoveOutlineEffect()
{
	if (bIsDistracting)
		return;

	StaticMesh->SetRenderCustomDepth(false);
	StaticMesh->SetCustomDepthStencilValue(0);

	TArray<USceneComponent*> Children;
	StaticMesh->GetChildrenComponents(true, Children);
	if (Children.Num() >= 1)
	{
		for (int i = 0; i < Children.Num(); i++)
		{
			UStaticMeshComponent* Mesh = Cast<UStaticMeshComponent>(Children[i]);
			if (Mesh)
			{
				Mesh->SetRenderCustomDepth(false);
				Mesh->SetCustomDepthStencilValue(0);
			}
		}
	}
}

