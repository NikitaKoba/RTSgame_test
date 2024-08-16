#include "CameraPawn.h"

#include "Components/InputComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Windows/AllowWindowsPlatformTypes.h"
#include <windows.h>

#include "Unit.h"
#include "UnitSelectionManager.h"
#include "Windows/HideWindowsPlatformTypes.h"
#include "Kismet/GameplayStatics.h"



ARTSCameraPawn::ARTSCameraPawn()
{
    PrimaryActorTick.bCanEverTick = true;

    MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComponent"));
    
    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
    CameraComponent->SetupAttachment(RootComponent);

    MovementSpeed = 2000.0f;
    ZoomSpeed = 50.0f;
    MinZoomHeight = 500.0f;
    MaxZoomHeight = 5000.0f;
    EdgeScrollThreshold = 10.0f;
    CameraAngle = -60.0f;
    EdgeScrollSpeed = 2.0f;
    WASDMovementSpeed = 3000.0f;

    UnitSelectionManager = CreateDefaultSubobject<AUnitSelectionManager>(TEXT("UnitSelectionManager"));
}

void ARTSCameraPawn::BeginPlay()
{
    Super::BeginPlay();
    
    PlayerController = UGameplayStatics::GetPlayerController(this, 0);
    if (PlayerController)
    {
        PlayerController->bShowMouseCursor = true;
        PlayerController->bEnableClickEvents = true;
        PlayerController->bEnableMouseOverEvents = true;
    }

    FRotator NewRotation = FRotator(CameraAngle, 0.0f, 0.0f);
    CameraComponent->SetRelativeRotation(NewRotation);
    
    FVector NewLocation = FVector(0.0f, 0.0f, MaxZoomHeight);
    SetActorLocation(NewLocation);
}

void ARTSCameraPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    CheckMouseAtScreenEdge();
}

void ARTSCameraPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAction("ZoomIn", IE_Pressed, this, &ARTSCameraPawn::ZoomIn);
    PlayerInputComponent->BindAction("ZoomOut", IE_Pressed, this, &ARTSCameraPawn::ZoomOut);
    PlayerInputComponent->BindAxis("MoveForward", this, &ARTSCameraPawn::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &ARTSCameraPawn::MoveRight);
    PlayerInputComponent->BindAction("SelectUnit", IE_Pressed, this, &ARTSCameraPawn::SelectUnit);
    PlayerInputComponent->BindAction("MoveUnit", IE_Pressed, this, &ARTSCameraPawn::MoveSelectedUnits); // Новая привязка для ПКМ
}

void ARTSCameraPawn::SelectUnit()
{
    if (UnitSelectionManager && PlayerController)
    {
        bool bIsCtrlPressed = PlayerController->IsInputKeyDown(EKeys::LeftControl) || PlayerController->IsInputKeyDown(EKeys::RightControl);
        UnitSelectionManager->SelectUnitAtMousePosition(PlayerController, bIsCtrlPressed);
    }
}
void ARTSCameraPawn::MoveCamera(const FVector2D& Direction)
{
    FVector RightVector = CameraComponent->GetRightVector();
    FVector ForwardVector = CameraComponent->GetForwardVector();
    
    FVector Movement = (ForwardVector * Direction.Y + RightVector * Direction.X) * MovementSpeed * GetWorld()->GetDeltaSeconds();
    
    FVector NewLocation = GetActorLocation() + Movement;
    NewLocation.Z = GetActorLocation().Z;
    
    SetActorLocation(NewLocation);
}

void ARTSCameraPawn::ZoomIn()
{
    if (PlayerController)
    {
        FVector2D MousePosition;
        if (PlayerController->GetMousePosition(MousePosition.X, MousePosition.Y))
        {
            FVector WorldLocation, WorldDirection;
            PlayerController->DeprojectScreenPositionToWorld(MousePosition.X, MousePosition.Y, WorldLocation, WorldDirection);

            FVector CameraLocation = GetActorLocation();
            FVector TargetLocation = CameraLocation + WorldDirection * ZoomSpeed;

            TargetLocation.Z = FMath::Clamp(TargetLocation.Z, MinZoomHeight, MaxZoomHeight);

            SetActorLocation(TargetLocation);
        }
    }
}
void ARTSCameraPawn::MoveSelectedUnits()
{
    if (PlayerController)
    {
        FVector2D MousePosition;
        if (PlayerController->GetMousePosition(MousePosition.X, MousePosition.Y))
        {
            FVector WorldLocation, WorldDirection;
            PlayerController->DeprojectScreenPositionToWorld(MousePosition.X, MousePosition.Y, WorldLocation, WorldDirection);

            FVector TraceStart = WorldLocation;
            FVector TraceEnd = WorldLocation + (WorldDirection * 10000.0f);

            FHitResult HitResult;
            FCollisionQueryParams Params;
            Params.AddIgnoredActor(this);

            if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, Params))
            {
                FVector HitLocation = HitResult.Location;
                UnitSelectionManager->MoveSelectedUnitsToLocation(HitLocation);
            }
        }
    }
}

void ARTSCameraPawn::ZoomOut()
{
    if (PlayerController)
    {
        FVector2D MousePosition;
        if (PlayerController->GetMousePosition(MousePosition.X, MousePosition.Y))
        {
            FVector WorldLocation, WorldDirection;
            PlayerController->DeprojectScreenPositionToWorld(MousePosition.X, MousePosition.Y, WorldLocation, WorldDirection);

            FVector CameraLocation = GetActorLocation();
            FVector TargetLocation = CameraLocation + WorldDirection * -ZoomSpeed;

            TargetLocation.Z = FMath::Clamp(TargetLocation.Z, MinZoomHeight, MaxZoomHeight);

            SetActorLocation(TargetLocation);
        }
    }
}

void ARTSCameraPawn::MoveForward(float Value)
{
    if (Value != 0.0f)
    {
        FVector Direction = FVector::ZeroVector;
        Direction.X = CameraComponent->GetForwardVector().X;
        Direction.Y = CameraComponent->GetForwardVector().Y;
        AddMovementInput(Direction, Value * WASDMovementSpeed / MovementSpeed);
    }
}

void ARTSCameraPawn::MoveRight(float Value)
{
    if (Value != 0.0f)
    {
        FVector Direction = FVector::ZeroVector;
        Direction.X = CameraComponent->GetRightVector().X;
        Direction.Y = CameraComponent->GetRightVector().Y;
        AddMovementInput(Direction, Value * WASDMovementSpeed / MovementSpeed);
    }
}

void ARTSCameraPawn::CheckMouseAtScreenEdge()
{
    if (PlayerController)
    {
        int32 ViewportSizeX, ViewportSizeY;
        PlayerController->GetViewportSize(ViewportSizeX, ViewportSizeY);

        float MouseX, MouseY;
        bool bIsMousePositionValid = PlayerController->GetMousePosition(MouseX, MouseY);

        FVector2D MovementDirection(0.0f, 0.0f);

        if (bIsMousePositionValid)
        {
            if (MouseX <= EdgeScrollThreshold)
            {
                MovementDirection.X = -1.0f;
            }
            else if (MouseX >= ViewportSizeX - EdgeScrollThreshold)
            {
                MovementDirection.X = 1.0f;
            }

            if (MouseY <= EdgeScrollThreshold)
            {
                MovementDirection.Y = 1.0f;
            }
            else if (MouseY >= ViewportSizeY - EdgeScrollThreshold)
            {
                MovementDirection.Y = -1.0f;
            }
        }

        if (!MovementDirection.IsZero())
        {
            MoveCamera(MovementDirection * EdgeScrollSpeed);
        }
    }
}
