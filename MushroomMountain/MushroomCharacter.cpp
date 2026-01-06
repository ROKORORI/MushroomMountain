#include "MushroomCharacter.h"
#include "MushroomDrop.h"
#include "ItemData.h"
#include "WBP_Status.h"
#include "WBP_HUD.h" // 🔹 HUD 위젯 포함

#include "GameplayTagContainer.h"
#include "GameplayTagsManager.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"
#include "Blueprint/UserWidget.h"   // ✅ UUserWidget / CreateWidget 사용 가능

// ================= Constructor =================
AMushroomCharacter::AMushroomCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    // 🔹 SpringArm 생성 및 설정 (카메라 거리/회전 제어)
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent);
    SpringArm->TargetArmLength = 300.0f;               // 캐릭터와 카메라 거리 (TPS 시점)
    SpringArm->bUsePawnControlRotation = true;         // 컨트롤러 회전 따라감

    // ✅ 카메라 Pivot을 머리/눈높이에 맞추기
    SpringArm->TargetOffset = FVector(0.f, 0.f, 60.f);

    // 🔹 카메라 생성 및 SpringArm에 부착
    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
    Camera->bUsePawnControlRotation = false;

    // 🔹 기본 회전 속도
    BaseTurnRate = 45.f;
    BaseLookUpRate = 45.f;

    // 🔹 기본 Mesh 숨기기 (Root Mesh는 비활성)
    GetMesh()->SetVisibility(false);
    GetMesh()->SetComponentTickEnabled(false);

    // 🔹 실제 캐릭터 Mesh (에디터에서 교체 가능)
    CharacterMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh"));
    CharacterMesh->SetupAttachment(RootComponent);
    CharacterMesh->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
    CharacterMesh->SetVisibility(true);
    CharacterMesh->SetHiddenInGame(false);
    CharacterMesh->SetMobility(EComponentMobility::Movable);
    CharacterMesh->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
}

void AMushroomCharacter::BeginPlay()
{
    Super::BeginPlay();

    // ✅ HUD 먼저 생성 (항상 보이는 HUD, 입력 차단)
    if (HUDWidgetClass)
    {
        HUDUI = CreateWidget<UWBP_HUD>(GetWorld(), HUDWidgetClass);
        if (HUDUI)
        {
            HUDUI->AddToViewport(0); // Z=0, 항상 밑에
            HUDUI->SetVisibility(ESlateVisibility::HitTestInvisible); // 보이지만 입력은 안 가로챔
            RefreshStatusUI(); // 초기 값 반영
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("HUDWidgetClass is null. BP_MushroomCharacter에서 BP_HUD를 할당하세요."));
    }

    // ✅ 스탯창(StatusUI) 생성 (HUD 위에 표시)
    if (StatusWidgetClass)
    {
        StatusUI = CreateWidget<UWBP_Status>(GetWorld(), StatusWidgetClass);
        if (StatusUI)
        {
            StatusUI->AddToViewport(100); // Z=100, HUD 위에
            StatusUI->SetVisibility(ESlateVisibility::Hidden);
            RefreshStatusUI();

            UE_LOG(LogTemp, Warning, TEXT("[UI] StatusUI created"));

            // ✅ X 버튼 클릭 → ToggleStatusUI 실행
            if (StatusUI->Button_Close)
            {
                UE_LOG(LogTemp, Warning, TEXT("[UI] Button_Close found -> Binding events"));
                StatusUI->Button_Close->OnClicked.AddDynamic(this, &AMushroomCharacter::ToggleStatusUI);
                StatusUI->Button_Close->OnPressed.AddDynamic(this, &AMushroomCharacter::ToggleStatusUI); // 추가
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("[UI] Button_Close is nullptr!"));
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("StatusWidgetClass is null. BP_MushroomCharacter에서 WBP_Status를 할당하세요."));
    }

    // ✅ 입력 모드 초기화
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        PC->bShowMouseCursor = false;
        PC->SetInputMode(FInputModeGameOnly());
    }
}


// ================= Tick =================
void AMushroomCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 🔹 마나 자연 회복 처리
    if (Stats.CurrentMP < Stats.MaxMP)
    {
        Stats.CurrentMP += Stats.ManaRegenRate * DeltaTime;
        Stats.CurrentMP = FMath::Clamp(Stats.CurrentMP, 0.0f, (float)Stats.MaxMP);
    }

    // ✅ HUD/UI는 매 프레임 갱신
    RefreshStatusUI();
}


// ================= Input Binding =================
void AMushroomCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    check(PlayerInputComponent);

    // 이동 입력
    PlayerInputComponent->BindAxis("MoveForward", this, &AMushroomCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveBackward", this, &AMushroomCharacter::MoveBackward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AMushroomCharacter::MoveRight);
    PlayerInputComponent->BindAxis("MoveLeft", this, &AMushroomCharacter::MoveLeft);

    // 카메라 회전 입력
    PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
    PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
    PlayerInputComponent->BindAxis("TurnRate", this, &AMushroomCharacter::TurnAtRate);
    PlayerInputComponent->BindAxis("LookUpRate", this, &AMushroomCharacter::LookUpAtRate);

    PlayerInputComponent->BindAxis("Turn", this, &AMushroomCharacter::AddControllerYawInput);
    PlayerInputComponent->BindAxis("LookUp", this, &AMushroomCharacter::AddControllerPitchInput);

    // 점프
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

    // 전투/액션
    PlayerInputComponent->BindAction("PrimaryAction", IE_Pressed, this, &AMushroomCharacter::OnPrimaryAction);
    PlayerInputComponent->BindAction("PickUp", IE_Pressed, this, &AMushroomCharacter::HandlePickup);
    PlayerInputComponent->BindAction("Inventory", IE_Pressed, this, &AMushroomCharacter::ShowInventory);
    PlayerInputComponent->BindAction("UseFirstItem", IE_Pressed, this, &AMushroomCharacter::UseFirstItem);

    // UI
    PlayerInputComponent->BindAction("StatWindow", IE_Pressed, this, &AMushroomCharacter::ToggleStatusUI);
}

// ================= Movement =================
void AMushroomCharacter::MoveForward(float Value)
{
    if (bStatusUIOpen) return;
    if ((Controller != nullptr) && (Value != 0.0f))
    {
        const FRotator YawRotation(0, Controller->GetControlRotation().Yaw, 0);
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        AddMovementInput(Direction, Value);
    }
}
void AMushroomCharacter::MoveBackward(float Value)
{
    if (bStatusUIOpen) return;
    MoveForward(-Value);
}

void AMushroomCharacter::MoveRight(float Value)
{
    if (bStatusUIOpen) return;
    if ((Controller != nullptr) && (Value != 0.0f))
    {
        const FRotator YawRotation(0, Controller->GetControlRotation().Yaw, 0);
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
        AddMovementInput(Direction, Value);
    }
}
void AMushroomCharacter::MoveLeft(float Value)
{
    if (bStatusUIOpen) return;
    MoveRight(-Value);
}

void AMushroomCharacter::TurnAtRate(float Rate)
{
    if (bStatusUIOpen) return;
    AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}
void AMushroomCharacter::LookUpAtRate(float Rate)
{
    if (bStatusUIOpen) return;
    AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

// ================= Attack =================
void AMushroomCharacter::OnPrimaryAction()
{
    if (bStatusUIOpen) return;

    // ✅ 마나 부족 시 공격 불가
    if (Stats.CurrentMP < 10.0f)
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(
                -1, 1.5f, FColor::Red,
                TEXT("Not enough MP!")
            );
        }
        return;
    }

    APlayerController* PC = Cast<APlayerController>(GetController());
    if (!PC) return;

    int32 SizeX, SizeY;
    PC->GetViewportSize(SizeX, SizeY);
    float CenterX = SizeX * 0.5f;
    float CenterY = SizeY * 0.5f;

    FVector WorldLoc, WorldDir;
    if (PC->DeprojectScreenPositionToWorld(CenterX, CenterY, WorldLoc, WorldDir))
    {
        // 1) 카메라 중앙선 → 목표 지점
        FVector CamStart = WorldLoc;
        FVector CamEnd = CamStart + (WorldDir * 5000.0f);

        FHitResult CamHit;
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(this);

        FVector TargetPoint = CamEnd;
        if (GetWorld()->LineTraceSingleByChannel(CamHit, CamStart, CamEnd, ECC_Visibility, Params))
        {
            TargetPoint = CamHit.Location;
        }

        // 2) 캐릭터 위치(Z=50)에서 TargetPoint로 발사
        FVector Start = GetActorLocation() + FVector(0, 0, 50.0f);
        FVector End = TargetPoint;

        FHitResult HitResult;
        if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params))
        {
            if (AActor* HitActor = HitResult.GetActor())
            {
                UGameplayStatics::ApplyDamage(HitActor, ClickDamage, GetController(), this, nullptr);
                UE_LOG(LogTemp, Log, TEXT("Hit: %s"), *HitActor->GetName());
            }

            DrawDebugLine(GetWorld(), Start, HitResult.Location, FColor::Red, false, 1.0f, 0, 2.0f);
        }
        else
        {
            DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1.0f, 0, 2.0f);
        }

        // ✅ 공격이 실행된 경우에만 마나 차감
        Stats.CurrentMP = FMath::Max(0.0f, Stats.CurrentMP - 10.0f);
    }
}

// ================= Item / Inventory =================
void AMushroomCharacter::SetOverlappingDrop(AMushroomDrop* Drop)
{
    OverlappingDrop = Drop;
}

void AMushroomCharacter::HandlePickup()
{
    if (bStatusUIOpen) return;
    if (OverlappingDrop)
    {
        const FItemData& Data = OverlappingDrop->ItemData;
        if (Data.ItemID == -1)
        {
            UE_LOG(LogTemp, Warning, TEXT("Invalid ItemData from Drop!"));
            return;
        }

        // 인벤토리에 같은 아이템이 있으면 개수만 증가
        bool bFound = false;
        for (FItemData& Item : Inventory)
        {
            if (Item.ItemID == Data.ItemID)
            {
                Item.Quantity += Data.Quantity;
                bFound = true;
                break;
            }
        }

        // 없으면 새로 추가
        if (!bFound)
        {
            Inventory.Add(Data);
        }

        UE_LOG(LogTemp, Log, TEXT("아이템 줍기 성공: %s (x%d)"), *Data.ItemName, Data.Quantity);

        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(
                -1, 2.0f, FColor::Yellow,
                FString::Printf(TEXT("Picked up: %s (x%d)"), *Data.ItemName, Data.Quantity)
            );
        }

        OverlappingDrop->Destroy();
        OverlappingDrop = nullptr;
    }
}

void AMushroomCharacter::ShowInventory()
{
    if (bStatusUIOpen) return;
    if (Inventory.Num() == 0)
    {
        UE_LOG(LogTemp, Log, TEXT("인벤토리가 비어 있습니다."));
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, TEXT("Inventory is empty"));
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("===== 인벤토리 내용 ====="));
    for (const FItemData& Item : Inventory)
    {
        FString ItemLine = FString::Printf(TEXT("%s (x%d)"), *Item.ItemName, Item.Quantity);
        UE_LOG(LogTemp, Log, TEXT("%s"), *ItemLine);

        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, ItemLine);
    }
}

void AMushroomCharacter::UseItem(int32 Index)
{
    if (bStatusUIOpen) return;
    if (Index < 0 || Index >= Inventory.Num()) return;
    FItemData& Item = Inventory[Index];

    // 마나 회복 아이템
    if (Item.Tags.HasTag(FGameplayTag::RequestGameplayTag(FName("Item.Mana"))))
    {
        Stats.CurrentMP = FMath::Clamp(Stats.CurrentMP + Item.ManaValue, 0.0f, (float)Stats.MaxMP);

        UE_LOG(LogTemp, Log, TEXT("%s consumed! MP +%d → %.0f/%d"),
            *Item.ItemName, Item.ManaValue, Stats.CurrentMP, Stats.MaxMP);
    }

    // 공격력 버프 아이템
    if (Item.Tags.HasTag(FGameplayTag::RequestGameplayTag(FName("Item.Buff.Damage"))))
    {
        Stats.AttackPower += Item.AttackValue;
        UE_LOG(LogTemp, Log, TEXT("%s consumed! Attack Power +%d → %d"),
            *Item.ItemName, Item.AttackValue, Stats.AttackPower);
    }

    // 사용 후 소모
    Item.Quantity -= 1;
    if (Item.Quantity <= 0)
    {
        Inventory.RemoveAt(Index);
    }
}

void AMushroomCharacter::UseFirstItem()
{
    if (bStatusUIOpen) return;
    UseItem(0);
}

// ================= Growth =================
void AMushroomCharacter::GainExp(int32 Amount)
{
    if (bStatusUIOpen) return;
    CurrentExp += Amount;

    while (CurrentExp >= MaxExp && Level < 50)
    {
        CurrentExp -= MaxExp;
        Level++;

        // 경험치 곡선 (레벨에 따라 MaxExp 증가)
        MaxExp = 100 + (Level * Level * 20);

        // 레벨업 시 MP 증가
        int32 MPIncrease = 0;
        if (Level <= 20)       MPIncrease = 10 * Level;
        else if (Level <= 40)  MPIncrease = 5 * Level;
        else                   MPIncrease = 2 * Level;

        Stats.MaxMP += MPIncrease;
        Stats.CurrentMP = Stats.MaxMP;

        // 공격/방어 교차 증가
        if (Level % 2 == 1) Stats.AttackPower += 1;
        else                Stats.Defense += 1;

        // 마나 재생 속도 증가
        if (Level % 3 == 0 || Level % 4 == 0)
        {
            Stats.ManaRegenRate += 1.0f;
        }

        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(
                -1, 2.0f, FColor::Green,
                FString::Printf(TEXT("레벨업! Lv.%d | MP %d | ATK %d | DEF %d | Regen %.1f/s"),
                    Level, Stats.MaxMP, Stats.AttackPower, Stats.Defense, Stats.ManaRegenRate)
            );
        }
    }
}

// ================= UI =================
void AMushroomCharacter::RefreshStatusUI()
{
    // ✅ 스탯창 갱신
    if (StatusUI)
    {
        StatusUI->UpdateStats(
            Stats.MaxMP,
            Stats.AttackPower,
            Stats.Defense,
            Stats.ManaRegenRate
        );
    }

    // ✅ HUD 갱신 (MP, EXP, Level, 텍스트)
    if (HUDUI && Stats.MaxMP > 0 && MaxExp > 0)
    {
        const float MPPercent = FMath::Clamp(Stats.CurrentMP / (float)Stats.MaxMP, 0.f, 1.f);
        const float ExpPercent = FMath::Clamp((float)CurrentExp / (float)MaxExp, 0.f, 1.f);

        HUDUI->UpdateBars(MPPercent, ExpPercent);
        HUDUI->UpdateLevel(Level);
        HUDUI->UpdateMPText((int32)Stats.CurrentMP, Stats.MaxMP);
        HUDUI->UpdateExpText(CurrentExp, MaxExp);
    }
}

void AMushroomCharacter::ToggleStatusUI()
{
    if (!StatusUI) return;

    APlayerController* PC = Cast<APlayerController>(GetController());
    if (!PC) return;

    // 🔹 실제 UI 가시성으로 상태 확인
    const bool bIsOpen = (StatusUI->GetVisibility() == ESlateVisibility::Visible);
    UE_LOG(LogTemp, Warning, TEXT("[UI] ToggleStatusUI called. IsOpen=%d"), bIsOpen ? 1 : 0);

    if (bIsOpen)
    {
        // 🔹 스탯창 닫기
        StatusUI->SetVisibility(ESlateVisibility::Collapsed);

        // 🔹 게임 입력 모드 복구
        FInputModeGameOnly InputMode;
        PC->SetInputMode(InputMode);
        PC->bShowMouseCursor = false;

        bStatusUIOpen = false; // 플래그 동기화
        UE_LOG(LogTemp, Warning, TEXT("[UI] Closed"));
    }
    else
    {
        // 🔹 스탯창 열기
        StatusUI->SetVisibility(ESlateVisibility::Visible);

        // 🔹 UI 입력 모드 (X 버튼 클릭 가능)
        FInputModeGameAndUI InputMode;
        // 굳이 필요 없다면 아래 줄은 주석 처리해도 됨 (포커스 독점 방지)
        // InputMode.SetWidgetToFocus(StatusUI->TakeWidget());
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        InputMode.SetHideCursorDuringCapture(false);
        PC->SetInputMode(InputMode);
        PC->bShowMouseCursor = true;

        bStatusUIOpen = true; // 플래그 동기화
        UE_LOG(LogTemp, Warning, TEXT("[UI] Opened"));
    }
}


