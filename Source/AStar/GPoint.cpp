// Fill out your copyright notice in the Description page of Project Settings.


#include "GPoint.h"
#include "AStarGameMode.h"
// Sets default values
AGPoint::AGPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	RootComponent = Mesh;
}

void AGPoint::SetSelected(bool Selected)
{
	if (Selected) {
		if (SelectedMaterial) {
			Mesh->SetMaterial(0, SelectedMaterial);
		}
		return;
	}
	if (DefaultMaterial) {
		Mesh->SetMaterial(0, DefaultMaterial);
	}
}

// Called when the game starts or when spawned
void AGPoint::BeginPlay()
{
	Super::BeginPlay();
	Mesh->OnClicked.AddDynamic(this, &AGPoint::OnClickedActor);
	SetSelected(false); // set default material
}

void AGPoint::OnClickedActor(UPrimitiveComponent* TouchedActor, FKey ButtonPressed) {
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, "AGPoint Clicked");
	AAStarGameMode* GameMode = Cast<AAStarGameMode>(GetWorld()->GetAuthGameMode());
	SetSelected(true);
	if (GameMode) {
		GameMode->PointClicked(this);
	}
}

void AGPoint::SetParent(AGPoint* ParentPoint)
{
	Parent = ParentPoint;
}

