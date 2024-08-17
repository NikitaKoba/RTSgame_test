#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EconomicSettlement.generated.h"

UENUM(BlueprintType)
enum class EResourceType : uint8
{
    Gold    UMETA(DisplayName = "Gold"),
    Food    UMETA(DisplayName = "Food"),
    Wood    UMETA(DisplayName = "Wood"),
    Stone   UMETA(DisplayName = "Stone")
};

UCLASS()
class RTSGAME_API AEconomicSettlement : public AActor
{
    GENERATED_BODY()

public:
    AEconomicSettlement();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Economy")
    int32 Population;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Economy")
    int32 Gold;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Economy")
    int32 Food;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Economy")
    int32 Wood;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Economy")
    int32 Stone;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Economy")
    float TaxRate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Economy")
    float GrowthRate;

    UFUNCTION(BlueprintCallable, Category = "Economy")
    void UpdateEconomy(float DeltaTime);

    UFUNCTION(BlueprintCallable, Category = "Economy")
    void TradeWith(AEconomicSettlement* OtherSettlement, int32 ResourceAmount, EResourceType ResourceType);

    UFUNCTION(BlueprintCallable, Category = "Economy")
    FString GetEconomyInfo() const;

    UFUNCTION(BlueprintCallable, Category = "Economy")
    int32 GetResourceAmount(EResourceType ResourceType) const;

    UFUNCTION(BlueprintCallable, Category = "Economy")
    void ModifyResource(EResourceType ResourceType, int32 Amount);
    void UpdateEconomyForTurn();
    void ShowNextTurnProjections();
    int32 CurrentTurn;          // Текущий ход
    int32 TurnsPerYear;         // Количество ходов в году (например, 2)
    float TimeBetweenTurns;     // Время между ходами
    float TimeSinceLastTurn; 
    float TimeSinceLastUpdate;
    const float UpdateInterval = 1.0f; 

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

private:
    void CollectTaxes();
    void ConsumeResources();
    void GrowPopulation();
    void EndTurn();
};
