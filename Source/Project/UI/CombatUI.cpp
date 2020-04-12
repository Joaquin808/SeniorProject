// Fill out your copyright notice in the Description page of Project Settings.
#include "CombatUI.h"
#include "Components/ProgressBar.h"

void UCombatUI::SetHealthBar(float Health, float MaxHealth)
{
	HealthBar->SetPercent(MaxHealth / Health);
}
