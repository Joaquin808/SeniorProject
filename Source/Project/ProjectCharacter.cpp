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

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	/**Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;**/

}

void AProjectCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

}

void AProjectCharacter::Attack()
{
	if (Weapon)
	{
		if (Stance == EStance::Combat)
		{
			
		}
		else
		{
			Stance = EStance::Combat;
		}

		PlayAnimMontage(AttackMontage);
		UE_LOG(LogTemp, Log, TEXT("Attack"));
	}
}

void AProjectCharacter::Ability()
{
	if (GetWorldTimerManager().IsTimerActive(TimerHandle_SonarCooldown))
	{
		UE_LOG(LogTemp, Log, TEXT("Timer is still active"));
		return;
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

	DrawDebugSphere(GetWorld(), End, SphereRadius, 100, FColor::Red, false, 1.0f);
	if (GetWorld()->SweepMultiByChannel(HitResults, Start, End, FQuat::Identity, ECC_WorldDynamic, Sphere, QueryParams))
	{
		UE_LOG(LogTemp, Log, TEXT("Hit something"));
		for (int32 i = 0; i < HitResults.Num(); i++)
		{
			auto Object = Cast<AEnvironmentalObjects>(HitResults[i].GetActor());
			if (Object)
			{
				Object->StaticMesh->SetRenderCustomDepth(true);
				Object->StaticMesh->SetCustomDepthStencilValue(2);
				RenderedObjects.Add(Object);
				UE_LOG(LogTemp, Log, TEXT("Hit an object"));
			}
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

void AProjectCharacter::SonarCooldown()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_SonarCooldown);

	for (auto Object : RenderedObjects)
	{
		Object->StaticMesh->SetRenderCustomDepth(false);
		Object->StaticMesh->SetCustomDepthStencilValue(0);
	}

	RenderedObjects.Empty();
}

void AProjectCharacter::OnOverlapBegin(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	auto Sword = Cast<ASword>(OtherActor);
	if (Sword)
	{
		UE_LOG(LogTemp, Log, TEXT("Overlapped sword"));
		if (Weapon)
		{
			UE_LOG(LogTemp, Log, TEXT("Return"));
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
			Stance = EStance::Armed;
			UE_LOG(LogTemp, Log, TEXT("Spawned weapon"));
		}
	}
}

void AProjectCharacter::OnOverlapEnd(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{

}

void AProjectCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &AProjectCharacter::Attack);

	PlayerInputComponent->BindAction("Ability", IE_Pressed, this, &AProjectCharacter::Ability);

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