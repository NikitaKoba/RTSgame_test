#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Selectable.generated.h"

UINTERFACE(MinimalAPI)
class USelectable : public UInterface
{
	GENERATED_BODY()
};

class RTSGAME_API ISelectable
{
	GENERATED_BODY()

public:
	// Метод, который будет вызываться при выделении юнита
	virtual void OnSelected() = 0;

	// Метод, который будет вызываться при снятии выделения
	virtual void OnDeselected() = 0;
};
