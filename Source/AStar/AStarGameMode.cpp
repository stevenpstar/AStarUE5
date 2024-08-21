// Fill out your copyright notice in the Description page of Project Settings.


#include "AStarGameMode.h"
#include "StarCharacter.h"
#include <Kismet/GameplayStatics.h>
#include "GPoint.h"
// F(n) = g(n) + h(n)
// n = next node  on path
// g(n) is cost of path from the start to n
// h(n) is estimated cost of cheapest path from n to the goal
void AAStarGameMode::PointClicked(AGPoint* Point)
{
	if (!PlayerSelected) {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, "PlayerSelected = false");
		return;
	}
	if (!Character) {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, "Character nullptr");
		return;
	}
	Selected = GetPoint(0, 0);
	int32 Index = GetIndex(Selected);
	EndPoint = Point;// GetPoint(7, 7);
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

			// TODO: Temporarily placing random walls.
			// Traversable should probably be a function that returns traversable depending on tile type.
			int32 randGen = FMath::RandRange(1, 10);
			if (randGen <= 2 && (x != 0 && y != 0)) {
				GPoint->SetTraversable(false);
			}
		}
	}
	if (BP_StarCharacter == nullptr) {
		return;
	}
	AStarCharacter* Ch = GetWorld()->SpawnActor<AStarCharacter>(BP_StarCharacter,
		FVector(0.0f, 0.0f, 100.0f), FRotator(0.0f, 0.0f, 0.0f), FActorSpawnParameters());
	Ch->OnPoint = GetPoint(0, 0);
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
	if (index < Points.Num()) {
		AGPoint* gp = Cast<AGPoint>(Points[index]);
		if (gp) {
			return gp;
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red,
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
	bool NorthWall = false;
	bool EastWall = false;
	bool SouthWall = false;
	bool WestWall = false;
	bool NEWall = false;
	bool NWWall = false;
	bool SEWall = false;
	bool SWWall = false;
	// Check North
	if (y + 1 < MaxY) {
		CheckPoint(x, y + 1, &LowestCost, LowestPoint, Point, 10, &NorthWall);
	}
	// Check South
	if (y - 1 >= 0) {
		CheckPoint(x, y - 1, &LowestCost, LowestPoint, Point, 10, &SouthWall);
	}
	// Check East
	if (x + 1 < MaxX) {
		CheckPoint(x + 1, y, &LowestCost, LowestPoint, Point, 10, &EastWall);
	}
	// Check West
	if (x - 1 >= 0) {
		CheckPoint(x - 1, y, &LowestCost, LowestPoint, Point, 10, &WestWall);
	}
	// Check North-East
	if (y + 1 < MaxY && x + 1 < MaxX) {
		if (NorthWall == false && EastWall == false) 
			CheckPoint(x + 1, y + 1, &LowestCost, LowestPoint, Point, 14, &NEWall);
	}

	if (y - 1 >= 0 && x + 1 < MaxX) {
		if (SouthWall == false && EastWall == false) 
			CheckPoint(x + 1, y - 1, &LowestCost, LowestPoint, Point, 14, &SEWall);
	}

	if (y + 1 < MaxY && x - 1 >= 0) {
		if (NorthWall == false && WestWall == false) 
			CheckPoint(x - 1, y + 1, &LowestCost, LowestPoint, Point, 14, &NWWall);
	}

	if (y - 1 >= 0 && x - 1 >= 0) {
		if (SouthWall == false && WestWall == false) 
			CheckPoint(x - 1, y - 1, &LowestCost, LowestPoint, Point, 14, &SWWall);
	}

	return LowestPoint;
}

void AAStarGameMode::CheckPoint(int32 x, int32 y, int32* LowestCost, AGPoint *& LowestPoint, AGPoint* Parent, int32 Cost, bool* FoundWall)
{
	AGPoint* GPoint = GetPoint(x, y);
	if (!GPoint) {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "GPoint Not Found");
		return;
	}
	if (InClosedSet(GPoint) || GPoint->Traversable == false) {
		// Ignore this point if it is already in the closed set or it is a non-traversable tile
		if (!GPoint->Traversable) {
			*FoundWall = true;
		}
		return;
	}
	if (!AddToOpenSet(GPoint)) {
		// GPoint already existed in the open set, need to check if cost from new point is cheaper
		int32 NewCost = Parent->Cost + Cost;
		if (NewCost > GPoint->Cost) {
			// Reparent
		//	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, "Reparenting");
			//GPoint->Parent = Parent;
		}
	}
	else {
		GPoint->Parent = Parent;
	}
	GPoint->Cost = Parent->Cost + Cost;

	int32 h = GPoint->Cost + CalculateHeuristic(GPoint, EndPoint);
	if (h < *LowestCost) {
		*LowestCost = h;
		LowestPoint = GPoint;
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
			NextPoint = Selected->Parent;
		}
		// Remove Starting Node (Consider RemoveAtSwap if performance is an issue)
		OpenSet.RemoveAt(CurrentPointIndex);
		ClosedSet.Push(Selected);
		if (Selected == EndPoint) {
			GoalFound = true;
			break;
		}

		if (OpenSet.Num() > 0) {
			Selected = NextPoint;
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Cannot Find Path");
			break;
		}
		maxTriesCounter++;
	}
	AGPoint* Start = GetPoint(0, 0);
	AGPoint* CurrentPoint = EndPoint;
	CurrentPoint->SetSelected(true);
	TArray<AGPoint*> Path;
	Path.Push(EndPoint);
	bool StartFound = false;
	int32 pathTries = 74;
	int32 pathCounter = 0;
	while (!StartFound && pathCounter < pathTries) {
		if (!CurrentPoint->Parent) {
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Parent does not exist");
			break;
		}
		CurrentPoint = CurrentPoint->Parent;
		CurrentPoint->SetSelected(true);
		if (CurrentPoint == Start) {
			StartFound = true;
		}
		Path.Push(CurrentPoint);
		pathCounter++;
	}

}

void AAStarGameMode::SetCharacter(AStarCharacter* Char)
{
	// Sets character also sets it selected, not sure about setting character???
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, "Setting Character");
	Character = Char;
	PlayerSelected = true;
}

bool AAStarGameMode::AddToOpenSet(AGPoint* AddPoint)
{
	bool found = false;
	for (AGPoint* P : OpenSet) {
		if (P == AddPoint) {
			found = true;
		}
	}

	if (!found) {
		OpenSet.Push(AddPoint);
	}
	return !found;
}

bool AAStarGameMode::InClosedSet(AGPoint* Point)
{
	bool found = false;
	if (!found) {
		for (AGPoint* P : ClosedSet) {
			if (P == Point) {
				found = true;
			}
		}
	}

	return found;
}

int32 AAStarGameMode::CalculateHeuristic(AGPoint* Start, AGPoint* EP)
{
	// Maybe revisit to remove sqrt
	int32 D = 10; // ortho
	int32 D2 = 14; // diag
	int32 dx = abs(Start->x - EP->x);
	int32 dy = abs(Start->y - EP->y);
	int32 H = D * FGenericPlatformMath::Max(dx, dy) + (D2 - D) * FGenericPlatformMath::Min(dx, dy);
	return H;
}

void AAStarGameMode::BeginPlay()
{
	GenerateGrid();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGPoint::StaticClass(), Points);
}
