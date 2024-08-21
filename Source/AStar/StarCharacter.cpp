// Fill out your copyright notice in the Description page of Project Settings.


#include "StarCharacter.h"
#include "AStarGameMode.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
AStarCharacter::AStarCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	SelectMesh = CreateDefaultSubobject<UStaticMeshComponent>("SelectMesh");
	SelectMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AStarCharacter::BeginPlay()
{
	Super::BeginPlay();
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, "BeginPlayCalled");
//	APlayerController* Controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
//	Controller->bAllowC
	SelectMesh->OnClicked.AddDynamic(this, &AStarCharacter::OnClickedCharacter);
}

void AStarCharacter::OnClickedCharacter(UPrimitiveComponent* TouchedActor, FKey ButtonPressed)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, "OnClickedCharacter");
	AAStarGameMode* GameMode = Cast<AAStarGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		GameMode->SetCharacter(this);
	}
}

// Called every frame
void AStarCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AStarCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

