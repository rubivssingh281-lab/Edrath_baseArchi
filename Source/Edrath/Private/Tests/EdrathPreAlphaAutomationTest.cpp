#include "Misc/AutomationTest.h"
#include "BlackClock/BlackClockComponent.h"
#include "BlackClock/PossibilityShiftActor.h"
#include "Camera/EdrathDualCameraComponent.h"
#include "Character/KaelCharacter.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Narrative/EdrathNarrativeSubsystem.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FEdrathPreAlphaVerticalSliceTest,
    "Edrath.PreAlpha.CoreSystemsAndDualCameraVerification",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEdrathPreAlphaVerticalSliceTest::RunTest(const FString&)
{
    UWorld* TestWorld = UWorld::CreateWorld(EWorldType::Game, false);
    if (!TestWorld)
    {
        AddError(TEXT("Test world must be created"));
        return false;
    }

    UGameInstance* GameInstance = NewObject<UGameInstance>(TestWorld);
    TestWorld->SetGameInstance(GameInstance);
    GameInstance->Init();

    AKaelCharacter* Kael = TestWorld->SpawnActor<AKaelCharacter>();
    TestNotNull(TEXT("KaelCharacter must spawn cleanly"), Kael);
    if (!Kael)
    {
        TestWorld->DestroyWorld(false);
        return false;
    }

    TestEqual(TEXT("Default Camera Mode must be ExteriorBirdsEye"),
        Kael->DualCameraComponent->GetCurrentCameraMode(), EEdrathCameraMode::ExteriorBirdsEye);
    Kael->DualCameraComponent->SetCameraPerspective(EEdrathCameraMode::InteriorThirdPerson);
    TestEqual(TEXT("Interior transition must switch camera mode"),
        Kael->DualCameraComponent->GetCurrentCameraMode(), EEdrathCameraMode::InteriorThirdPerson);

    APossibilityShiftActor* Bridge = TestWorld->SpawnActor<APossibilityShiftActor>();
    TestNotNull(TEXT("Possibility shift bridge must spawn"), Bridge);
    if (Bridge)
    {
        Bridge->SetActorLocation(Kael->GetActorLocation() + FVector(200.0f, 0.0f, 0.0f));
        TestTrue(TEXT("Black Clock possibility shift must succeed"), Kael->ActivateBlackClockShift());
        TestEqual(TEXT("Bridge must shift to rejected timeline"),
            Bridge->GetCurrentActorTimelineState(), ETimelineState::RejectedTimeline);
    }

    Kael->PerformPrimaryAttack();
    FEchoActionFrame EchoFrame;
    TestTrue(TEXT("Echo Combat strike must manifest"),
        Kael->BlackClockComponent->TriggerEchoStrike(1.2f, EEdrathRegion::Veyl, EchoFrame));
    TestEqual(TEXT("Recorded combat frame must preserve montage section"),
        EchoFrame.ActionMontageSection, FName(TEXT("Slash_Combo_01")));

    UEdrathNarrativeSubsystem* Narrative = GameInstance->GetSubsystem<UEdrathNarrativeSubsystem>();
    TestNotNull(TEXT("Narrative subsystem must initialize"), Narrative);
    if (Narrative)
    {
        for (int32 Day = 0; Day < 7; ++Day)
        {
            Narrative->AdvanceSettlementDay();
        }
        TestEqual(TEXT("Settlement loop must reset to day one"), Narrative->GetCurrentSettlementLoopDay(), 1);
        Narrative->BreakAct1SettlementLoop();
        TestTrue(TEXT("Crypt twist must break settlement loop"), Narrative->IsSettlementLoopBroken());
    }

    TestWorld->DestroyWorld(false);
    return !HasAnyErrors();
}
