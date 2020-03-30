// Fill out your copyright notice in the Description page of Project Settings.
#include "Vent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AVent::AVent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	DefaultRoot = CreateAbstractDefaultSubobject<USceneComponent>(TEXT("DefaultRoot"));
	RootComponent = DefaultRoot;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(DefaultRoot);

	StencilValue = 2;
}

// Called when the game starts or when spawned
void AVent::BeginPlay()
{
	Super::BeginPlay();
	
	if (bOpenVentOnStart)
		OpenVent();
}

void AVent::EnableOutline()
{
	TArray<USceneComponent*> Children;
	Mesh->GetChildrenComponents(true, Children);
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

void AVent::DisableOutline()
{
	TArray<USceneComponent*> Children;
	Mesh->GetChildrenComponents(true, Children);
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

void AVent::OutlineVent(bool bOutline)
{
	switch (bOutline)
	{
	case true:
		EnableOutline();
		break;
	case false:
		DisableOutline();
		break;
	}
}
