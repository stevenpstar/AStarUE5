// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GPoint.generated.h"

UCLASS()
class ASTAR_API AGPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGPoint();
	int32 x;
	int32 y;
	int32 height;
	int32 Cost = 0;
	bool Traversable = true;
	AGPoint* Parent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UMaterialInterface* DefaultMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UMaterialInterface* SelectedMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UMaterialInterface* TraversableMaterial;

	void SetSelected(bool Selected);
	void SetTraversable(bool Trav);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* Mesh;

	UFUNCTION()
	void OnClickedActor(UPrimitiveComponent* TouchedActor, FKey ButtonPressed);
	UFUNCTION()
	void OnHoveredActor(UPrimitiveComponent* TouchedComponent );

	void SetParent(AGPoint* ParentPoint);

public:	
};
