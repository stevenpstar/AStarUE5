// Fill out your copyright notice in the Description page of Project Settings.


#include "AStarPlayerController.h"
#include "AStarGameMode.h"

void AAStarPlayerController::BeginPlay()
{
	bShowMouseCursor = true;
	bEnableClickEvents = false;
	bEnableMouseOverEvents = true;
}

void AAStarPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAction("NextPath", EInputEvent::IE_Pressed, this, &AAStarPlayerController::NextPath);
}

void AAStarPlayerController::NextPath()
{
	AAStarGameMode* GameMode = Cast<AAStarGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		//GameMode->FindPath();
	}

}
