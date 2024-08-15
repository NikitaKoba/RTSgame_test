#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "UnitSelectionManager.h" 
#include "CameraPawn.generated.h"

UCLASS()

class RTSGAME_API ARTSCameraPawn : public APawn
{
	GENERATED_BODY()

public:
	ARTSCameraPawn();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void SelectUnit();
	void MoveSelectedUnits();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	UFloatingPawnMovement* MovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float MovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float ZoomSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float MinZoomHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float MaxZoomHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float EdgeScrollThreshold;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float CameraAngle;

	UPROPERTY(VisibleAnywhere)
	AUnitSelectionManager* UnitSelectionManager; //
	
	float EdgeScrollSpeed;
	float WASDMovementSpeed;

protected:
	virtual void BeginPlay() override;

private:
	void MoveCamera(const FVector2D& Direction);
	void ZoomIn();
	void ZoomOut();
	void MoveForward(float Value);
	void MoveRight(float Value);
	void CheckMouseAtScreenEdge();
    
	APlayerController* PlayerController;
};