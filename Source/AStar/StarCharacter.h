// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "StarCharacter.generated.h"
class AGPoint;
UCLASS()
class ASTAR_API AStarCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AStarCharacter();
	AGPoint* OnPoint;

	void MoveAlongPath(TArray<AGPoint*> P);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Component")
	UAnimationAsset* Idle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Component")
	UAnimationAsset* Move;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	bool Moving = false;

	void SetSelected(bool Sel);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* SelectMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* SelectedMesh;

	UFUNCTION()
	void OnClickedCharacter(UPrimitiveComponent* TouchedActor, FKey ButtonPressed);
	UFUNCTION()
	void OnHoveredCharacter(UPrimitiveComponent* TouchedComponent );
	UFUNCTION()
	void OnStopHoveredCharacter(UPrimitiveComponent* TouchedComponent );


	FRotator CharRotation;
	bool Rotating = false;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	TArray<AGPoint*> AStarPath;
	int32 NextPoint = 0;

};
