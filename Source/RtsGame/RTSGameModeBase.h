#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RTSGameModeBase.generated.h"

UCLASS()
class RTSGAME_API ARTSGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ARTSGameModeBase();  // Объявление конструктора
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UUserWidget> EndTurnWidgetClass;

	UPROPERTY()
	UUserWidget* EndTurnWidget;
	
	void EndTurn();
	void StartNewTurn();
	virtual void BeginPlay() override;
private:
	int32 CurrentTurn;  // Объявление переменной CurrentTurn
};