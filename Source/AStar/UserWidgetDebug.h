// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UserWidgetDebug.generated.h"

/**
 * 
 */
UCLASS()
class ASTAR_API UUserWidgetDebug : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintImplementableEvent)
	void SetTileCost(int32 Cost);
	
};
