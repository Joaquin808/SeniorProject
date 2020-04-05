// Fill out your copyright notice in the Description page of Project Settings.
#include "CombatComponent.h"
#include "TimerManager.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/AudioComponent.h"
#include "ProjectCharacter.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	//PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;

}

void UCombatComponent::Initialize(ACharacter* Owner, UAudioComponent* CombatAudioComp)
{
	if (Owner && CombatAudioComp)
	{
		this->Owner = Owner;
		this->CombatAudio = CombatAudioComp;
	}
}

bool UCombatComponent::TakeDamage(float Damage)
{
	UE_LOG(LogTemp, Log, TEXT("Damage done to BossAI"));
	if (bIsBlocking)
	{
		if (StartEventTimer(BlockingHitMontage->GetPlayLength(), true))
			Owner->PlayAnimMontage(BlockingHitMontage);
		bBlockedHit = true;
		return false;
	}
	else
	{
		Health = FMath::Clamp(Health - Damage, 0.0f, MaxHealth);
		return true;
	}
}

void UCombatComponent::Attack()
{
	if (StartEventTimer(MontageToPlay()->GetPlayLength(), false)) //|| GetWorldTimerManager().IsTimerActive(TimerHandle_MoveAroundPlayer))
	{
		Owner->PlayAnimMontage(MontageToPlay());
		bIsAttacking = true;
		PlayCombatAudio(0);
		MontageIndex++;
		if (MontageIndex > (AttackAnimations.Num() - 1))
		{
			MontageIndex = 0;
		}
	}
}

UAnimMontage* UCombatComponent::MontageToPlay()
{
	return AttackAnimations[MontageIndex];
}

void UCombatComponent::Block()
{
	if (StartEventTimer(BlockingMontage->GetPlayLength(), false))
	{
		UE_LOG(LogTemp, Log, TEXT("Block"));
		bIsBlocking = true;
		Owner->PlayAnimMontage(BlockingMontage);
	}
}

void UCombatComponent::UnBlock()
{
	bIsBlocking = false;
	Owner->StopAnimMontage(BlockingMontage);
}

void UCombatComponent::Dodge()
{

}

void UCombatComponent::HitWasBlocked()
{
	if (bHitWasBlocked)
	{
		return;
	}

	if (StartEventTimer(StunnedDuration, true))
	{
		Owner->PlayAnimMontage(BlockingHitMontage);

		Owner->GetController()->StopMovement();

		bHitWasBlocked = true;
	}
}

void UCombatComponent::BlockedHitDone()
{
	ClearTimer();
}

bool UCombatComponent::StartEventTimer(float InRate, bool bCanBeOverriden)
{
	if (EventTimerActive() && !bCanBeOverriden)
	{
		UE_LOG(LogTemp, Log, TEXT("Event timer is active"));
		return false;
	}

	if (bCanBeOverriden)
		ClearTimer();

	GetWorld()->GetTimerManager().SetTimer(TimerHandle_EventTimer, this, &UCombatComponent::ClearTimer, InRate, false);
	return true;
}

bool UCombatComponent::EventTimerActive()
{
	return GetWorld()->GetTimerManager().IsTimerActive(TimerHandle_EventTimer);
}

void UCombatComponent::ClearTimer()
{
	bIsAttacking = false;
	Owner->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	bHitWasBlocked = false;
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_EventTimer);
}

void UCombatComponent::PlayCombatAudio(int32 Type)
{
	switch (Type)
	{
	case 0:
		if (Owner == Cast<AProjectCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)))
		{
			CombatAudio->SetBoolParameter(FName{ TEXT("Player") }, true);
		}
		else
		{
			CombatAudio->SetBoolParameter(FName{ TEXT("Player") }, false);
		}
		break;
	}

	CombatAudio->SetIntParameter(FName{ TEXT("Type") }, Type);
	CombatAudio->Play();
}

