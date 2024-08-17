#include "EconomicSettlement.h"

AEconomicSettlement::AEconomicSettlement()
{
	PrimaryActorTick.bCanEverTick = true;

	// Инициализация значений по умолчанию
	Population = 1000;
	Gold = 5000;
	Food = 1000;
	Wood = 500;
	Stone = 250;
	TaxRate = 0.1f;
	GrowthRate = 0.01f;
}

void AEconomicSettlement::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("Initial Population: %d"), Population);
}

void AEconomicSettlement::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
void AEconomicSettlement::UpdateEconomy(float DeltaTime)
{
	//UE_LOG(LogTemp, Warning, TEXT("Before UpdateEconomy - Population: %d, Gold: %d"), Population, Gold);

	CollectTaxes();
	ConsumeResources();
	GrowPopulation();

	//UE_LOG(LogTemp, Warning, TEXT("After UpdateEconomy - Population: %d, Gold: %d"), Population, Gold);
}

void AEconomicSettlement::CollectTaxes()
{
	int32 TaxesCollected = FMath::FloorToInt(Population * TaxRate);
	Gold += TaxesCollected;
}

void AEconomicSettlement::ConsumeResources()
{
	int32 FoodConsumed = FMath::FloorToInt(Population * 0.1f);
	Food -= FoodConsumed;
	Food = FMath::Max(0, Food);
}

void AEconomicSettlement::GrowPopulation()
{
	if (Food > Population * 0.1f)
	{
		int32 NewPopulation = FMath::FloorToInt(Population * (1 + GrowthRate));
		UE_LOG(LogTemp, Warning, TEXT("Growing Population from %d to %d"), Population, NewPopulation);
		Population = NewPopulation;
	}
	else
	{
		int32 PopulationLoss = FMath::FloorToInt(Population * 0.05f);
		UE_LOG(LogTemp, Warning, TEXT("Population Loss: %d"), PopulationLoss);
		Population = FMath::Max(0, Population - PopulationLoss);
	}
}
void AEconomicSettlement::EndTurn()
{
	// Обновление экономики
	UpdateEconomyForTurn();

	// Показать прогноз на следующий ход
	ShowNextTurnProjections();

	// Сброс времени для следующего хода, если нужно
	TimeSinceLastTurn = 0.0f;
}

void AEconomicSettlement::TradeWith(AEconomicSettlement* OtherSettlement, int32 ResourceAmount, EResourceType ResourceType)
{
	if (!OtherSettlement || ResourceAmount <= 0)
		return;

	int32 AvailableAmount = GetResourceAmount(ResourceType);
	if (AvailableAmount >= ResourceAmount)
	{
		ModifyResource(ResourceType, -ResourceAmount);
		OtherSettlement->ModifyResource(ResourceType, ResourceAmount);
		UE_LOG(LogTemp, Log, TEXT("%s traded %d %s to %s"), *GetName(), ResourceAmount, *UEnum::GetValueAsString(ResourceType), *OtherSettlement->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s doesn't have enough %s to trade"), *GetName(), *UEnum::GetValueAsString(ResourceType));
	}
}

FString AEconomicSettlement::GetEconomyInfo() const
{
	return FString::Printf(TEXT("Population: %d\nGold: %d\nFood: %d\nWood: %d\nStone: %d\nTax Rate: %.2f\nGrowth Rate: %.2f"),
		Population, Gold, Food, Wood, Stone, TaxRate, GrowthRate);
}

int32 AEconomicSettlement::GetResourceAmount(EResourceType ResourceType) const
{
	switch (ResourceType)
	{
	case EResourceType::Gold:
		return Gold;
	case EResourceType::Food:
		return Food;
	case EResourceType::Wood:
		return Wood;
	case EResourceType::Stone:
		return Stone;
	default:
		return 0;
	}
}

void AEconomicSettlement::ModifyResource(EResourceType ResourceType, int32 Amount)
{
	switch (ResourceType)
	{
	case EResourceType::Gold:
		Gold += Amount;
		break;
	case EResourceType::Food:
		Food += Amount;
		break;
	case EResourceType::Wood:
		Wood += Amount;
		break;
	case EResourceType::Stone:
		Stone += Amount;
		break;
	}
}
void AEconomicSettlement::UpdateEconomyForTurn()
{
	// Логика обновления экономики
	CollectTaxes();
	ConsumeResources();
	GrowPopulation();

	UE_LOG(LogTemp, Warning, TEXT("Экономика обновлена: население %d, золото %d"), Population, Gold);
}
void AEconomicSettlement::ShowNextTurnProjections()
{
	// Сохранение текущих значений
	int32 TempPopulation = Population;
	int32 TempGold = Gold;
	int32 TempFood = Food;

	// Применение изменений
	CollectTaxes();
	ConsumeResources();
	GrowPopulation();

	// Восстановление оригинальных значений
	Population = TempPopulation;
	Gold = TempGold;
	Food = TempFood;
}