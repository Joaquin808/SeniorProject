// Fill out your copyright notice in the Description page of Project Settings.
#include "BossAI.h"
#include "Engine/Engine.h"
#include "Pickups/Weapons/Sword.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectCharacter.h"
#include "NavigationSystem.h"
#include "TimerManager.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

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
		Weapon->SetOwner(this);
	}

	Health = MaxHealth;

	PlayerReference = Cast<AProjectCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	ApproachPlayer();

	FTimerHandle TimerHandle_CheckDistanceToPlayer;
	GetWorldTimerManager().SetTimer(TimerHandle_CheckDistanceToPlayer, this, &ABossAI::CheckDistanceToPlayer, 0.1f, true);
}

void ABossAI::ApproachPlayer()
{
	UE_LOG(LogTemp, Log, TEXT("Approach player"));
	UAIBlueprintHelperLibrary::SimpleMoveToActor(GetController(), PlayerReference);
}

void ABossAI::Attack()
{
	if (GetWorldTimerManager().IsTimerActive(TimerHandle_EventTimer))
	{
		return;
	}

	//bIsAttacking = true;
	GetWorldTimerManager().SetTimer(TimerHandle_EventTimer, this, &ABossAI::ClearTimer, 0.1f, false, PlayAnimMontage(MontageToPlay()));
	MontageIndex++;
	if (MontageIndex > (AttackAnimations.Num() - 1))
	{
		MontageIndex = 0;
	}
}

void ABossAI::Block()
{
	UE_LOG(LogTemp, Log, TEXT("Block"));
	bIsBlocking = true;
	PlayAnimMontage(BlockingMontage);
}

void ABossAI::UnBlock()
{
	UE_LOG(LogTemp, Log, TEXT("UnBlock"));
	bIsBlocking = false;
}

void ABossAI::Roll()
{

}

void ABossAI::CombatChoice()
{
	UE_LOG(LogTemp, Log, TEXT("Combat choice"));
	if (PlayerReference->bIsAttacking)
	{
		bool bLocal = FMath::RandBool();
		if (bLocal)
		{
			Block();
			if (bBlockedHit)
			{
				Attack();
			}
		}
		else
		{
			Roll();
		}
	}
	else
	{
		Attack();
	}
}

void ABossAI::CheckDistanceToPlayer()
{
	UE_LOG(LogTemp, Log, TEXT("Check Distance to Player"));
	if ((GetActorLocation() - PlayerReference->GetActorLocation()).Size() <= DistanceToPlayerThreshold && !bIsAttacking)
	{
		UE_LOG(LogTemp, Log, TEXT("Within distance"));
		CombatChoice();
	}
	else
	{
		ApproachPlayer();
	}
}

UAnimMontage * ABossAI::MontageToPlay()
{
	return AttackAnimations[MontageIndex];
}

void ABossAI::ClearTimer()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_EventTimer);
}

// Called every frame
void ABossAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// makes it so the AI is always looking at the player
	FVector PlayerLocation = PlayerReference->GetActorLocation() - GetActorLocation();
	PlayerLocation.Normalize();

	FRotator NewLookAt = PlayerLocation.Rotation();
	NewLookAt.Roll = 0.0f;
	NewLookAt.Pitch = 0.0f;

	SetActorRotation(NewLookAt);
	
}

void ABossAI::Damage(float Damage)
{
	UE_LOG(LogTemp, Log, TEXT("Damage done to BossAI"));
	if (bIsBlocking)
	{
		PlayAnimMontage(BlockingHitMontage);
		bBlockedHit = true;
		CombatChoice();
	}
	else
	{
		Health = Health - Damage;
	}
}
