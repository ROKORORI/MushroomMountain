#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"          // ✅ DataTable용
#include "GameplayTagContainer.h"      // ✅ GameplayTags
#include "ItemData.generated.h"

// 🔹 아이템 타입 구분
UENUM(BlueprintType)
enum class EItemType : uint8
{
    None        UMETA(DisplayName = "None"),
    Consumable  UMETA(DisplayName = "Consumable"),
    Equipment   UMETA(DisplayName = "Equipment"),
    Material    UMETA(DisplayName = "Material")
};

// 🔹 DataTable용 아이템 데이터 구조체
USTRUCT(BlueprintType)
struct FItemData : public FTableRowBase
{
    GENERATED_BODY();

    // 고유 ID
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    int32 ItemID = -1;

    // 표시 이름
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FString ItemName = TEXT("Unknown");

    // 타입
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    EItemType Type = EItemType::None;

    // 개수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    int32 Quantity = 1;

    // 태그 (Item.Mana, Item.Buff.Damage 등)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FGameplayTagContainer Tags;

    // 효과 값
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
    int32 ManaValue = 0;       // 마나 회복량

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
    int32 AttackValue = 0;     // 공격력 보정

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
    int32 DefenseValue = 0;    // 방어력 보정

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
    int32 ConstructionValue = 0; // 건설 자원 단위
};

