// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "ProjectCharacter.h"
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
#include "Pickups/Keys/DoorKey.h"

//////////////////////////////////////////////////////////////////////////
// AProjectCharacter

AProjectCharacter::AProjectCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

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
	InteractWithPickups();
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
			Door->PlayDoorSound(EDoorType::Open);
			Door->bDoorIsOpen = false;
			break;
		case false:
			// if the door is closed, check to see if it's unlocked 
			// if the door is locked check to see if the player has the key to unlock the door, if they do unlock door and open it, if not do nothing
			// I'll add a sound effect and a animation of a locked door trying to be opened
			if (Door->bIsDoorLocked)
			{
				if (PlayerHasKeyForDoor(Door))
				{
					Door->bIsDoorLocked = false;
					Door->PlayDoorSound(EDoorType::Unlock);
					Door->OpenDoor();
					Door->PlayDoorSound(EDoorType::Closed);
					Door->bDoorIsOpen = true;
					GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, "Player has key");
				}
				else
				{
					Door->PlayDoorSound(EDoorType::Locked);
				}
			}
			else
			{
				Door->OpenDoor();
				Door->PlayDoorSound(EDoorType::Closed);
				Door->bDoorIsOpen = true;
			}

			break;
		}
	}
}

void AProjectCharacter::CheckForPickups()
{
	TArray<FHitResult> HitResults;

	FVector Start = FirstPersonCameraComponent->GetComponentLocation();
	FVector End = Start + (FirstPersonCameraComponent->GetForwardVector() * LineTraceDistance);

	FCollisionShape Sphere = FCollisionShape::MakeSphere(KeySphereRadius);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	if (bDrawCheckForPickupsSphere)
		DrawDebugSphere(GetWorld(), End, KeySphereRadius, 12, FColor::Purple, false, 1.0f);
	if (GetWorld()->SweepMultiByChannel(HitResults, Start, End, FQuat::Identity, ECollisionChannel::ECC_Visibility, Sphere, QueryParams))
	{
		// for keys
		for (int32 i = 0; i < HitResults.Num(); i++)
		{
			auto DoorKey = Cast<ADoorKey>(HitResults[i].GetActor());
			if (DoorKey)
			{
				// if the object is already outlined and detected, no reason to continuing to outline it every time this function is called
				if (FoundPickup)
					return;
				DoorKey->EnableOutline();
				FoundPickup = DoorKey;
			}
			// if FoundPickup is not nullptr, then I want to disable the outline of that object if it is equal to a doorkey 
			// and I'm currently not looking at it, then set it to nullptr afterwards
			else if (FoundPickup)
			{
				if (FoundPickup == Cast<ADoorKey>(FoundPickup))
				{
					auto Key = Cast<ADoorKey>(FoundPickup);
					Key->DisableOutline();
				}

				FoundPickup = nullptr;
			}
		}
	}
}

void AProjectCharacter::InteractWithPickups()
{
	if (FoundPickup)
	{
		// if found pickup is not nullptr then check to see if it is already in the players inventory, if it's not then add it to the players inventory and destroy it
		if (!PlayerHasItemInInventory(FoundPickup))
		{
			Inventory.Add(FoundPickup->Name, FoundPickup);
			FoundPickup->Destroy();
			FoundPickup = nullptr;
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, "Key added");
		}
	}
}

bool AProjectCharacter::PlayerHasKeyForDoor(AEnvironmentalObjects* Door)
{
	return Inventory.Contains(Door->DoorKeyName);
}

bool AProjectCharacter::PlayerHasItemInInventory(APickup* Item)
{
	return Inventory.Contains(Item->Name);
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

void AProjectCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetWorldTimerManager().IsTimerActive(TimerHandle_FPSTimer))
	{
		return;
	}

	this->DeltaTime = DeltaTime;
	GetWorldTimerManager().SetTimer(TimerHandle_FPSTimer, this, &AProjectCharacter::UpdateFPS, 1.0f, true);
}

void AProjectCharacter::UpdateFPS()
{
	// I've been experiencing some performance issues, so I want to keep track of my FPS
	// I imagine i'll need to know my current FPS to alter how the game operates at certain times
	FPS = 1.0f / DeltaTime;
	GetWorldTimerManager().ClearTimer(TimerHandle_FPSTimer);
	FString String = FString::SanitizeFloat(FPS);
	if(bDebugMode)
		GEngine->AddOnScreenDebugMessage(1, 0.5f, FColor::Blue, String);
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

		// alter sound and range based on if the player is crouched or not
		switch (bIsCrouched)
		{
		case true:
			FootStepVolume = CrouchedFootStepVolume;
			FootStepRange = CrouchedFootStepRange;
			break;
		case false:
			FootStepVolume = WalkingFootStepVolume;
			FootStepRange = WalkingFootStepRange;
			break;
		}

		MakeNoise(FootStepVolume, this, GetActorLocation(), FootStepRange);
	}
}

void AProjectCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);

		// alter sound and range based on if the player is crouched or not
		switch (bIsCrouched)
		{
		case true:
			FootStepVolume = CrouchedFootStepVolume;
			FootStepRange = CrouchedFootStepRange;
			break;
		case false:
			FootStepVolume = WalkingFootStepVolume;
			FootStepRange = WalkingFootStepRange;
			break;
		}

		MakeNoise(FootStepVolume, this, GetActorLocation(), FootStepRange);
	}
}