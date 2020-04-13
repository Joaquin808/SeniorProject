// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CombatUI.generated.h"

UCLASS()
class PROJECT_API UCombatUI : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidgetOptional))
	class UProgressBar* HealthBar;

	UPROPERTY(BlueprintReadOnly)
	float MaxHealth;

	UPROPERTY(BlueprintReadOnly)
	float Health;

public:

	void SetHealthBar(float Health, float MaxHealth);
	
};
