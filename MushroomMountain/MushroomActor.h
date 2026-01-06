#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MushroomActor.generated.h"

class AMushroomDrop;

UCLASS()
class MUSHROOMMOUNTAIN_API AMushroomActor : public AActor
{
    GENERATED_BODY()

public:
    AMushroomActor();

    // UE Damage 시스템 훅 (UGameplayStatics::ApplyDamage 로 들어옴)
    virtual float TakeDamage(float DamageAmount,
        struct FDamageEvent const& DamageEvent,
        class AController* EventInstigator,
        AActor* DamageCauser) override;

    // 남은 체력 확인용
    UFUNCTION(BlueprintCallable, Category = "Mushroom")
    float GetHealth() const { return Health; }

protected:
    // 충돌 (루트)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mushroom")
    class UBoxComponent* Collision;

    // 시각 메쉬 (BP에서 지정)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mushroom")
    class UStaticMeshComponent* Mesh;

    // 체력
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mushroom")
    float Health = 50.f;

    // 드랍 아이템 설정
    UPROPERTY(EditAnywhere, Category = "Drop")
    TSubclassOf<class AMushroomDrop> DropClass;

    UPROPERTY(EditAnywhere, Category = "Drop", meta = (ClampMin = "1"))
    int32 DropCount = 1;   // 현재는 항상 1개 드랍
};

