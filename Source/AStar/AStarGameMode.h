// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AStarGameMode.generated.h"

/**
 * 
 */
class AGPoint;
UCLASS()
class ASTAR_API AAStarGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TSubclassOf<class AGPoint> BP_AGPoint;
	void PointClicked(AGPoint* Point);
	void FindPath();
private:
	void GenerateGrid();
	AGPoint* GetNextPoint(AGPoint* Point);
	void CheckPoint(int32 x, int32 y, int32* LowestCost, AGPoint *& LowestPoint, AGPoint* Parent);
	void AddToOpenSet(AGPoint* AddPoint);
	// maybe should be float, maybe not
	int32 CalculateHeuristic(AGPoint* Start, AGPoint* End);
	
	int32 GetIndex(AGPoint* Point);
	int32 mapWidth = 8;
	int32 mapHeight = 8;

	AGPoint* GetPoint(int32 x, int32 y);
	AGPoint* Selected;
	AGPoint* EndPoint;
	int32 CurrentPointIndex;

	TArray<AActor*> Points;

	TArray<AGPoint*> OpenSet;
	TArray<AGPoint*> ClosedSet;

protected:
	virtual void BeginPlay() override;
};
