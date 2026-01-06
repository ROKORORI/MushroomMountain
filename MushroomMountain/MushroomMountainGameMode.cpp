#include "MushroomMountainGameMode.h"
#include "MyPlayerController.h"
#include "MushroomCharacter.h" 
#include "WBP_HUD.h"

#include "Engine/Engine.h"       // 🔹 GEngine 디버그 출력용
#include "Engine/DataTable.h"    // ✅ DataTable 사용

AMushroomMountainGameMode::AMushroomMountainGameMode()
{
    PrimaryActorTick.bCanEverTick = true;

    // 🖱 커스텀 PlayerController 사용
    PlayerControllerClass = AMyPlayerController::StaticClass();
}

void AMushroomMountainGameMode::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    // 🔹 시간 진행
    CurrentTimeOfDay += DeltaSeconds * TimeScale;
    if (CurrentTimeOfDay >= 24.f)
        CurrentTimeOfDay = 0.f;

    // 🔹 모든 플레이어의 HUD 시계 갱신
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        if (APlayerController* PC = It->Get())
        {
            if (AMushroomCharacter* Player = Cast<AMushroomCharacter>(PC->GetPawn()))
            {
                if (Player->GetHUD())  // ✅ Getter 사용
                {
                    Player->GetHUD()->UpdateClock(CurrentTimeOfDay, IsDaytime());
                }
            }
        }
    }
}

bool AMushroomMountainGameMode::IsDaytime() const
{
    return CurrentTimeOfDay >= 6.f && CurrentTimeOfDay < 18.f;
}

FItemData AMushroomMountainGameMode::GetItemDataByID(int32 ItemID) const
{
    if (!ItemDataTable) return FItemData();

    static const FString Context(TEXT("ItemDataLookup"));
    for (const FName& RowName : ItemDataTable->GetRowNames())
    {
        if (const FItemData* Row = ItemDataTable->FindRow<FItemData>(RowName, Context))
        {
            UE_LOG(LogTemp, Log, TEXT("RowName=%s, Row->ItemID=%d, LookingFor=%d"),
                *RowName.ToString(), Row->ItemID, ItemID);

            if (Row->ItemID == ItemID)
            {
                return *Row;
            }
        }
    }
    return FItemData(); // 못 찾으면 빈 데이터
}


