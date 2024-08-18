#include "Unit.h"
#include "Components/BillboardComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "AIController.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "Components/DecalComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SplineComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/FloatingPawnMovement.h"


AUnit::AUnit()
{
    PrimaryActorTick.bCanEverTick = true;

    // Создание компонента для отображения индикатора выделения
    PrimaryActorTick.bCanEverTick = true;

    // Настройка индикатора выделения
    SelectionIndicator = CreateDefaultSubobject<UBillboardComponent>(TEXT("SelectionIndicator"));
    SelectionIndicator->SetupAttachment(RootComponent);
    SelectionIndicator->SetVisibility(false);

    // Настройка AI контроллера
    AIControllerClass = AAIController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

    // Настройка декаля выделения
    SelectionDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("SelectionDecal"));
    SelectionDecal->SetupAttachment(RootComponent);
    SelectionDecal->SetVisibility(false);
    SelectionDecal->SetRelativeRotation(FRotator(-90, 0, 0)); // Декаль на земле
    SelectionDecal->DecalSize = FVector(200, 200, 200); // Размер декал

    MovementRadiusDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("MovementRadiusDecal"));
    MovementRadiusDecal->SetupAttachment(RootComponent);
    MovementRadiusDecal->SetVisibility(false);
    MovementRadiusDecal->DecalSize = FVector(500.0f, 500.0f, 500.0f);
    MovementRadiusDecal->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
    
    GetCharacterMovement()->bOrientRotationToMovement = true; // Автоматический поворот персонажа
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 640.0f, 0.0f); // Скорость поворота
    
    MovementRadius = 1000.0f;
    MovementPoints = 10.0f;  // Отключаем поворот через контроллер
    bUseControllerRotationYaw = false;
    PrimaryActorTick.bCanEverTick = true;
    
}
void AUnit::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AUnit::StartNewTurn()
{
    StepsTakenThisTurn = 0;
    bCanMove = true;

    InitialPosition = GetActorLocation(); // Фиксируем начальную позицию в начале хода
    MovementRadius = GetMovementRadius(); // Фиксируем радиус движения в начале хода

    DrawMovementBoundary(); // Отрисовываем границу движения
    UE_LOG(LogTemp, Warning, TEXT("New turn started: bCanMove = %s, MovementRadius = %f"), bCanMove ? TEXT("true") : TEXT("false"), MovementRadius);
}


void AUnit::EndTurn()
{
    bCanMove = false;
    UE_LOG(LogTemp, Warning, TEXT("Turn ended: bCanMove = %s"), bCanMove ? TEXT("true") : TEXT("false"));
    HideMovementRadius();
}


void AUnit::BeginPlay()
{
    Super::BeginPlay();

    // Проверка, есть ли у юнита контроллер
    if (Controller == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("Контроллер не найден для юнита %s. Спавним контроллер..."), *GetName());
        SpawnDefaultController();  // Спавн AIController, если он не установлен

        if (Controller == nullptr)
        {
            UE_LOG(LogTemp, Error, TEXT("Не удалось создать AIController для юнита %s"), *GetName());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("AIController успешно создан для юнита %s"), *GetName());
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Контроллер уже существует для юнита %s"), *GetName());
    }

    // Проверка наличия навигационной системы
    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
    if (NavSys)
    {
        UE_LOG(LogTemp, Warning, TEXT("Навигационная система найдена для юнита %s"), *GetName());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Навигационная система не найдена для юнита %s"), *GetName());
    }
    if (MovementPoints <= 0.0f)
    {
        MovementPoints = 10.0f; // Задайте значение по умолчанию, если оно не было задано
        UE_LOG(LogTemp, Warning, TEXT("MovementPoints не были инициализированы, установлено значение по умолчанию 10"));
    }

    MovementRadius = GetMovementRadius(); // Инициализируем MovementRadius на основе MovementPoints

    // Если необходимо, отрисуйте границу здесь
    DrawMovementBoundary();
    if (MovementRadiusDecal->GetDecalMaterial())
    {
        MovementRadiusMaterial = MovementRadiusDecal->CreateDynamicMaterialInstance();
        MovementRadiusDecal->SetDecalMaterial(MovementRadiusMaterial);
    }
}

void AUnit::UpdateMovementRadius()
{
    float Radius = GetMovementRadius();

    if (MovementRadiusMaterial)
    {
        MovementRadiusMaterial->SetScalarParameterValue(TEXT("Radius"), Radius);
    }

    MovementRadiusDecal->SetWorldLocation(InitialPosition); // Установка радиуса вокруг начальной позиции
    MovementRadiusDecal->DecalSize = FVector(Radius * 2, Radius * 2, 500.0f);
}

void AUnit::ShowMovementRadius()
{
    if (MovementRadiusDecal)
    {
        MovementRadiusDecal->SetVisibility(true);
        UpdateMovementRadius();
    }
}

void AUnit::HideMovementRadius()
{
    if (MovementRadiusDecal)
    {
        MovementRadiusDecal->SetVisibility(false);
    }
}

float AUnit::GetMovementRadius() const
{
    // Например, если каждое очко хода позволяет перемещаться на 100 единиц в игре.
    return MovementPoints * 100.0f;
}


void AUnit::OnSelected()
{
    // Граница передвижения фиксируется при старте нового хода, так что здесь ничего не меняем
    if (SelectionDecal)
    {
        SelectionDecal->SetVisibility(true);
     
    }

    // Отрисовываем границу без изменения позиции
    DrawMovementBoundary();
}

void AUnit::ShowMovementPath(const FVector& TargetLocation)
{
    // Создаем и настраиваем SplineComponent для отображения пути.
    if (!MovementPath)
    {
        MovementPath = NewObject<USplineComponent>(this, TEXT("MovementPath"));
        MovementPath->SetupAttachment(RootComponent);
    }

    // Очистка и настройка SplineComponent
    MovementPath->ClearSplinePoints();
    MovementPath->AddSplinePoint(GetActorLocation(), ESplineCoordinateSpace::World);
    MovementPath->AddSplinePoint(TargetLocation, ESplineCoordinateSpace::World);

    // Визуализация Spline (можно использовать материал или цветную линию)
    // Для этого может понадобиться дополнительный Blueprint или код для отрисовки линии.
}

void AUnit::OnDeselected()
{
    if (SelectionDecal)
    {
        SelectionDecal->SetVisibility(false);
    }
    UE_LOG(LogTemp, Warning, TEXT("Unit deselected"));
}

void AUnit::MoveToLocation(const FVector& TargetLocation)
{
    if (!bCanMove)
    {
        UE_LOG(LogTemp, Warning, TEXT("Юнит больше не может двигаться в этом ходу."));
        return;
    }

    FVector CurrentLocation = GetActorLocation();
    FVector FlatTargetLocation = FVector(TargetLocation.X, TargetLocation.Y, CurrentLocation.Z);

    if (!IsWithinMovementRadius(FlatTargetLocation, InitialPosition))
    {
        UE_LOG(LogTemp, Warning, TEXT("Целевая точка находится за пределами радиуса движения."));
        return;
    }

    float Distance = FVector::Dist(FlatTargetLocation, InitialPosition);

    if (!HasEnoughSteps(Distance))
    {
        UE_LOG(LogTemp, Warning, TEXT("Недостаточно хода для перемещения в указанное место."));
        return;
    }

    StepsTakenThisTurn += Distance;

    if (!AttemptMove(FlatTargetLocation))
    {
        LogMoveError("Не удалось выполнить перемещение. Возможно, путь недоступен или проблемы с NavMesh.");
        return;
    }

    // Граница не обновляется после каждого движения, оставаясь вокруг начальной позиции
}


bool AUnit::IsWithinMovementRadius(const FVector& TargetLocation, const FVector& CurrentLocation) const
{
    float Distance = FVector::Dist(TargetLocation, CurrentLocation);
    return Distance <= MovementRadius;
}

bool AUnit::HasEnoughSteps(float Distance) const
{
    return (StepsTakenThisTurn + Distance) <= MovementRadius;
}

bool AUnit::AttemptMove(const FVector& TargetLocation)
{
    AAIController* AIController = Cast<AAIController>(GetController());
    if (!AIController)
    {
        LogMoveError("AIController не найден для юнита.");
        return false;
    }

    UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    if (!NavSys)
    {
        LogMoveError("NavMesh не найден для юнита.");
        return false;
    }

    FVector Direction = (TargetLocation - GetActorLocation()).GetSafeNormal();
    SetActorRotation(FRotationMatrix::MakeFromX(Direction).Rotator());

    FNavLocation UnitNavLocation, TargetNavLocation;
    if (!NavSys->ProjectPointToNavigation(GetActorLocation(), UnitNavLocation) ||
        !NavSys->ProjectPointToNavigation(TargetLocation, TargetNavLocation))
    {
        LogMoveError("Целевая точка или текущая позиция не находится на NavMesh.");
        return false;
    }

    EPathFollowingRequestResult::Type MoveResult = AIController->MoveToLocation(TargetNavLocation.Location);
    return MoveResult == EPathFollowingRequestResult::RequestSuccessful;
}
void AUnit::DrawMovementBoundary()
{
    FVector Center = InitialPosition;  // Центр границы — начальная позиция юнита
    float Radius = MovementRadius;      // Радиус границы, зафиксированный в начале хода

    // Отрисовка круга с заданным радиусом на 0.1 секунды
    DrawDebugCircle(GetWorld(), Center, Radius, 50, FColor::Green, false, 0.1f, 0, 10.0f, FVector(1, 0, 0), FVector(0, 1, 0), true);
}


void AUnit::LogMoveWarning(const FString& Message) const
{
    UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);
}

void AUnit::LogMoveError(const FString& Message) const
{
    UE_LOG(LogTemp, Error, TEXT("%s"), *Message);
}
