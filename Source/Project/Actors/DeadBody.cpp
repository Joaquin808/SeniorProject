// Fill out your copyright notice in the Description page of Project Settings.
#include "DeadBody.h"
#include "Components/SkeletalMeshComponent.h"
#include "TimerManager.h"
#include "ProjectCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ADeadBody::ADeadBody()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	//DefaultRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultRoot"));
	//RootComponent = DefaultRoot;

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	//SkeletalMesh->SetupAttachment(DefaultRoot);

	StencilValue = 2;

}

// Called when the game starts or when spawned
void ADeadBody::BeginPlay()
{
	Super::BeginPlay();
	
	PlayerReference = Cast<AProjectCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

	GetWorldTimerManager().SetTimer(TimerHandle_DistanceToPlayer, this, &ADeadBody::CheckDistanceToPlayer, 0.1f, true);
}

void ADeadBody::CheckDistanceToPlayer()
{
	if (FVector::Dist(GetActorLocation(), PlayerReference->GetActorLocation()) <= DistanceToPlayerThreshold)
	{
		Outline(true);
	}
	else
	{
		Outline(false);
	}
}

void ADeadBody::Outline(bool bOutline)
{
	if (bOutline != bIsOutlined)
	{
		switch (bOutline)
		{
		case true:
			SkeletalMesh->SetRenderCustomDepth(true);
			SkeletalMesh->SetCustomDepthStencilValue(StencilValue);
			bIsOutlined = true;
			break;
		case false:
			SkeletalMesh->SetRenderCustomDepth(false);
			SkeletalMesh->SetCustomDepthStencilValue(0);
			bIsOutlined = false;
			break;
		}
	}
}

