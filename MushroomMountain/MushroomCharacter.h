#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ItemData.h"
#include "MushroomCharacter.generated.h"

class AMushroomDrop;
class UWBP_Status; // 🔹 스탯창 UI 위젯 클래스
class UWBP_HUD;    // 🔹 HUD UI 위젯 클래스

// 🔹 캐릭터 스탯 구조체
USTRUCT(BlueprintType)
struct FCharacterStats
{
    GENERATED_BODY()

    // 현재 마나 (실시간 변동)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float CurrentMP = 50.0f;

    // 최대 마나
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    int32 MaxMP = 100;

    // 공격력
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    int32 AttackPower = 10;

    // 방어력
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    int32 Defense = 0;

    // 마나 재생 속도 (초당 회복량)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float ManaRegenRate = 7.0f;
};

UCLASS()
class MUSHROOMMOUNTAIN_API AMushroomCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AMushroomCharacter();

    // ===== Components / Camera =====
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh)
    class USkeletalMeshComponent* CharacterMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
    class USpringArmComponent* SpringArm;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
    class UCameraComponent* Camera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
    float BaseTurnRate;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
    float BaseLookUpRate;

    // ===== Inventory / Stats / Growth =====
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
    TArray<FItemData> Inventory;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    FCharacterStats Stats;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Growth")
    int32 Level = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Growth")
    int32 CurrentExp = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Growth")
    int32 MaxExp = 100;

    // ===== API =====
    void SetOverlappingDrop(class AMushroomDrop* Drop);

    UFUNCTION()
    void UseItem(int32 Index);

    UFUNCTION()
    void UseFirstItem();

    UFUNCTION(BlueprintCallable, Category = "Growth")
    void GainExp(int32 Amount);

    // ===== UI / HUD =====
    // 🔹 스탯창 열림 여부 플래그
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
    bool bStatusUIOpen = false;

    // 🔹 스탯창/상태창/HUD 갱신
    void RefreshStatusUI();

    // 🔹 HUD Getter (외부에서 HUD에 접근할 때 사용)
    UWBP_HUD* GetHUD() const { return HUDUI; }

protected:
    // ===== 기본 오버라이드 =====
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // ===== Movement =====
    void MoveForward(float Value);
    void MoveBackward(float Value);
    void MoveRight(float Value);
    void MoveLeft(float Value);
    void TurnAtRate(float Rate);
    void LookUpAtRate(float Rate);

    // ===== Combat / Action =====
    void OnPrimaryAction();
    void HandlePickup();
    void ShowInventory();

    // ===== Combat Params =====
    UPROPERTY(EditAnywhere, Category = "Combat")
    float ClickDamage = 10.f;

    UPROPERTY(EditAnywhere, Category = "Combat")
    float ClickRange = 500.f;

    UPROPERTY(EditAnywhere, Category = "Combat")
    float ClickRadius = 16.f;

    // ===== UI: Status Window =====
    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<class UWBP_Status> StatusWidgetClass;

    UPROPERTY()
    UWBP_Status* StatusUI = nullptr;

    UFUNCTION(BlueprintCallable, Category = "UI")
    void ToggleStatusUI();

    // ===== UI: HUD (항상 표시) =====
    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<class UWBP_HUD> HUDWidgetClass;  // BP_HUD 할당용

    UPROPERTY()
    UWBP_HUD* HUDUI = nullptr;                   // 생성된 HUD 위젯

private:
    // 현재 겹친 드랍 아이템
    AMushroomDrop* OverlappingDrop = nullptr;
};
