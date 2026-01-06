#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemData.h"
#include "MushroomDrop.generated.h"

UCLASS()
class MUSHROOMMOUNTAIN_API AMushroomDrop : public AActor
{
    GENERATED_BODY()

public:
    AMushroomDrop();

    // ✅ 드랍 생성 시점에 호출해서 아이템 데이터 초기화
    void Initialize(int32 InItemID);

protected:
    // 충돌 범위
    UPROPERTY(VisibleAnywhere, Category = "Drop")
    class USphereComponent* Collision;

    // 드랍 메쉬 (BP에서 지정)
    UPROPERTY(VisibleAnywhere, Category = "Drop")
    class UStaticMeshComponent* Mesh;

    // 아이템 이름 표시
    UPROPERTY(VisibleAnywhere, Category = "Drop")
    class UTextRenderComponent* PickupText;

    virtual void BeginPlay() override;

public:
    // 아이템 고유 ID
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop")
    int32 ItemID = -1;

    // 아이템 데이터 (DataTable에서 로드됨)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drop")
    FItemData ItemData;

    virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
    virtual void NotifyActorEndOverlap(AActor* OtherActor) override;
    virtual void Tick(float DeltaSeconds) override;
};


