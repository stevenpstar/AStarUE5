// Fill out your copyright notice in the Description page of Project Settings.


#include "AStarGameMode.h"
#include <Kismet/GameplayStatics.h>
#include "GPoint.h"
// F(n) = g(n) + h(n)
// n = next node  on path
// g(n) is cost of path from the start to n
// h(n) is estimated cost of cheapest path from n to the goal
void AAStarGameMode::PointClicked(AGPoint* Point)
{
	if (Selected) {
		Selected->SetSelected(false);
	}
	Selected = Point;
	int32 Index = GetIndex(Selected);
	EndPoint = GetPoint(7, 7);
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, "Point clicked called in game mode, Starting Pathfinding...");
	if (!Selected) {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, "Selected is nullptr");
	} 
	if (!EndPoint) {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, "EndPoint is nullptr");
	}
	FindPath();
}

void AAStarGameMode::GenerateGrid()
{
	if (BP_AGPoint == nullptr) { return; }
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, "Generating Grid...");
	for (int y = 0; y < mapHeight; y++) {
		for (int x = 0; x < mapWidth; x++) {
			float xPos = x * 200.0f;
			float yPos = y * 200.0f;
			float zPos = 0.0f;
			AGPoint* GPoint = GetWorld()->SpawnActor<AGPoint>(BP_AGPoint,
				FVector(xPos, yPos, zPos), FRotator(0.0f, 0.0f, 0.0f), FActorSpawnParameters());
			GPoint->x = x;
			GPoint->y = y;
		}
	}
}

int32 AAStarGameMode::GetIndex(AGPoint* Point)
{
	int32 index = Point->x + (Point->y * mapWidth);
	if (index < Points.Num()) {
		AGPoint* gp = Cast<AGPoint>(Points[index]);
		if (gp) {
			int32 h = (7 - gp->x) + (7 - gp->y);
			return index;
		}
	}
	return -1;
}

AGPoint* AAStarGameMode::GetPoint(int32 x, int32 y)
{
	int32 index = x + (y * mapWidth);
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green,
		FString::Printf(TEXT("Checking index: %d"), index));
	if (index < Points.Num()) {
		AGPoint* gp = Cast<AGPoint>(Points[index]);
		if (gp) {
			return gp;
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green,
				"GP Not found");
		}
	}
	return nullptr;
}

AGPoint* AAStarGameMode::GetNextPoint(AGPoint* Point)
{
	if (!Point) { 
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Point is null");
		return nullptr; 
	}
	int32 MaxY = mapHeight;
	int32 MaxX = mapWidth;
	int32 x = Point->x;
	int32 y = Point->y;
	AGPoint* LowestPoint = nullptr;
	int32 LowestCost = INT32_MAX;
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, 
		FString::Printf(TEXT("x, y, %d, %d"), x, y));

	// Check North
	if (y + 1 < MaxY) {
		CheckPoint(x, y + 1, &LowestCost, LowestPoint, Point);
	}
	// Check South
	if (y - 1 >= 0) {
		CheckPoint(x, y - 1, &LowestCost, LowestPoint, Point);
	}
	// Check East
	if (x + 1 < MaxX) {
		CheckPoint(x + 1, y, &LowestCost, LowestPoint, Point);
	}
	// Check West
	if (x - 1 >= 0) {
		CheckPoint(x - 1, y, &LowestCost, LowestPoint, Point);
	}
	// Check North-East
	if (y + 1 < MaxY && x + 1 < MaxX) {
		CheckPoint(x + 1, y + 1, &LowestCost, LowestPoint, Point);
	}

	if (y - 1 >= 0 && x + 1 < MaxX) {
		CheckPoint(x + 1, y - 1, &LowestCost, LowestPoint, Point);
	}

	if (y + 1 < MaxY && x - 1 >= 0) {
		CheckPoint(x - 1, y + 1, &LowestCost, LowestPoint, Point);
	}

	if (y - 1 >= 0 && x - 1 >= 0) {
		CheckPoint(x - 1, y - 1, &LowestCost, LowestPoint, Point);
	}

	return LowestPoint;
}

void AAStarGameMode::CheckPoint(int32 x, int32 y, int32* LowestCost, AGPoint *& LowestPoint, AGPoint* Parent)
{
	AGPoint* GPoint = GetPoint(x, y);
	if (!GPoint) {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "GPoint Not Found");
	}
	// TODO: Setting Parent will need to change, come back to this
	GPoint->Parent = Parent;
	GPoint->Cost = Parent->Cost + 1;
	AddToOpenSet(GPoint);
	int32 h = GPoint->Cost + CalculateHeuristic(GPoint, EndPoint);
	if (h < *LowestCost) {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, "GPoint is next lowest");
		*LowestCost = h;
		LowestPoint = GPoint;
		if (LowestPoint) {
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, "LowestPoint is set!");
		}
	}
}

void AAStarGameMode::FindPath()
{
	int32 maxTries = 100;
	int32 maxTriesCounter = 0;
	bool GoalFound = false;
	// set every point found in prev open/closed sets to not selected
	for (AGPoint* OP : OpenSet) {
		OP->SetSelected(false);
	}
	for (AGPoint* OP : ClosedSet) {
		OP->SetSelected(false);
	}

	OpenSet.Empty();
	ClosedSet.Empty();
	// Add Starting Point to Open Set and save index
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, 
		FString::Printf(TEXT("Finding path from, x: %d y: %d, OpenNum: %d"), Selected->x, Selected->y, OpenSet.Num()));
	if (!Selected) {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, 
			"Selected not found, not finding path!");
		return;
	}
	Selected->SetSelected(true);
	while (!GoalFound && maxTriesCounter < maxTries) {
		AddToOpenSet(Selected);
		// Temp
		for (int i = 0; i < OpenSet.Num(); i++) {
			if (OpenSet[i] == Selected) {
				CurrentPointIndex = i;
			}
		}
		// Check neighbours, we add our selected (starting point) node as their parent.
		AGPoint* NextPoint = GetNextPoint(Selected);
		if (Selected == nullptr) { 
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Selected found to be null, returning.");
			return;
		}
		if (NextPoint == nullptr) { 
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Next Point found to be null, returning.");
			return;
		}
		// Remove Starting Node (Consider RemoveAtSwap if performance is an issue)
		OpenSet.RemoveAt(CurrentPointIndex);
		ClosedSet.Push(Selected);
		if (Selected == EndPoint) {
			GoalFound = true;
			Selected->SetSelected(true);
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Magenta, "Goal Found");
		}
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, FString::Printf(TEXT("Closed Set Count: %d"), ClosedSet.Num()));

		if (OpenSet.Num() > 0) {
			//Selected->SetSelected(false);
			NextPoint->SetSelected(true);
			Selected = NextPoint;
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Uh Something went wrong");
		}
		maxTriesCounter++;
	}
}

void AAStarGameMode::AddToOpenSet(AGPoint* AddPoint)
{
	bool found = false;
	for (AGPoint* P : OpenSet) {
		if (P == AddPoint) {
			found = true;
		}
	}
	if (!found) {
		for (AGPoint* P : ClosedSet) {
			if (P == AddPoint) {
				found = true;
			}
		}
	}

	if (!found) {
		OpenSet.Push(AddPoint);
	}
}

int32 AAStarGameMode::CalculateHeuristic(AGPoint* Start, AGPoint* EP)
{
	// Maybe revisit to remove sqrt
	int32 dx = abs(Start->x - EP->x);
	int32 dy = abs(Start->y - EP->y);
	return 1 * (dx + dy) + (sqrt(2) - 2 * 1) * FGenericPlatformMath::Min(dx, dy);
}

void AAStarGameMode::BeginPlay()
{
	GenerateGrid();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGPoint::StaticClass(), Points);
}
