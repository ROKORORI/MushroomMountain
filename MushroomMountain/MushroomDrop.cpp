#include "MushroomDrop.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/TextRenderComponent.h"
#include "MushroomCharacter.h"
#include "MushroomMountainGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/PlayerCameraManager.h"

AMushroomDrop::AMushroomDrop()
{
    PrimaryActorTick.bCanEverTick = true;

    Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
    SetRootComponent(Collision);
    Collision->InitSphereRadius(80.f);
    Collision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(Collision);
    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // ❌ 기본 Sphere 메쉬 하드코딩 제거 → BP에서 지정

    PickupText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("PickupText"));
    PickupText->SetupAttachment(RootComponent);
    PickupText->SetHorizontalAlignment(EHTA_Center);
    PickupText->SetWorldSize(30.f);
    PickupText->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
    PickupText->SetVisibility(false);
}

// ✅ 드랍 생성 시 바로 아이템 데이터 초기화
void AMushroomDrop::Initialize(int32 InItemID)
{
    ItemID = InItemID;

    if (AMushroomMountainGameMode* GM = Cast<AMushroomMountainGameMode>(UGameplayStatics::GetGameMode(this)))
    {
        if (GM->ItemDataTable)
        {
            ItemData = GM->GetItemDataByID(ItemID);
            if (ItemData.ItemID == -1)
            {
                UE_LOG(LogTemp, Warning, TEXT("❌ ItemID %d not found in DataTable!"), ItemID);
            }
            else
            {
                UE_LOG(LogTemp, Log, TEXT("✅ Drop Initialized: %s (ID %d)"),
                    *ItemData.ItemName, ItemData.ItemID);
            }
        }
    }
}

void AMushroomDrop::BeginPlay()
{
    Super::BeginPlay();

    // 에디터에서 직접 배치한 경우 ItemID를 수동으로 지정했을 수 있으니 보정
    if (ItemID != -1 && ItemData.ItemID == -1)
    {
        if (AMushroomMountainGameMode* GM = Cast<AMushroomMountainGameMode>(UGameplayStatics::GetGameMode(this)))
        {
            ItemData = GM->GetItemDataByID(ItemID);
        }
    }
}

void AMushroomDrop::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    // 텍스트가 카메라를 향하도록 회전
    if (PickupText && PickupText->IsVisible())
    {
        APlayerCameraManager* CamMgr = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
        if (CamMgr)
        {
            FVector CamLoc = CamMgr->GetCameraLocation();
            FVector ToCam = CamLoc - PickupText->GetComponentLocation();
            FRotator LookAtRot = FRotationMatrix::MakeFromX(ToCam).Rotator();
            PickupText->SetWorldRotation(LookAtRot);
        }
    }
}

void AMushroomDrop::NotifyActorBeginOverlap(AActor* OtherActor)
{
    Super::NotifyActorBeginOverlap(OtherActor);

    if (AMushroomCharacter* Player = Cast<AMushroomCharacter>(OtherActor))
    {
        Player->SetOverlappingDrop(this);

        if (PickupText)
        {
            FString DisplayText = FString::Printf(TEXT("Press F to pick up %s (x%d)"),
                *ItemData.ItemName, ItemData.Quantity);

            PickupText->SetText(FText::FromString(DisplayText));
            PickupText->SetVisibility(true);
        }
    }
}

void AMushroomDrop::NotifyActorEndOverlap(AActor* OtherActor)
{
    Super::NotifyActorEndOverlap(OtherActor);

    if (AMushroomCharacter* Player = Cast<AMushroomCharacter>(OtherActor))
    {
        Player->SetOverlappingDrop(nullptr);

        if (PickupText)
        {
            PickupText->SetVisibility(false);
        }
    }
}
