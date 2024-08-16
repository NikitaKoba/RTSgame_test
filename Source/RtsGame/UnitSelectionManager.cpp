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

            if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, Params))
            {
                AActor* HitActor = HitResult.GetActor();

                if (HitActor && HitActor->Implements<USelectable>())
                {
                    ISelectable* SelectableInterface = Cast<ISelectable>(HitActor);

                    if (bIsCtrlPressed)
                    {
                        // Если Ctrl нажат, добавляем или убираем юнит из выделения
                        if (SelectedUnits.Contains(HitActor))
                        {
                            // Снимаем выделение, если юнит уже был выделен
                            if (SelectableInterface)
                            {
                                SelectableInterface->OnDeselected();
                            }
                            SelectedUnits.Remove(HitActor);
                        }
                        else
                        {
                            // Выделяем новый юнит
                            if (SelectableInterface)
                            {
                                SelectableInterface->OnSelected();
                            }
                            SelectedUnits.Add(HitActor);
                        }
                    }
                    else
                    {
                        // Если Ctrl не нажат, снимаем выделение со всех остальных юнитов
                        DeselectAllUnits();

                        // Выделяем новый юнит
                        if (SelectableInterface)
                        {
                            SelectableInterface->OnSelected();
                        }
                        SelectedUnits.Add(HitActor);
                    }
                }
                else
                {
                    // Если кликаем на пустую область, снимаем выделение
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
