// Fill out your copyright notice in the Description page of Project Settings.
#include "BossAI.h"
#include "Engine/Engine.h"
#include "Pickups/Weapons/Sword.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectCharacter.h"
#include "NavigationSystem.h"
#include "TimerManager.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Actors/RoamingPoint.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CombatComponent.h"
#include "Components/AudioComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "UI/CombatUI.h"

// Sets default values
ABossAI::ABossAI()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));

	FootstepAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("FootstepAudioComp"));

	FeetOutline = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FeetOutline"));
	FeetOutline->SetupAttachment(GetMesh());

	CombatAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("CombatAudioComp"));
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
		Weapon->Owner = this;
	}

	CombatComponent->Initialize(this, CombatAudioComp, HealthBar);

	PlayerReference = Cast<AProjectCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
    PlayerReference->BossAIReference = this;
	ApproachPlayer();

	SpawnRoamingPoints();

	FTimerHandle TimerHandle_CheckDistanceToPlayer;
	GetWorldTimerManager().SetTimer(TimerHandle_CheckDistanceToPlayer, this, &ABossAI::CheckDistanceToPlayer, 0.1f, true);
}

void ABossAI::ApproachPlayer()
{
	if (GetWorldTimerManager().IsTimerActive(TimerHandle_MoveAroundPlayer))
	{
		return;
	}

	//LogMessage("Approach player");
	UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), PlayerReference->GetActorLocation() - FVector(50, 0, 0));
}

void ABossAI::Roll()
{

}

void ABossAI::CombatChoice()
{
	//UE_LOG(LogTemp, Log, TEXT("Combat choice"));
	//if (PlayerReference->bIsAttacking)
	//{
		bool bLocal = FMath::RandBool();
		if (bLocal)
		{
			CombatComponent->Block();
			if (CombatComponent->bBlockedHit)
			{
				StopRoamingTimer();
				CombatComponent->Attack();
			}
			else
			{
				StartRoamingTimer();
			}
		}
		else
		{
			StopRoamingTimer();
			CombatComponent->Attack();
		}
	//}
	/**else
	{
		StopRoamingTimer();
		CombatComponent->Attack();
	}**/
}

void ABossAI::CheckDistanceToPlayer()
{
	//UE_LOG(LogTemp, Log, TEXT("Check Distance to Player"));
	if ((GetActorLocation() - PlayerReference->GetActorLocation()).Size() <= DistanceToPlayerThreshold)// && !CombatComponent->bIsAttacking)
	{
		//UE_LOG(LogTemp, Log, TEXT("Within distance"));
		if (!PlayerReference->bDebugMode)
		{
			SetActorHiddenInGame(false);
			Weapon->SetActorHiddenInGame(false);
		}
		
		if (!bIsOutlined)
			EnableFeetOutline();
		CombatChoice();
	}
	else
	{
		if (!PlayerReference->bDebugMode)
		{
			SetActorHiddenInGame(true);
			Weapon->SetActorHiddenInGame(true);
		}

		DisableFeetOutline();
		ApproachPlayer();
	}
}

void ABossAI::SpawnRoamingPoints()
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	RoamingPoint1 = GetWorld()->SpawnActor<ARoamingPoint>(RoamingPointClass, SpawnParams);
	RoamingPoint2 = GetWorld()->SpawnActor<ARoamingPoint>(RoamingPointClass, SpawnParams);
	RoamingPoint3 = GetWorld()->SpawnActor<ARoamingPoint>(RoamingPointClass, SpawnParams);

	if (RoamingPoint1 && RoamingPoint2 && RoamingPoint3)
	{
		RoamingPoint1->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale);
		RoamingPoint1->SetActorLocation(GetActorLocation() + RoamingPoint1Location);

		RoamingPoint2->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale);
		RoamingPoint2->SetActorLocation(GetActorLocation() + RoamingPoint2Location);

		RoamingPoint3->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale);
		RoamingPoint3->SetActorLocation(GetActorLocation() + RoamingPoint3Location);
	}

}

void ABossAI::MoveAroundPlayer()
{
	if (CombatComponent->bIsAttacking)
	{
		return;
	}

	int32 Rand = FMath::RandRange(0, 2);

	TArray<AActor*> RoamingPoints;
	RoamingPoints.Add(RoamingPoint1);
	RoamingPoints.Add(RoamingPoint2);
	RoamingPoints.Add(RoamingPoint3);

	float Distance = 0.0f;;
	AActor* Target = nullptr;

	for (int32 i = 0; i < RoamingPoints.Num(); i++)
	{
		// if Distance is equal to zero, then set the distance to the first RoamingPoints distance to the player
		if (Distance == 0.0f)
		{
			Distance = FVector::Dist(RoamingPoints[i]->GetActorLocation(), PlayerReference->GetActorLocation());
		}

		// if we can find a RoamingPoint that is closer to the player, then we want to set that actor as our target and update the largest distance
		if (FVector::Dist(RoamingPoints[i]->GetActorLocation(), PlayerReference->GetActorLocation()) <= Distance)
		{
			Distance = FVector::Dist(RoamingPoints[i]->GetActorLocation(), PlayerReference->GetActorLocation());
			Target = RoamingPoints[i];
		}
	}

	// if the target is not equal to nullptr, simply move to the current location of the current target (RoamingPoints)
	if (Target != nullptr)
	{
		//LogMessage("Move Around Player");
		AIMoveToLocation = Target->GetActorLocation();
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), AIMoveToLocation);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, "Target is nullptr");
	}
}

void ABossAI::StartRoamingTimer()
{
	if (GetWorldTimerManager().IsTimerActive(TimerHandle_MoveAroundPlayer))
	{
		return;
	}

	//LogMessage("StartRoamingTimer");
	GetWorldTimerManager().SetTimer(TimerHandle_MoveAroundPlayer, this, &ABossAI::MoveAroundPlayer, 1.5f, true);
}

void ABossAI::StopRoamingTimer()
{
	if (GetWorldTimerManager().IsTimerActive(CombatComponent->TimerHandle_EventTimer))
	{
		return;
	}

	if (GetWorldTimerManager().IsTimerActive(CombatComponent->TimerHandle_EventTimer))
	{
		if (FVector::Dist(GetActorLocation(), AIMoveToLocation) <= 50)
		{
			//LogMessage("StopRoamingTimer");
			GetWorldTimerManager().ClearTimer(CombatComponent->TimerHandle_EventTimer);
			return;
		}
		else
		{
			return;
		}
	}

	//LogMessage("StopRoamingTimer");
	GetWorldTimerManager().ClearTimer(CombatComponent->TimerHandle_EventTimer);
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
	if (CombatComponent->TakeDamage(Damage))
	{
		LogMessage("BossAI was damaged");
	}
	else
	{
		PlayerReference->HitWasBlocked();
		CombatChoice();
	}
	
}

void ABossAI::HitWasBlocked()
{
	CombatComponent->HitWasBlocked();
}

void ABossAI::EnableOutlineEffect()
{
	GetMesh()->SetRenderCustomDepth(true);
	GetMesh()->SetCustomDepthStencilValue(2);

	Weapon->SwordMesh->SetRenderCustomDepth(true);
	Weapon->SwordMesh->SetCustomDepthStencilValue(2);

	DisableFeetOutline();
	bIsOutlined = true;
}

void ABossAI::DisableOutlineEffect()
{
	if (PlayerReference->bDebugMode)
		return;

	GetMesh()->SetRenderCustomDepth(false);
	GetMesh()->SetCustomDepthStencilValue(0);

	Weapon->SwordMesh->SetRenderCustomDepth(false);
	Weapon->SwordMesh->SetCustomDepthStencilValue(0);

	bIsOutlined = false;
}

void ABossAI::EnableFeetOutline()
{
	FeetOutline->SetRenderCustomDepth(true);
	FeetOutline->SetCustomDepthStencilValue(2);
}

void ABossAI::DisableFeetOutline()
{
	FeetOutline->SetRenderCustomDepth(false);
	FeetOutline->SetCustomDepthStencilValue(0);
}

void ABossAI::LogMessage(FString Message)
{
	UE_LOG(LogTemp, Log, TEXT("%s"), *Message);
}