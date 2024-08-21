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
	void AddNeighbours(AGPoint* Point);
	void CheckPoint(int32 x, int32 y, AGPoint* Parent, int32 Cost, bool* FoundWall);

	bool AddToOpenSet(AGPoint* AddPoint);
	bool InClosedSet(AGPoint* Point);

	int32 CalculateHeuristic(AGPoint* Start, AGPoint* End);
	
	int32 mapWidth = 8;
	int32 mapHeight = 8;
	int32 CurrentPointIndex;

	bool PlayerSelected = false;

	AGPoint* GetPoint(int32 x, int32 y);
	AGPoint* GetNextPoint();
	AGPoint* Selected;
	AGPoint* EndPoint;
	AStarCharacter* Character;

	TArray<AActor*> Points;
	TArray<AGPoint*> OpenSet;
	TArray<AGPoint*> ClosedSet;


protected:
	virtual void BeginPlay() override;
};
