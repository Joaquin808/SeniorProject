// Fill out your copyright notice in the Description page of Project Settings.
#include "BossAI.h"
#include "Engine/Engine.h"
#include "Pickups/Weapons/Sword.h"

// Sets default values
ABossAI::ABossAI()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABossAI::BeginPlay()
{
	Super::BeginPlay();
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	Weapon = GetWorld()->SpawnActor<ASword>(WeaponClass, FVector(-10.0f, 20.0f, 10.0f), FRotator(0.0f, 0.0f, -90.0f), SpawnParams);

	if (Weapon)
	{
		Weapon->SetActorEnableCollision(false);
		FString AttachmentSocket = "Socket_Weapon";
		FName SocketName = FName(*AttachmentSocket);
		Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);
	}

	Health = MaxHealth;
}

// Called every frame
void ABossAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABossAI::Damage(float Damage)
{
	if (bIsBlocking)
	{
		PlayAnimMontage(BlockingHitMontage);
	}
	else
	{
		Health = Health - Damage;
	}
}
