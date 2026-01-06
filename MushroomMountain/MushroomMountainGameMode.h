#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Engine/DataTable.h"   // ✅ DataTable 사용
#include "ItemData.h"           // ✅ FItemData 구조체
#include "MushroomMountainGameMode.generated.h"

UCLASS(minimalapi)
class AMushroomMountainGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AMushroomMountainGameMode();

    // 하루 시간 (0.0 ~ 24.0)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
    float CurrentTimeOfDay = 6.0f; // 아침 6시부터 시작

    // 시간 진행 속도 (1초에 몇 시간 진행?)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
    float TimeScale = 0.1f; // 1초 = 0.1시간 → 10분 = 1일

    // 낮/밤 판별 함수
    UFUNCTION(BlueprintCallable, Category = "Time")
    bool IsDaytime() const;

    // 🔹 Tick 함수 오버라이드
    virtual void Tick(float DeltaSeconds) override;

    // ✅ 아이템 데이터테이블 (에디터에서 할당)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    UDataTable* ItemDataTable;

    // ✅ ItemID로 아이템 데이터 가져오기
    UFUNCTION(BlueprintCallable, Category = "Item")
    FItemData GetItemDataByID(int32 ItemID) const;
};
