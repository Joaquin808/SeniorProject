// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ProjectGameMode.generated.h"

UENUM(BlueprintType)
enum class EDifficulty : uint8
{
	Easy,
	Normal,
	Hard
};

UCLASS(minimalapi)
class AProjectGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AProjectGameMode();

	// a difficulty modifier just in case I need it later
	EDifficulty Difficulty;

public:

	UFUNCTION(BlueprintCallable)
	void SetDifficulty(EDifficulty NewDifficulty);

};



