#include "MyPlayerController.h"

void AMyPlayerController::BeginPlay()
{
    Super::BeginPlay();

    // 🖱 실행 시 마우스 커서 보이게
    bShowMouseCursor = true;

    // 게임 + UI 입력 둘 다 가능
    FInputModeGameAndUI InputMode;
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    SetInputMode(InputMode);
}
