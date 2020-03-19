// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "ProjectCharacter.h"
#include "ProjectProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Pickups/Weapons/Sword.h"
#include "Engine/World.h"
#include "Environment/EnvironmentalObjects.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/PawnNoiseEmitterComponent.h"
#include "Actors/AbilityLight.h"
#include "Components/PointLightComponent.h"
#include "AI/BossAI.h"
#include "Components/CombatComponent.h"
#include "Pickups/Pickup.h"
#include "AI/FollowAI.h"
#include "Engine/Engine.h"

//////////////////////////////////////////////////////////////////////////
// AProjectCharacter

AProjectCharacter::AProjectCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	Collider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Collider"));
	Collider->OnComponentBeginOverlap.AddDynamic(this, &AProjectCharacter::OnOverlapBegin);
	Collider->OnComponentEndOverlap.AddDynamic(this, &AProjectCharacter::OnOverlapEnd);
	Collider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	Collider->SetupAttachment(RootComponent);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetMesh());
	FirstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	NoiseEmitterComp = CreateDefaultSubobject<UPawnNoiseEmitterComponent>(TEXT("NoiseEmitterComp"));

	// for some reason it doesn't actually get created during construction
	//PointLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLight"));

	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));

}

void AProjectCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	if (bUseLight)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		Light = GetWorld()->SpawnActor<AAbilityLight>(LightClass, SpawnParams);
		Light->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale);
		Light->SetActorLocation(GetActorLocation() + FVector(0, 0, LightHeight));
	}

	CombatComponent->Owner = this;

	// I need the mesh hidden when were in the official map for the horror process, but need it visible in the BossMap for animations
	if (UGameplayStatics::GetCurrentLevelName(this) == "Official")
	{
		GetMesh()->SetHiddenInGame(true);
	}
	else if (UGameplayStatics::GetCurrentLevelName(this) == "BossMap")
	{
		GetMesh()->SetHiddenInGame(false);

		// Spawn weapon if we're in the boss map and change the players stance to combat
		if (Weapon)
		{
			return;
		}

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
			Stance = EStance::Combat;
		}
	}

	FTimerHandle TimerHandle_CheckForInteractions;
	GetWorldTimerManager().SetTimer(TimerHandle_CheckForInteractions, this, &AProjectCharacter::CheckForInteractions, 0.1f, true);
}

void AProjectCharacter::Attack()
{
	if (Weapon)
	{
		CombatComponent->Attack();
	}
}

void AProjectCharacter::HitWasBlocked()
{
	CombatComponent->HitWasBlocked();
 
}

void AProjectCharacter::BlockedHitDone()
{
	CombatComponent->BlockedHitDone();
}

void AProjectCharacter::Block()
{
	if (Weapon)
	{
		CombatComponent->Block();
	}
}

void AProjectCharacter::UnBlock()
{
	CombatComponent->UnBlock();
}

void AProjectCharacter::Damage(float Damage)
{
	UE_LOG(LogTemp, Log, TEXT("Damage done to player"));
	CombatComponent->TakeDamage(Damage);
}

void AProjectCharacter::Ability()
{
	if (GetWorldTimerManager().IsTimerActive(TimerHandle_SonarCooldown))
	{
		UE_LOG(LogTemp, Log, TEXT("Timer is still active"));
		return;
	}

	if (bUsePointLight)
	{
		SonarCooldownRate = PointLightRate;
	}

	GetWorldTimerManager().SetTimer(TimerHandle_SonarCooldown, this, &AProjectCharacter::SonarCooldown, 0.01, false, SonarCooldownRate);

	TArray<FHitResult> HitResults;

	FVector Start = GetActorLocation();
	FVector End = Start + (GetActorForwardVector() * TraceDistance);

	FQuat Quat;

	FCollisionShape Sphere = FCollisionShape::MakeSphere(SphereRadius);
	//Sphere.ShapeType = ECollisionShape::Sphere;
	//Sphere.MakeSphere(SphereRadius);

	FCollisionObjectQueryParams ObjectParams;
	//ObjectParams.AllDynamicObjects;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	if (bOutlineObjects)
	{
		DrawDebugSphere(GetWorld(), End, SphereRadius, 100, FColor::Red, false, 1.0f);
		if (GetWorld()->SweepMultiByChannel(HitResults, Start, End, FQuat::Identity, ECC_WorldDynamic, Sphere, QueryParams))
		{
			//UE_LOG(LogTemp, Log, TEXT("Hit something"));
			for (int32 i = 0; i < HitResults.Num(); i++)
			{
				auto Object = Cast<AEnvironmentalObjects>(HitResults[i].GetActor());
				if (Object)
				{
					Object->PlayerReference = this;
					Object->EnableOutlineEffect();
					RenderedObjects.Add(Object);
					//UE_LOG(LogTemp, Log, TEXT("Hit an object"));
				}

				auto Enemy = Cast<ABossAI>(HitResults[i].GetActor());
				if (BossAIReference && BossAIReference == Enemy)
				{
					BossAIReference->EnableOutlineEffect();
				}

				if (FollowAI && FollowAI == Cast<AFollowAI>(HitResults[i].GetActor()))
				{
					GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue, "FollowAI Exists, outline it");
					FollowAI->OutlineAI(true);
				}
			}
		}
	}

	if (bUseOverheadLight)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		AbilityLight = GetWorld()->SpawnActor<AAbilityLight>(AbilityLightClass, GetActorLocation() + FVector(0, 0, AbilityLightHeight), FRotator::ZeroRotator, SpawnParams);
	}

	if (bUsePointLight)
	{
		ExpandLight();
	}
}

void AProjectCharacter::SonarCooldown()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_SonarCooldown);

	if (bOutlineObjects)
	{
		for (auto Object : RenderedObjects)
		{
			Object->PlayerReference = this;
			Object->RemoveOutlineEffect();
		}

		RenderedObjects.Empty();

		if (BossAIReference)
		{
			BossAIReference->DisableOutlineEffect();
		}

		if (FollowAI)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue, "FollowAI Exists, remove the outline");
			FollowAI->OutlineAI(false);
		}
	}

	if (AbilityLight)
	{
		AbilityLight->Destroy();
	}

	if (bUsePointLight)
	{
		ReverseLight();
	}
}

void AProjectCharacter::CheckLightDistanceToAI(float LightRadius)
{
	if (!bDebugMode)
	{
		if (BossAIReference)
		{
			float DistToAI = FVector::Dist(GetActorLocation(), BossAIReference->GetActorLocation());
			if (DistToAI <= LightRadius)
			{
				BossAIReference->SetActorHiddenInGame(false);
				BossAIReference->Weapon->SetActorHiddenInGame(false);
			}
			else
			{
				BossAIReference->SetActorHiddenInGame(true);
				BossAIReference->Weapon->SetActorHiddenInGame(true);
			}
		}
	}
}

void AProjectCharacter::CheckForInteractions()
{
	CheckForDoors();
	CheckForPickups();
}

void AProjectCharacter::Interact()
{
	InteractWithDoor();
}

void AProjectCharacter::CheckForDoors()
{
	FHitResult HitResult;

	FVector Start = FirstPersonCameraComponent->GetComponentLocation();
	FVector End = Start + (FirstPersonCameraComponent->GetForwardVector() * LineTraceDistance);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility, QueryParams))
	{
		auto Object = Cast<AEnvironmentalObjects>(HitResult.GetActor());
		if (Object && Object->bIsDoor)
		{
			Door = Object;
			Door->EnableOutlineEffect();
			//UE_LOG(LogTemp, Log, TEXT("Door"));
		}
	}
	else
	{
		if (Door)
		{
			Door->RemoveOutlineEffect();
		}

		Door = nullptr;
	}
}

void AProjectCharacter::InteractWithDoor()
{
	if (Door)
	{
		switch (Door->bDoorIsOpen)
		{
		case true:
			Door->CloseDoor();
			Door->bDoorIsOpen = false;
			break;
		case false:
			Door->OpenDoor();
			Door->bDoorIsOpen = true;
			break;
		}
	}
}

void AProjectCharacter::CheckForPickups()
{
	FHitResult HitResult;

	FVector Start = FirstPersonCameraComponent->GetComponentLocation();
	FVector End = Start + (FirstPersonCameraComponent->GetForwardVector() * LineTraceDistance);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility, QueryParams))
	{
		// for keys
		auto Pickup = Cast<APickup>(HitResult.GetActor());
		if (Pickup)
		{
			Inventory.Add(Pickup->Name, Pickup);
		}
	}
}

void AProjectCharacter::DoCrouch()
{
	if (bIsCrouched)
	{
		UnCrouch();
		bIsCrouched = false;
		//FirstPersonCameraComponent->SetRelativeLocation(FVector(39.5600014, 1.75, 64.0));
	}
	else
	{
		Crouch();
		bIsCrouched = true;
		//FirstPersonCameraComponent->SetRelativeLocation(FVector(39.5600014, 1.75, 32.0));
	}
}

void AProjectCharacter::Dodge()
{
	
}

void AProjectCharacter::OnOverlapBegin(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	// if I run into an enironment object, outline it to let the player know that they hit something since they can't see
	auto Object = Cast<AEnvironmentalObjects>(OtherActor);
	if (Object && !Object->bIsOutlined)
	{
		Object->PlayerReference = this;
		Object->bWasRanInto = true;
		Object->EnableOutlineEffect();
	}
}

void AProjectCharacter::OnOverlapEnd(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	auto Object = Cast<AEnvironmentalObjects>(OtherActor);
	if (Object && Object->bWasRanInto)
	{
		Object->PlayerReference = this;
		Object->bWasRanInto = false;
		Object->RemoveOutlineEffect();
	}
}

void AProjectCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &AProjectCharacter::Attack);

	PlayerInputComponent->BindAction("Block", IE_Pressed, this, &AProjectCharacter::Block);
	PlayerInputComponent->BindAction("Block", IE_Released, this, &AProjectCharacter::UnBlock);

	PlayerInputComponent->BindAction("Ability", IE_Pressed, this, &AProjectCharacter::Ability);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AProjectCharacter::Interact);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AProjectCharacter::DoCrouch);

	PlayerInputComponent->BindAction("Dodge", IE_Pressed, this, &AProjectCharacter::Dodge);

	PlayerInputComponent->BindAxis("MoveForward", this, &AProjectCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AProjectCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
}

void AProjectCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
		MakeNoise(1.0f, this, GetActorLocation(), 1000.0f);
	}
}

void AProjectCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
		MakeNoise(1.0f, this, GetActorLocation(), 1000.0f);
	}
}