#include "World/Act1PreAlphaSliceDirector.h"
#include "BlackClock/BlackClockComponent.h"
#include "BlackClock/PossibilityShiftActor.h"
#include "Camera/EdrathDualCameraComponent.h"
#include "Character/KaelCharacter.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Narrative/EdrathNarrativeSubsystem.h"
#include "Narrative/ForensicClueActor.h"
#include "World/BeliefBuildingActor.h"

AAct1PreAlphaSliceDirector::AAct1PreAlphaSliceDirector()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AAct1PreAlphaSliceDirector::BeginPlay()
{
    Super::BeginPlay();

    if (!KaelCharacter && GetWorld())
    {
        KaelCharacter = Cast<AKaelCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    }

    if (UEdrathNarrativeSubsystem* Narrative = GetNarrativeSubsystem())
    {
        Narrative->OnBeliefWeightChanged.AddDynamic(this, &AAct1PreAlphaSliceDirector::HandleBeliefWeightChanged);
    }

    if (KaelCharacter && KaelCharacter->DualCameraComponent)
    {
        KaelCharacter->DualCameraComponent->OnCameraPerspectiveChanged.AddDynamic(
            this, &AAct1PreAlphaSliceDirector::HandleCameraPerspectiveChanged);
    }

    SpawnZoneOneActors();
    EnterZone(EPreAlphaZoneState::Zone1_BattlefieldAwakening);
}

void AAct1PreAlphaSliceDirector::InitializeSlice(
    AKaelCharacter* InKael,
    APossibilityShiftActor* InGorgeBridge,
    ABeliefBuildingActor* InVillageShrine)
{
    KaelCharacter = InKael;
    GorgeBridge = InGorgeBridge;
    VillageShrine = InVillageShrine;

    if (UEdrathNarrativeSubsystem* Narrative = GetNarrativeSubsystem())
    {
        Narrative->OnBeliefWeightChanged.AddUniqueDynamic(
            this, &AAct1PreAlphaSliceDirector::HandleBeliefWeightChanged);
    }

    if (KaelCharacter && KaelCharacter->DualCameraComponent)
    {
        KaelCharacter->DualCameraComponent->OnCameraPerspectiveChanged.AddUniqueDynamic(
            this, &AAct1PreAlphaSliceDirector::HandleCameraPerspectiveChanged);
    }
}

void AAct1PreAlphaSliceDirector::SpawnZoneOneActors()
{
    if (!GetWorld())
    {
        return;
    }

    if (DyingSoldierDialogueTriggerClass)
    {
        if (AActor* SoldierTrigger = GetWorld()->SpawnActor<AActor>(
            DyingSoldierDialogueTriggerClass, GetActorLocation(), GetActorRotation()))
        {
            SpawnedZoneOneActors.Add(SoldierTrigger);
        }
    }

    if (BattlefieldWraithClass)
    {
        for (int32 WraithIndex = 0; WraithIndex < FMath::Max(0, BattlefieldWraithCount); ++WraithIndex)
        {
            const FVector Offset(static_cast<float>(WraithIndex + 1) * 250.0f, 0.0f, 0.0f);
            if (AActor* Wraith = GetWorld()->SpawnActor<AActor>(
                BattlefieldWraithClass, GetActorLocation() + Offset, GetActorRotation()))
            {
                SpawnedZoneOneActors.Add(Wraith);
            }
        }
    }
}

UEdrathNarrativeSubsystem* AAct1PreAlphaSliceDirector::GetNarrativeSubsystem() const
{
    if (const UWorld* World = GetWorld())
    {
        if (UGameInstance* GameInstance = World->GetGameInstance())
        {
            return GameInstance->GetSubsystem<UEdrathNarrativeSubsystem>();
        }
    }
    return nullptr;
}

UBlackClockComponent* AAct1PreAlphaSliceDirector::GetBlackClockComponent() const
{
    return KaelCharacter ? KaelCharacter->BlackClockComponent : nullptr;
}

void AAct1PreAlphaSliceDirector::EnterZone(EPreAlphaZoneState NewZone)
{
    if (CurrentZone == EPreAlphaZoneState::PreAlphaCompleted ||
        static_cast<uint8>(NewZone) < static_cast<uint8>(CurrentZone))
    {
        return;
    }
    CurrentZone = NewZone;
}

void AAct1PreAlphaSliceDirector::RegisterDyingSoldierDialogueTriggered()
{
    if (CurrentZone == EPreAlphaZoneState::Zone1_BattlefieldAwakening)
    {
        bDyingSoldierDialogueTriggered = true;
        EvaluateZoneProgression();
    }
}

void AAct1PreAlphaSliceDirector::RegisterBattlefieldWraithDefeated()
{
    if (CurrentZone != EPreAlphaZoneState::Zone1_BattlefieldAwakening)
    {
        return;
    }
    DefeatedBattlefieldWraithCount = FMath::Clamp(
        DefeatedBattlefieldWraithCount + 1, 0, FMath::Max(0, BattlefieldWraithCount));
    EvaluateZoneProgression();
}

bool AAct1PreAlphaSliceDirector::RegisterBattlefieldWraithDefeatedUsingEchoCombat(float SecondsAgo)
{
    if (CurrentZone != EPreAlphaZoneState::Zone1_BattlefieldAwakening)
    {
        return false;
    }

    UBlackClockComponent* BlackClock = GetBlackClockComponent();
    if (!BlackClock)
    {
        return false;
    }

    FEchoActionFrame EchoFrame;
    if (!BlackClock->TriggerEchoStrike(SecondsAgo, EEdrathRegion::Veyl, EchoFrame))
    {
        return false;
    }

    RegisterBattlefieldWraithDefeated();
    return true;
}

bool AAct1PreAlphaSliceDirector::AdvanceSettlementDay()
{
    if (CurrentZone != EPreAlphaZoneState::Zone3_LoopingSettlementExterior)
    {
        return false;
    }

    UEdrathNarrativeSubsystem* Narrative = GetNarrativeSubsystem();
    if (!Narrative)
    {
        return false;
    }

    Narrative->AdvanceSettlementDay();
    ++AdvancedSettlementDayCount;
    EvaluateZoneProgression();
    return true;
}

void AAct1PreAlphaSliceDirector::RegisterHouseForensicClue(AForensicClueActor* ForensicClue)
{
    if (!ForensicClue || RegisteredHouseClues.Contains(ForensicClue))
    {
        return;
    }

    RegisteredHouseClues.Add(ForensicClue);
    ForensicClue->OnForensicClueInspected.AddDynamic(
        this, &AAct1PreAlphaSliceDirector::HandleForensicClueInspected);
    ForensicClue->BindToBlackClock(GetBlackClockComponent());
}

void AAct1PreAlphaSliceDirector::RegisterCryptPortcullisShift()
{
    if (CurrentZone == EPreAlphaZoneState::Zone5_CryptPriestTwist)
    {
        bCryptPortcullisShifted = true;
    }
}

void AAct1PreAlphaSliceDirector::RegisterPriestCorpseInspected()
{
    if (CurrentZone == EPreAlphaZoneState::Zone5_CryptPriestTwist)
    {
        bPriestCorpseInspected = true;
    }
}

bool AAct1PreAlphaSliceDirector::CompletePriestMemoryReconstruction()
{
    if (CurrentZone != EPreAlphaZoneState::Zone5_CryptPriestTwist ||
        !bCryptPortcullisShifted || !bPriestCorpseInspected || bPriestMemoryReconstructed)
    {
        return false;
    }

    UBlackClockComponent* BlackClock = GetBlackClockComponent();
    UEdrathNarrativeSubsystem* Narrative = GetNarrativeSubsystem();
    if (!BlackClock || !Narrative)
    {
        return false;
    }

    if (!BlackClock->EnterMemoryReconstruction(PriestMemoryAnchorTag, PriestMemoryOwner))
    {
        return false;
    }

    Narrative->BreakAct1SettlementLoop();
    bPriestMemoryReconstructed = true;
    CurrentZone = EPreAlphaZoneState::PreAlphaCompleted;
    OnAct1PreAlphaCompleted.Broadcast();
    return true;
}

void AAct1PreAlphaSliceDirector::HandleBeliefWeightChanged(FGameplayTag BuildingTag, float NewBeliefWeight)
{
    if (CurrentZone == EPreAlphaZoneState::Zone3_LoopingSettlementExterior &&
        VillageShrine && BuildingTag == VillageShrine->BuildingBeliefTag &&
        NewBeliefWeight >= VillageShrine->ExistenceBeliefThreshold)
    {
        bShrineObserved = true;
        EvaluateZoneProgression();
    }
}

void AAct1PreAlphaSliceDirector::HandleForensicClueInspected(FGameplayTag EvidenceTag, AActor*)
{
    if (CurrentZone != EPreAlphaZoneState::Zone4_KaelHouseForensics)
    {
        return;
    }

    if (EvidenceTag == HandwritingEvidenceTag)
    {
        bHandwritingClueInspected = true;
    }
    if (EvidenceTag == ScratchedPortraitEvidenceTag)
    {
        bScratchedPortraitInspected = true;
    }
    EvaluateZoneProgression();
}

void AAct1PreAlphaSliceDirector::HandleCameraPerspectiveChanged(EEdrathCameraMode NewMode, AActor*)
{
    if (NewMode == EEdrathCameraMode::InteriorThirdPerson)
    {
        EvaluateZoneProgression();
    }
}

void AAct1PreAlphaSliceDirector::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    RefreshZoneState();
}

void AAct1PreAlphaSliceDirector::RefreshZoneState()
{
    if (CurrentZone == EPreAlphaZoneState::Zone2_CollapsedGorgeBridge && GorgeBridge &&
        GorgeBridge->GetCurrentActorTimelineState() == ETimelineState::RejectedTimeline)
    {
        EnterZone(EPreAlphaZoneState::Zone3_LoopingSettlementExterior);
    }
    if (CurrentZone == EPreAlphaZoneState::Zone5_CryptPriestTwist && CryptPortcullis &&
        CryptPortcullis->GetCurrentActorTimelineState() == ETimelineState::RejectedTimeline)
    {
        bCryptPortcullisShifted = true;
    }
    EvaluateZoneProgression();
}

void AAct1PreAlphaSliceDirector::EvaluateZoneProgression()
{
    if (CurrentZone == EPreAlphaZoneState::Zone1_BattlefieldAwakening &&
        bDyingSoldierDialogueTriggered &&
        DefeatedBattlefieldWraithCount >= FMath::Max(0, BattlefieldWraithCount))
    {
        EnterZone(EPreAlphaZoneState::Zone2_CollapsedGorgeBridge);
    }
    else if (CurrentZone == EPreAlphaZoneState::Zone3_LoopingSettlementExterior &&
        bShrineObserved && AdvancedSettlementDayCount >= 7 &&
        GetNarrativeSubsystem() && GetNarrativeSubsystem()->GetCurrentSettlementLoopDay() == 1)
    {
        EnterZone(EPreAlphaZoneState::Zone4_KaelHouseForensics);
    }
    else if (CurrentZone == EPreAlphaZoneState::Zone4_KaelHouseForensics &&
        KaelCharacter && KaelCharacter->DualCameraComponent &&
        KaelCharacter->DualCameraComponent->GetCurrentCameraMode() == EEdrathCameraMode::InteriorThirdPerson &&
        bHandwritingClueInspected && bScratchedPortraitInspected)
    {
        EnterZone(EPreAlphaZoneState::Zone5_CryptPriestTwist);
    }
}
