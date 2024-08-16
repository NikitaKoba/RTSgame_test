#include "UnitSelectionManager.h"
#include "Selectable.h"
#include "Unit.h"
#include "Engine/HitResult.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

AUnitSelectionManager::AUnitSelectionManager()
{
}


void AUnitSelectionManager::SelectUnitAtMousePosition(APlayerController* PlayerController, bool bIsCtrlPressed)
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

            // Добавляем отладочную линию
            GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, Params);
            DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 2.0f, 0, 1.0f);

            if (HitResult.bBlockingHit)
            {
                AActor* HitActor = HitResult.GetActor();
                UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *HitActor->GetName());

                if (HitActor && HitActor->Implements<USelectable>())
                {
                    ISelectable* SelectableInterface = Cast<ISelectable>(HitActor);

                    if (bIsCtrlPressed)
                    {
                        if (SelectedUnits.Contains(HitActor))
                        {
                            if (SelectableInterface)
                            {
                                SelectableInterface->OnDeselected();
                            }
                            SelectedUnits.Remove(HitActor);
                        }
                        else
                        {
                            if (SelectableInterface)
                            {
                                SelectableInterface->OnSelected();
                            }
                            SelectedUnits.Add(HitActor);
                        }
                    }
                    else
                    {
                        DeselectAllUnits();

                        if (SelectableInterface)
                        {
                            SelectableInterface->OnSelected();
                        }
                        SelectedUnits.Add(HitActor);
                    }
                }
                else
                {
                    DeselectAllUnits();
                }
            }
        }
    }
}
void AUnitSelectionManager::DeselectAllUnits()
{
    for (AActor* Unit : SelectedUnits)
    {
        if (Unit && Unit->Implements<USelectable>())
        {
            ISelectable* SelectableInterface = Cast<ISelectable>(Unit);
            if (SelectableInterface)
            {
                SelectableInterface->OnDeselected();
            }
        }
    }
    SelectedUnits.Empty(); // Очищаем список выделенных юнитов
}
void AUnitSelectionManager::MoveSelectedUnitsToLocation(const FVector& TargetLocation)
{
    for (AActor* Unit : SelectedUnits)
    {
        AUnit* UnitToMove = Cast<AUnit>(Unit);
        if (UnitToMove)
        {
            UnitToMove->MoveToLocation(TargetLocation);
        }
    }
}