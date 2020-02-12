// Fill out your copyright notice in the Description page of Project Settings.
#include "Sword.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"

ASword::ASword()
{
	SwordMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SwordMesh"));
	RootComponent = SwordMesh;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(RootComponent);

}