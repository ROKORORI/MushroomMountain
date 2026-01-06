# Mushroom Mountain (UE5, C++ Only)

언리얼5 C++ 기반으로 만든 작은 오픈월드/서바이벌 느낌의 프로젝트입니다.  
버섯(타겟)을 공격하면 아이템이 드랍되고, 플레이어는 아이템을 줍고 사용하면서 MP/스탯/레벨을 성장시킵니다.  
또한 게임 내 시간이 흐르며(낮/밤) HUD에 시간이 표시됩니다.

---

## Implemented Features (구현된 기능)

### 1) Player Core (이동/카메라)
- 3인칭 카메라(SpringArm + Camera) 세팅
- 기본 이동/회전 입력 처리
- 캐릭터 메시 컴포넌트 분리 세팅(에디터에서 교체 용이)

---

### 2) Combat System (전투)
- **화면 중앙 기반 타겟팅 공격**
  - 카메라 화면 중앙을 기준으로 레이캐스트(LineTrace) → 타겟 명중
  - 명중 시 `ApplyDamage()` 호출로 데미지 적용
- **MP 소모형 공격**
  - 공격 1회당 MP 10 소모
  - MP 부족 시 공격 불가(디버그 메시지 출력)

---

### 3) Target Actor (버섯 타겟 / HP / 리스폰)
- 타겟 액터는 HP를 보유하며 `TakeDamage()`로 체력 감소
- HP가 0 이하가 되면:
  - 플레이어에게 EXP 지급
  - 아이템 드랍 스폰
  - 일정 시간 후 같은 위치에 리스폰(타이머 기반)

---

### 4) Drop & Pickup (드랍/줍기)
- 드랍 액터는 오버랩으로 “줍기 가능 상태”를 제공
- 플레이어가 드랍과 가까워지면:
  - **TextRender 안내 문구 표시**
  - 안내 텍스트는 카메라를 바라보도록 회전(빌보드 느낌)
- 플레이어가 줍기 입력(F) 시:
  - 인벤토리에 아이템이 추가되고, 드랍 액터는 제거

---

### 5) Inventory (인벤토리)
- 인벤토리는 `FItemData` 기반으로 관리
- 같은 ItemID가 이미 있으면 Quantity 누적
- 인벤토리 출력(디버그 로그) 기능 포함

---

### 6) Item System (아이템 데이터 / DataTable / GameplayTags)
- 아이템은 DataTable Row 구조체(`FItemData`)로 정의
- `GameplayTags`로 아이템 효과를 분기 처리
- 아이템은 드랍 시 ItemID 기반으로 DataTable에서 로드되어 초기화됨

---

### 7) Item Use (아이템 사용)
- 첫 번째 아이템 사용 기능(단축 동작)
- 구현된 예시 효과:
  - `Item.Mana` 태그 → MP 회복
  - `Item.Buff.Damage` 태그 → 공격력 증가(버프 형태)
- 사용 시 Quantity 감소, 0이 되면 인벤토리에서 제거

---

### 8) EXP & Leveling (경험치 / 레벨업 / 스탯 성장)
- EXP 획득 시 레벨업 루프 처리(최대 레벨 제한 포함)
- 레벨업 시:
  - MaxMP 증가 + CurrentMP 회복
  - 홀수/짝수 레벨 조건에 따라 Attack/Defense 증가
  - 특정 레벨 조건에서 ManaRegenRate 증가
- 경험치 요구량은 레벨에 따라 증가(레벨 기반 수식 적용)

---

### 9) UI (HUD + Status Window)
- **HUD UI**
  - MP/EXP 바(퍼센트), 레벨, 수치 텍스트 업데이트 인터페이스 제공
  - 게임 시간 표시(시계 UI) 업데이트 인터페이스 제공
- **Status UI(스탯창)**
  - MaxMP / Attack / Defense / RegenRate 업데이트 인터페이스 제공
  - 닫기 버튼(Button_Close) 이벤트로 스탯창 닫기 지원
- **스탯창 토글**
  - 스탯창 열림/닫힘에 따라 입력 모드(GameOnly ↔ GameAndUI), 커서 표시/숨김 전환

---

### 10) Day/Night Time Flow (게임 시간 / 낮밤)
- GameMode에서 `CurrentTimeOfDay`가 Tick마다 증가하며 24시간 루프
- 낮/밤 판정(예: 6~18시 낮)
- HUD에 현재 시간/낮밤 상태를 업데이트하는 흐름 구현

---

## Controls (기본 조작)
- 이동: WASD
- 시점 회전: 마우스(또는 Turn/LookUp 축)
- 공격: PrimaryAction (프로젝트 입력 설정 기준)
- 줍기: F
- 스탯창 토글: StatWindow (프로젝트 입력 설정 기준)
- 첫 아이템 사용: UseFirstItem (프로젝트 입력 설정 기준)

> 키 매핑 이름은 프로젝트 Input 설정에 따라 다를 수 있습니다.

---

## Tech Notes (기술 메모)
- C++ 중심 구조 + UMG 위젯은 BP에서 구현하고, C++에서 업데이트 함수 호출 방식으로 연결
- GameplayTags를 활용해 아이템 효과를 데이터 기반으로 분기
- 모듈 의존성: UMG / Slate / SlateCore / GameplayTags 포함
