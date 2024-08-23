// Fill out your copyright notice in the Description page of Project Settings.


#include "StarCharacter.h"
#include "AStarGameMode.h"
#include "GPoint.h"
#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetMathLibrary.h>

// Sets default values
AStarCharacter::AStarCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SelectMesh = CreateDefaultSubobject<UStaticMeshComponent>("SelectMesh");
	SelectMesh->SetupAttachment(RootComponent);

	SelectedMesh = CreateDefaultSubobject<UStaticMeshComponent>("SelectedMesh");
	SelectedMesh->SetupAttachment(RootComponent);
}

void AStarCharacter::MoveAlongPath(TArray<AGPoint*> P)
{
	// for now just move one node, starting from the 2nd last node.
	Moving = true;
	AStarPath = P;
	NextPoint = AStarPath.Num() - 1;
}

void AStarCharacter::SetSelected(bool Sel)
{
	if (SelectedMesh) {
		SelectedMesh->SetVisibility(Sel);
	}
}

// Called when the game starts or when spawned
void AStarCharacter::BeginPlay()
{
	Super::BeginPlay();
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, "BeginPlayCalled");
	SelectMesh->OnClicked.AddDynamic(this, &AStarCharacter::OnClickedCharacter);
	SelectMesh->OnBeginCursorOver.AddDynamic(this, &AStarCharacter::OnHoveredCharacter);
	SelectMesh->OnEndCursorOver.AddDynamic(this, &AStarCharacter::OnStopHoveredCharacter);
	CharRotation = GetMesh()->GetComponentRotation();
}

void AStarCharacter::OnClickedCharacter(UPrimitiveComponent* TouchedActor, FKey ButtonPressed)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, "OnClickedCharacter");
	SelectedMesh->SetVisibility(true);
	AAStarGameMode* GameMode = Cast<AAStarGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		GameMode->SetCharacter(this);
	}
}

void AStarCharacter::OnHoveredCharacter(UPrimitiveComponent* TouchedComponent)
{
	SelectedMesh->SetVisibility(true);
	AAStarGameMode* GameMode = Cast<AAStarGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		GameMode->HoveringCharacter(this);
	}

}

void AStarCharacter::OnStopHoveredCharacter(UPrimitiveComponent* TouchedComponent)
{
	SelectedMesh->SetVisibility(false);
}


// Called every frame
void AStarCharacter::Tick(float DeltaTime)
{
	if (Moving && AStarPath.Num() > 0) {
		FVector CurrentLocation = RootComponent->GetComponentLocation();
		AGPoint* Target = AStarPath[NextPoint];
		FVector TargetLocation = FVector(Target->x * 200.0f, Target->y * 200.0f, 100.0f);
		FVector NewLocation =
			FMath::VInterpConstantTo(
				CurrentLocation,
				TargetLocation,
				DeltaTime,
				300.0f);
		this->SetActorLocation(NewLocation);
		FVector Dir = CurrentLocation - TargetLocation;
		FRotator PlayerRot = GetMesh()->GetComponentRotation();//FRotationMatrix::MakeFromX().Rotator();
		//FRotator TargetRot = FRotationMatrix::MakeFromX(FVector(TargetLocation.X, TargetLocation.Y, 0)).Rotator();
		FRotator DirRot = FRotationMatrix::MakeFromX(FVector(Dir.X, Dir.Y, 0.0f)).Rotator();
		//FRotator NewRot = UKismetMathLibrary::RLerp(PlayerRot, TargetRot, 1.0f, true);
		FRotator NewRot = FMath::RInterpConstantTo(PlayerRot, DirRot, DeltaTime, 100.0f);
		Rotating = true;
		this->SetActorRotation(DirRot);
		if (this->GetActorLocation() == TargetLocation) {
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, "Made it to the location!");
			CharRotation = GetMesh()->GetComponentRotation();
			Rotating = false;
			NextPoint -= 1;
			OnPoint = Target;
			if (Target == AStarPath[0]) {
				// We have made it to the end
				AAStarGameMode* GameMode = Cast<AAStarGameMode>(GetWorld()->GetAuthGameMode());
				Moving = false;
				if (GameMode) {
					GameMode->StopMoving();
					AStarPath.Empty();
				}
			}
		}
	}
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AStarCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

