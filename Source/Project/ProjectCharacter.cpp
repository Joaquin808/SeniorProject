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

//////////////////////////////////////////////////////////////////////////
// AProjectCharacter

AProjectCharacter::AProjectCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;

}

void AProjectCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

}

void AProjectCharacter::Attack()
{

}

void AProjectCharacter::Ability()
{

}

void AProjectCharacter::DoCrouch()
{
	if (bIsCrouched)
	{
		UnCrouch();
		bIsCrouched = false;
		FirstPersonCameraComponent->SetRelativeLocation(FVector(39.5600014, 1.75, 64.0));
	}
	else
	{
		Crouch();
		bIsCrouched = true;
		FirstPersonCameraComponent->SetRelativeLocation(FVector(39.5600014, 1.75, 32.0));
	}
}

void AProjectCharacter::Dodge()
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
	}
}

void AProjectCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}