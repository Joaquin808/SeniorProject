// Fill out your copyright notice in the Description page of Project Settings.
#include "DoorKey.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"

ADoorKey::ADoorKey()
{
	DefaultRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultRoot"));
	RootComponent = DefaultRoot;

	KeyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("KeyMesh"));
	KeyMesh->SetupAttachment(RootComponent);

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	BoxComp->SetupAttachment(KeyMesh);

	StencilValue = 2;
}

void ADoorKey::EnableOutline()
{
	KeyMesh->SetRenderCustomDepth(true);
	KeyMesh->SetCustomDepthStencilValue(StencilValue);
}

void ADoorKey::DisableOutline()
{
	KeyMesh->SetRenderCustomDepth(false);
	KeyMesh->SetCustomDepthStencilValue(0);
}
