// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GridPoint.generated.h"

UCLASS()
class ASTAR_API AGridPoint : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AGridPoint();
	int32 x;
	int32 y;
	int32 height;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

};
