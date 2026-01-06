#include "MushroomActor.h"
#include "MushroomDrop.h"
#include "MushroomCharacter.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"

AMushroomActor::AMushroomActor()
{
    PrimaryActorTick.bCanEverTick = false;

    // 1) Collision = 루트 컴포넌트
    Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
    RootComponent = Collision;
    Collision->InitBoxExtent(FVector(40.f, 40.f, 50.f));
    Collision->SetCollisionProfileName(TEXT("BlockAllDynamic"));
    Collision->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

    // 2) Static Mesh (비주얼)
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(RootComponent);
    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Mesh->SetRelativeLocation(FVector(0.f, 0.f, -50.f));
    Mesh->SetRelativeScale3D(FVector(0.9f));

    // ❌ 기본 Cylinder 메쉬 하드코딩 제거 → BP에서 지정
    // DropClass는 에디터에서 세팅
}

float AMushroomActor::TakeDamage(float DamageAmount,
    const FDamageEvent& DamageEvent,
    AController* EventInstigator,
    AActor* DamageCauser)
{
    Health -= DamageAmount;
    Health = FMath::Max(0.f, Health); // 음수 방지

    // HP 디버그 출력
    DrawDebugString(GetWorld(),
        GetActorLocation() + FVector(0, 0, 120.f),
        FString::Printf(TEXT("HP: %.0f"), Health),
        nullptr,
        (Health > 0.f ? FColor::Yellow : FColor::Red),
        1.0f,
        true);

    if (Health <= 0.f)
    {
        // ✅ 처치한 플레이어에게 경험치 +5
        if (EventInstigator)
        {
            if (AMushroomCharacter* Player = Cast<AMushroomCharacter>(EventInstigator->GetPawn()))
            {
                Player->GainExp(5);
            }
        }

        // ✅ 드랍 생성
        if (DropClass)
        {
            FVector SpawnLoc = GetActorLocation() + FVector(0, 0, 50.f);

            AMushroomDrop* Drop = GetWorld()->SpawnActor<AMushroomDrop>(
                DropClass,
                SpawnLoc,
                FRotator::ZeroRotator
            );

            if (Drop)
            {
                int32 RandID = FMath::RandRange(1, 10);
                Drop->Initialize(RandID);

                UE_LOG(LogTemp, Log, TEXT("드랍 스폰 성공: %s at %s (ItemID %d)"),
                    *Drop->GetName(),
                    *SpawnLoc.ToString(),
                    RandID);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("드랍 스폰 실패! DropClass가 올바른지 확인 필요"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("DropClass가 설정되지 않았습니다."));
        }

        // ✅ 10초 뒤 리스폰 예약
        if (UWorld* World = GetWorld())
        {
            FVector RespawnLoc = GetActorLocation();
            FRotator RespawnRot = GetActorRotation();
            TSubclassOf<AMushroomActor> ActorClass = GetClass();

            FTimerHandle TimerHandle;
            World->GetTimerManager().SetTimer(
                TimerHandle,
                FTimerDelegate::CreateLambda([World, ActorClass, RespawnLoc, RespawnRot]()
                    {
                        World->SpawnActor<AMushroomActor>(ActorClass, RespawnLoc, RespawnRot);
                    }),
                10.0f, false
            );
        }

        Destroy(); // 자기 자신 삭제
    }

    return DamageAmount;
}


