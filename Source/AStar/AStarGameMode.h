// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AStarGameMode.generated.h"

/**
 * 
 */
class AStarCharacter;
class AGPoint;
UCLASS()
class ASTAR_API AAStarGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TSubclassOf<class AGPoint> BP_AGPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TSubclassOf<class AStarCharacter> BP_StarCharacter;

	void PointClicked(AGPoint* Point);
	void FindPath();
	void SetCharacter(AStarCharacter* Char);
private:
	void GenerateGrid();
	AGPoint* GetNextPoint(AGPoint* Point);
	void CheckPoint(int32 x, int32 y, int32* LowestCost, AGPoint *& LowestPoint, AGPoint* Parent, int32 Cost, bool* FoundWall);
	bool AddToOpenSet(AGPoint* AddPoint);
	bool InClosedSet(AGPoint* Point);
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

	AStarCharacter* Character;
	bool PlayerSelected = false;

protected:
	virtual void BeginPlay() override;
};
