// Fill out your copyright notice in the Description page of Project Settings.
#include "EnvironmentalObjects.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "ProjectCharacter.h"

// Sets default values
AEnvironmentalObjects::AEnvironmentalObjects()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));

	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComp"));
	CollisionComp->SetupAttachment(StaticMesh);

	StencilValue = 2;
}

// Called when the game starts or when spawned
void AEnvironmentalObjects::BeginPlay()
{
	Super::BeginPlay();

	if (bAlwaysOutlined)
	{
		EnableOutlineEffect();
	}
}

void AEnvironmentalObjects::EnableOutlineEffect()
{
	if (!bIsOutlined)
	{
		if (PlayerReference && PlayerReference->bDebugMode && bIsAWall)
		{
			return;
		}

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

		bIsOutlined = true;
	}
}

void AEnvironmentalObjects::RemoveOutlineEffect()
{
	if (!bAlwaysOutlined && !bWasRanInto)
	{
		if (PlayerReference && PlayerReference->bDebugMode && bIsAWall)
		{
			return;
		}

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

		bIsOutlined = false;
	}
}

