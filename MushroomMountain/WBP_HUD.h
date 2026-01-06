#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WBP_HUD.generated.h"

UCLASS()
class MUSHROOMMOUNTAIN_API UWBP_HUD : public UUserWidget
{
    GENERATED_BODY()

public:
    // 🔹 MP / EXP 게이지 업데이트
    UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
    void UpdateBars(float MPPercent, float ExpPercent);

    // 🔹 시계 갱신
    UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
    void UpdateClock(float CurrentTime, bool bIsDaytime);

    // 🔹 스킬 슬롯 쿨타임 갱신
    UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
    void UpdateSkillCooldown(FName SkillKey, float Remaining, float Total);

    // 🔹 레벨 표시
    UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
    void UpdateLevel(int32 Level);

    // 🔹 MP 텍스트 갱신
    UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
    void UpdateMPText(int32 CurrentMP, int32 MaxMP);

    UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
    void UpdateExpText(int32 CurrentExp, int32 MaxExp);
};
