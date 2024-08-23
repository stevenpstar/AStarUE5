// Fill out your copyright notice in the Description page of Project Settings.


#include "AStarGameMode.h"
#include "StarCharacter.h"
#include <Kismet/GameplayStatics.h>
#include "Blueprint/UserWidget.h"
#include "UserWidgetDebug.h"
#include "GPoint.h"
// F(n) = g(n) + h(n)
// n = next node  on path
// g(n) is cost of path from the start to n
// h(n) is estimated cost of cheapest path from n to the goal
// Using TArray, no priority queue
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
	if (PlayerMoving) { return; }
	Selected = Character->OnPoint;
	if (DebugWidget) {
		DebugWidget->SetTileCost(Point->Cost);
	}
	EndPoint = Point;
	FindPath();
}

void AAStarGameMode::MoveCommand() {
	if (Character && Path.Num() > 0) {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, "Move Along Path about to be called");
		Character->MoveAlongPath(Path);
		PlayerMoving = true;
	}
}

void AAStarGameMode::StopMoving()
{
	PlayerMoving = false;
	Selected = Character->OnPoint;
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

	AStarCharacter* Ch2 = GetWorld()->SpawnActor<AStarCharacter>(BP_StarCharacter,
		FVector(7 * 200.0f, 7 * 200.0f, 100.0f), FRotator(0.0f, 0.0f, 0.0f), FActorSpawnParameters());
	Ch2->OnPoint = GetPoint(7, 7);

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

AGPoint* AAStarGameMode::GetNextPoint()
{
	int32 LowestCost = INT32_MAX;
	AGPoint* CheapestPoint = nullptr;
	int32 h = 0;
	for (int i = 0; i < OpenSet.Num();i++) {
		h = OpenSet[i]->Cost + CalculateHeuristic(OpenSet[i], EndPoint);
		if (h < LowestCost) {
			LowestCost = OpenSet[i]->Cost;
			CheapestPoint = OpenSet[i];
		}
	}

	if (CheapestPoint == nullptr) {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "No Cheapest Point found?");
	}
	return CheapestPoint;
}

void AAStarGameMode::AddNeighbours(AGPoint* Point)
{
	if (!Point) { 
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Point is null");
	}
	int32 x = Point->x;
	int32 y = Point->y;

	// Checking walls
	bool NorthWall = false;
	bool EastWall = false;
	bool SouthWall = false;
	bool WestWall = false;
	bool NEWall = false;
	bool NWWall = false;
	bool SEWall = false;
	bool SWWall = false;
	// Check North
	if (y + 1 < mapHeight) {
		CheckPoint(x, y + 1, Point, 10, &NorthWall);
	}
	// Check South
	if (y - 1 >= 0) {
		CheckPoint(x, y - 1, Point, 10, &SouthWall);
	}
	// Check East
	if (x + 1 < mapWidth) {
		CheckPoint(x + 1, y, Point, 10, &EastWall);
	}
	// Check West
	if (x - 1 >= 0) {
		CheckPoint(x - 1, y, Point, 10, &WestWall);
	}
	// Check North-East
	if (y + 1 < mapHeight && x + 1 < mapWidth) {
		if (NorthWall == false && EastWall == false) 
			CheckPoint(x + 1, y + 1, Point, 14, &NEWall);
	}

	if (y - 1 >= 0 && x + 1 < mapWidth) {
		if (SouthWall == false && EastWall == false) 
			CheckPoint(x + 1, y - 1, Point, 14, &SEWall);
	}

	if (y + 1 < mapHeight && x - 1 >= 0) {
		if (NorthWall == false && WestWall == false) 
			CheckPoint(x - 1, y + 1, Point, 14, &NWWall);
	}

	if (y - 1 >= 0 && x - 1 >= 0) {
		if (SouthWall == false && WestWall == false) 
			CheckPoint(x - 1, y - 1, Point, 14, &SWWall);
	}
}

// Checks if the point at x, y position is valid (traversable), re-parents if necessary and calculates and sets cost of traversal
void AAStarGameMode::CheckPoint(int32 x, int32 y, AGPoint* Parent, int32 Cost, bool* FoundWall)
{
	AGPoint* GPoint = GetPoint(x, y);
	if (GPoint == Parent) {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "GPoint == Parent this shouldn't happen");
	}
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
		int32 OldCost = Parent->Cost;
	//	if (OldCost > GPoint->Cost + Cost) {
	//		// Reparent
	//		GPoint->Parent = Parent;
	//		GPoint->Cost = Parent->Cost + Cost;
	//	}
		if (Parent->Cost + Cost < GPoint->Cost) {
			GPoint->Parent = Parent;
			GPoint->Cost = Parent->Cost + Cost;
		}
	}
	else {
		GPoint->Parent = Parent;
		GPoint->Cost = Parent->Cost + Cost;
	}
}

void AAStarGameMode::FindPath()
{
	int32 maxTries = 100;
	int32 maxTriesCounter = 0;
	bool GoalFound = false;
	// set every point found in prev open/closed sets to not selected to clear visuals
	for (AGPoint* OP : OpenSet) {
		OP->SetSelected(false);
	}
	for (AGPoint* OP : ClosedSet) {
		OP->SetSelected(false);
	}

	OpenSet.Empty();
	ClosedSet.Empty();
	if (!Selected) {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, 
			"Selected not found, not finding path!");
		return;
	}

	ClosedSet.Push(Selected);
	while (!GoalFound && maxTriesCounter < maxTries) {
		// Add any neighbours that aren't in the open list, to the open list
		AddNeighbours(Selected);
		AGPoint* NextPoint = GetNextPoint();
		for (int i = 0; i < OpenSet.Num(); i++) {
			if (OpenSet[i] == NextPoint) {
				CurrentPointIndex = i;
			}
		}
		ClosedSet.Push(NextPoint);
		OpenSet.RemoveAt(CurrentPointIndex);
		if (Selected == nullptr) { 
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Selected found to be null, returning.");
			return;
		}
		if (NextPoint == nullptr) { 
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "No Next Point found, going to return error for now");
			return;
		}
		if (Selected == EndPoint) {
			GoalFound = true;
			// We found our goal yay
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
	AGPoint* Start = Character->OnPoint;
	AGPoint* CurrentPoint = EndPoint;
	CurrentPoint->SetSelected(true);
	Path.Empty();
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

void AAStarGameMode::FindMoveableTiles()
{

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, "Finding Moveable Tiles");
	// Find range of character movement.
	int32 range = 40;
	int32 maxTries = 100;
	int32 maxTriesCounter = 0;
	bool GoalFound = false;
	// set every point found in prev open/closed sets to not selected to clear visuals
	for (AActor* OP : Points) {
		AGPoint* P = Cast<AGPoint>(OP);
		P->SetWalkable(false);
		P->MCost = 0;
	}
	OpenSetMov.Empty();
	ClosedSetMov.Empty();
	// add starting point
	Character->OnPoint->Cost = 0;
	OpenSetMov.Push(Character->OnPoint);
	int32 ind = 0;
	while (OpenSetMov.Num() > 0 && maxTriesCounter < maxTries) {
		// Add Neighbours
		AddNeighboursUnderCost(OpenSetMov[ind], range);
		OpenSetMov[ind]->SetWalkable(true);
		ClosedSetMov.Push(OpenSetMov[ind]);
		OpenSetMov.RemoveAt(ind);
		if (OpenSetMov.Num() == 0) { return; }
		int32 lowestCost = INT32_MAX;
		for (int i = 0; i < OpenSetMov.Num(); i++) {
			if (OpenSetMov[i]->MCost < lowestCost && OpenSetMov[i]->MCost <= 40) {
				lowestCost = OpenSetMov[i]->MCost;
				ind = i;
			}
		}
		if (ind >= OpenSetMov.Num()) { return; }
		AGPoint* NextPoint = OpenSetMov[ind];
		maxTriesCounter++;
	}
	// Reset cost
	if (OpenSetMov.Num() > 0) {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, "OpenSetMov big");
	}
}

void AAStarGameMode::CheckPointUnderCost(int32 x, int32 y, AGPoint* Parent, int32 Cost, bool* FoundWall)
{
	AGPoint* GPoint = GetPoint(x, y);
	if (GPoint == Parent) {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "GPoint == Parent this shouldn't happen");
	}
	if (!GPoint) {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "GPoint Not Found");
		return;
	}
	if (InClosedSetMov(GPoint) || GPoint->Traversable == false) {
		// Ignore this point if it is already in the closed set or it is a non-traversable tile

		if (!GPoint->Traversable) {
			*FoundWall = true;
		}
		return;
	}
	bool found = false;
	for (AGPoint* P : OpenSetMov) {
		if (P == GPoint) {
			found = true;
		}
	}
	if (found) {

		int32 NewCost = Parent->MCost + Cost;
		if (NewCost < GPoint->MCost) {
			// Reparent
			GPoint->Parent = Parent;
			GPoint->MCost = Parent->MCost + Cost;
		}
	}
	else {
		GPoint->Parent = Parent;
		GPoint->MCost = Parent->MCost + Cost;
	}

	if (GPoint->MCost > 40) {
		return;
	}

	// extremely specific debug message
	if (GPoint->x == 1 && GPoint->y == 1) {
		// COST SHOULD BE 14 HERE
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue,
			FString::Printf(TEXT("Parent Cost : % d"), Parent->MCost));

		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue,
			FString::Printf(TEXT("GPoint Cost : % d"), GPoint->MCost));
	}

	AddToOpenSetMov(GPoint);
}


void AAStarGameMode::AddNeighboursUnderCost(AGPoint* Point, int32 cost)
{
	if (!Point) { 
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Point is null");
	}
	int32 x = Point->x;
	int32 y = Point->y;

	// Checking walls
	bool NorthWall = false;
	bool EastWall = false;
	bool SouthWall = false;
	bool WestWall = false;
	bool NEWall = false;
	bool NWWall = false;
	bool SEWall = false;
	bool SWWall = false;
	// Check North
	if (y + 1 < mapHeight) {
		CheckPointUnderCost(x, y + 1, Point, 10, &NorthWall);
	}
	// Check South
	if (y - 1 >= 0) {
		CheckPointUnderCost(x, y - 1, Point, 10, &SouthWall);
	}
	// Check East
	if (x + 1 < mapWidth) {
		CheckPointUnderCost(x + 1, y, Point, 10, &EastWall);
	}
	// Check West
	if (x - 1 >= 0) {
		CheckPointUnderCost(x - 1, y, Point, 10, &WestWall);
	}
	// Check North-East
	if (y + 1 < mapHeight && x + 1 < mapWidth) {
		if (NorthWall == false && EastWall == false) 
			CheckPointUnderCost(x + 1, y + 1, Point, 14, &NEWall);
	}

	if (y - 1 >= 0 && x + 1 < mapWidth) {
		if (SouthWall == false && EastWall == false) 
			CheckPointUnderCost(x + 1, y - 1, Point, 14, &SEWall);
	}

	if (y + 1 < mapHeight && x - 1 >= 0) {
		if (NorthWall == false && WestWall == false) 
			CheckPointUnderCost(x - 1, y + 1, Point, 14, &NWWall);
	}

	if (y - 1 >= 0 && x - 1 >= 0) {
		if (SouthWall == false && WestWall == false) 
			CheckPoint(x - 1, y - 1, Point, 14, &SWWall);
	}
}


void AAStarGameMode::SetCharacter(AStarCharacter* Char)
{
	// Sets character also sets it selected
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, "Setting Character");
	Character = Char;
	if (Character->OnPoint == nullptr) {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Setting ONPoint to Start");
		Character->OnPoint = GetPoint(Character->GetActorLocation().X / 200.0f, 
			Character->GetActorLocation().Y / 200.0f);
	}
	Selected = Character->OnPoint;
	PlayerSelected = true;
	FindMoveableTiles();
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
	// This is bad, basically return true if the function added the node to the open set
	return !found;
}

bool AAStarGameMode::InClosedSet(AGPoint* Point)
{
	bool found = false;
	for (AGPoint* P : ClosedSet) {
		if (P == Point) {
			found = true;
		}
	}
	return found;
}

bool AAStarGameMode::AddToOpenSetMov(AGPoint* AddPoint)
{
	bool found = false;
	for (AGPoint* P : OpenSetMov) {
		if (P == AddPoint) {
			found = true;
		}
	}

	if (!found) {
		OpenSetMov.Push(AddPoint);
	}
	// This is bad, basically return true if the function added the node to the open set
	return !found;
}

bool AAStarGameMode::InClosedSetMov(AGPoint* Point)
{
	bool found = false;
	for (AGPoint* P : ClosedSetMov) {
		if (P == Point) {
			found = true;
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
	if (WidgetDebugClass) {
		DebugWidget = Cast<UUserWidgetDebug>(CreateWidget(GetWorld(), WidgetDebugClass));
		if (DebugWidget) {
			DebugWidget->AddToViewport();
			DebugWidget->SetTileCost(0);
		}
	}
}
