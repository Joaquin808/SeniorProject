// Fill out your copyright notice in the Description page of Project Settings.
#include "Sword.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "TimerManager.h"
#include "Engine/Engine.h"
#include "Interfaces/CombatInterface.h"
#include "DrawDebugHelpers.h"

ASword::ASword()
{
	SwordMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SwordMesh"));
	RootComponent = SwordMesh;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(RootComponent);

	Collision1 = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision1"));
	Collision1->SetupAttachment(RootComponent);
	Collision1->SetRelativeLocation(FVector(-10.5, 0.000092, 129.602173));
	Collision1->SetRelativeScale3D(FVector(0.187219, 0.773331, 0.327722));

	Collision2 = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision2"));
	Collision2->SetupAttachment(RootComponent);
	Collision2->SetRelativeLocation(FVector(-10.5, 0.000092, 109.411697));
	Collision2->SetRelativeScale3D(FVector(0.187219, 0.773331, 0.327722));

	Collision3 = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision3"));
	Collision3->SetupAttachment(RootComponent);
	Collision3->SetRelativeLocation(FVector(-10.5, 0.000092, 88.933907));
	Collision3->SetRelativeScale3D(FVector(0.187219, 0.773331, 0.327722));

	Collision4 = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision4"));
	Collision4->SetupAttachment(RootComponent);
	Collision4->SetRelativeLocation(FVector(-10.5, 0.000092, 67.289299));
	Collision4->SetRelativeScale3D(FVector(0.187219, 0.773331, 0.327722));

	Collision5 = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision5"));
	Collision5->SetupAttachment(RootComponent);
	Collision5->SetRelativeLocation(FVector(-10.5, 0.000092, 46.67083));
	Collision5->SetRelativeScale3D(FVector(0.187219, 0.773331, 0.327722));

}

void ASword::BeginPlay()
{
	Super::BeginPlay();

	CollisionBoxes.Add(Collision1);
	CollisionBoxes.Add(Collision2);
	CollisionBoxes.Add(Collision3);
	CollisionBoxes.Add(Collision4);
	CollisionBoxes.Add(Collision5);

	Owner = GetOwner();
}

void ASword::Attack()
{
	for (auto Box : CollisionBoxes)
	{
		FHitResult Hit;

		FVector Start = Box->GetComponentLocation();
		FVector End = Start + (Box->GetForwardVector() * AttackDistance);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(Owner);

		if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, QueryParams))
		{
			auto Interface = Cast<ICombatInterface>(Hit.GetActor());
			if (Interface)
			{
				if (HitActors.Contains(Hit.GetActor()))
				{

				}
				else
				{
					HitActors.Add(Hit.GetActor());
					Interface->Damage(Damage);
				}
			}
		}
		
		DrawDebugLine(GetWorld(), Start, End, FColor::Red, true, 1.0f);
	}
}

void ASword::StartAttack()
{
	GetWorldTimerManager().SetTimer(TimerHandle_Attack, this, &ASword::Attack, 0.001f, true);
}

void ASword::StopAttack()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_Attack);
	HitActors.Empty();
}