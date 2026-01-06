#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"   // ✅ UButton 정의 추가

#include "WBP_Status.generated.h"

UCLASS()
class MUSHROOMMOUNTAIN_API UWBP_Status : public UUserWidget
{
    GENERATED_BODY()

public:
    // 🔹 스탯창 업데이트: 필요한 값만 전달
    UFUNCTION(BlueprintImplementableEvent, Category = "UI")
    void UpdateStats(int32 MaxMP, int32 Attack, int32 Defense, float RegenRate);

    UPROPERTY(meta = (BindWidgetOptional))
    UButton* Button_Close;

};

