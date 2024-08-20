// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AStarPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ASTAR_API AAStarPlayerController : public APlayerController
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;
	void NextPath();
public:
	virtual void SetupInputComponent() override;
	
};
