// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Vent.generated.h"

UCLASS()
class PROJECT_API AVent : public AActor
{
	GENERATED_BODY()

protected:

	USceneComponent* DefaultRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditDefaultsOnly)
	int32 StencilValue;

	// for debug purposes
	UPROPERTY(EditDefaultsOnly)
	bool bOpenVentOnStart;

	UPROPERTY(BlueprintReadWrite)
	bool bVentIsOpen;
	
public:	
	// Sets default values for this actor's properties
	AVent();

	UPROPERTY(BlueprintReadOnly)
	class AFollowAI* AI;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void EnableOutline();

	void DisableOutline();

public:

	UFUNCTION(BlueprintCallable)
	void OutlineVent(bool bOutline);

	UFUNCTION(BlueprintImplementableEvent)
	void OpenVent();

};
