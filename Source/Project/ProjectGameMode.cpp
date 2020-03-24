// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
#include "ProjectGameMode.h"
#include "ProjectCharacter.h"
#include "UObject/ConstructorHelpers.h"

AProjectGameMode::AProjectGameMode()
	: Super()
{
	Difficulty = EDifficulty::Normal;
}

void AProjectGameMode::SetDifficulty(EDifficulty NewDifficulty)
{
	Difficulty = NewDifficulty;
}
