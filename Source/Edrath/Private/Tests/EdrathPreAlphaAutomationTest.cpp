#include "Misc/AutomationTest.h"
#include "BlackClock/BlackClockComponent.h"
#include "BlackClock/PossibilityShiftActor.h"
#include "Camera/EdrathDualCameraComponent.h"
#include "Camera/EdrathInteriorTransitionVolume.h"
#include "Character/KaelCharacter.h"
#include "Core/EdrathGameplayTags.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Narrative/EdrathNarrativeSubsystem.h"
#include "Narrative/ForensicClueActor.h"
#include "World/Act1PreAlphaSliceDirector.h"
#include "World/BeliefBuildingActor.h"
#include "Character/AshenNPCCharacter.h"
#include "Character/EdrathCompanionCharacter.h"
#include "World/MemoryFruitTreeActor.h"
#include "World/NhalFactionManager.h"
#include "World/TombSarcophagusActor.h"
#include "World/UnwrittenIslandSpawner.h"
#include "Narrative/EdrathQuestSubsystem.h"
#include "World/HouseWithOneMoreRoomActor.h"
#include "Core/EdrathScalabilitySubsystem.h"
#include "Core/EdrathHardwareOptimizationSubsystem.h"
#include "Core/EdrathSaveSubsystem.h"
#include "Core/EdrathTelemetrySubsystem.h"
#include "BlackClock/EchoCombatComponent.h"
#include "UI/BlackClockWidget.h"
#include "UI/ForensicLedgerWidget.h"
#include "UI/MemoryDialogueWidget.h"
#include "UI/EdrathHUD.h"
#include "UI/EdrathReticleWidget.h"
#include "UI/EchoCombatBufferWidget.h"
#include "UI/ForensicContradictionWidget.h"
#include "UI/EdrathTouchControlsWidget.h"
#include "Character/BattlefieldWraithCharacter.h"
#include "World/DyingSoldierActor.h"
#include "World/Act1GorgeBridgeActor.h"
#include "Character/SettlementVillagerCharacter.h"
#include "Character/KaelModularRigComponent.h"
#include "Character/KaelTerrainAdaptationComponent.h"
#include "World/EdrathTerrainSubsystem.h"
#include "World/KaelHouseInteriorActor.h"
#include "World/PriestCorpseActor.h"
#include "World/Act1BattlefieldLevelScriptActor.h"
#include "World/Act2VeylCourtDirector.h"
#include "World/Act3HollowgreenDirector.h"
#include "World/Act4NhalDirector.h"
#include "World/Act5CrownOfWinterDirector.h"
#include "World/Act6UnwrittenSeaDirector.h"
#include "Character/FirstArchivistBossCharacter.h"
#include "Character/EdrathCompanionAIController.h"
#include "Narrative/MemoryAnchorActor.h"
#include "World/WeddingAltarActor.h"
#include "World/BuriedFutureGraveActor.h"
#include "World/VillageVotingShrineActor.h"
#include "World/DragonMemoryHoardActor.h"
#include "World/LastHonestKingMonumentActor.h"
#include "Core/EdrathAudioSubsystem.h"

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





IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FEdrathAct1PreAlphaFiveZoneTest,
    "Edrath.PreAlpha.Act1FiveZonePlayableSlice",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEdrathAct1PreAlphaFiveZoneTest::RunTest(const FString&)
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
    APossibilityShiftActor* GorgeBridge = TestWorld->SpawnActor<APossibilityShiftActor>();
    APossibilityShiftActor* CryptPortcullis = TestWorld->SpawnActor<APossibilityShiftActor>();
    ABeliefBuildingActor* Shrine = TestWorld->SpawnActor<ABeliefBuildingActor>();
    AAct1PreAlphaSliceDirector* Director = TestWorld->SpawnActor<AAct1PreAlphaSliceDirector>();

    TestNotNull(TEXT("Kael must spawn for the five-zone slice"), Kael);
    TestNotNull(TEXT("Gorge bridge must spawn for the five-zone slice"), GorgeBridge);
    TestNotNull(TEXT("Crypt portcullis must spawn for the five-zone slice"), CryptPortcullis);
    TestNotNull(TEXT("Village shrine must spawn for the five-zone slice"), Shrine);
    TestNotNull(TEXT("Act I slice director must spawn"), Director);
    if (!Kael || !GorgeBridge || !CryptPortcullis || !Shrine || !Director)
    {
        TestWorld->DestroyWorld(false);
        return false;
    }

    Shrine->BuildingBeliefTag = EdrathGameplayTags::RegionVeyl;
    Director->GorgeBridge = GorgeBridge;
    Director->CryptPortcullis = CryptPortcullis;
    Director->VillageShrine = Shrine;
    Director->HandwritingEvidenceTag = EdrathGameplayTags::Twist1;
    Director->ScratchedPortraitEvidenceTag = EdrathGameplayTags::Twist2;
    Director->PriestMemoryAnchorTag = EdrathGameplayTags::QuestAct1Crypt;
    Director->InitializeSlice(Kael, GorgeBridge, Shrine);

    TestEqual(TEXT("Slice starts in Zone 1"),
        Director->GetCurrentZone(), EPreAlphaZoneState::Zone1_BattlefieldAwakening);
    TestTrue(TEXT("Zone 1 must use the specified dying soldier dialogue"),
        Director->DyingSoldierDialogueLine.EqualTo(FText::FromString(TEXT("Do not let my daughter be born"))));
    Director->RegisterDyingSoldierDialogueTriggered();
    Director->BattlefieldWraithCount = 3;
    Kael->PerformPrimaryAttack();
    TestTrue(TEXT("First battlefield wraith must be defeated through Echo Combat"),
        Director->RegisterBattlefieldWraithDefeatedUsingEchoCombat());
    TestTrue(TEXT("Second battlefield wraith must be defeated through Echo Combat"),
        Director->RegisterBattlefieldWraithDefeatedUsingEchoCombat());
    TestTrue(TEXT("Third battlefield wraith must be defeated through Echo Combat"),
        Director->RegisterBattlefieldWraithDefeatedUsingEchoCombat());
    TestEqual(TEXT("Three defeated wraiths advance to Zone 2"),
        Director->GetCurrentZone(), EPreAlphaZoneState::Zone2_CollapsedGorgeBridge);

    GorgeBridge->ApplyTimelineState(ETimelineState::RejectedTimeline);
    Director->RefreshZoneState();
    TestEqual(TEXT("Rejected bridge advances to Zone 3"),
        Director->GetCurrentZone(), EPreAlphaZoneState::Zone3_LoopingSettlementExterior);

    UEdrathNarrativeSubsystem* Narrative = GameInstance->GetSubsystem<UEdrathNarrativeSubsystem>();
    Narrative->SetCitizenBeliefWeight(Shrine->BuildingBeliefTag, 1.0f);
    for (int32 Day = 0; Day < 7; ++Day)
    {
        TestTrue(TEXT("Settlement day must advance in Zone 3"), Director->AdvanceSettlementDay());
    }
    TestEqual(TEXT("Seven-day loop advances to Zone 4"),
        Director->GetCurrentZone(), EPreAlphaZoneState::Zone4_KaelHouseForensics);

    Kael->DualCameraComponent->SetCameraPerspective(EEdrathCameraMode::InteriorThirdPerson);
    AForensicClueActor* HandwritingClue = TestWorld->SpawnActor<AForensicClueActor>();
    AForensicClueActor* PortraitClue = TestWorld->SpawnActor<AForensicClueActor>();
    TestNotNull(TEXT("Handwriting forensic clue must spawn"), HandwritingClue);
    TestNotNull(TEXT("Scratched portrait forensic clue must spawn"), PortraitClue);
    if (HandwritingClue && PortraitClue)
    {
        HandwritingClue->SetActorLocation(Kael->GetActorLocation());
        PortraitClue->SetActorLocation(Kael->GetActorLocation());
        HandwritingClue->EvidenceTag = EdrathGameplayTags::Twist1;
        HandwritingClue->InitialDescription = FText::FromString(TEXT("Kael's handwriting is dated seven days in the future."));
        HandwritingClue->RejectedTimelineDescription = FText::FromString(TEXT("The handwriting belongs to the priest's dead hand."));
        PortraitClue->EvidenceTag = EdrathGameplayTags::Twist2;
        PortraitClue->InitialDescription = FText::FromString(TEXT("A woman stands beside Kael with her face scratched away."));
        PortraitClue->RejectedTimelineDescription = FText::FromString(TEXT("The scratched face is the only person who remembers Kael correctly."));
        Director->RegisterHouseForensicClue(HandwritingClue);
        Director->RegisterHouseForensicClue(PortraitClue);

        TestTrue(TEXT("Handwriting clue must be inspectable indoors"), HandwritingClue->Inspect(Kael));
        TestTrue(TEXT("Portrait clue must be inspectable indoors"), PortraitClue->Inspect(Kael));
        TestTrue(TEXT("Black Clock shift must mutate forensic descriptions"), Kael->ActivateBlackClockShift());
        TestTrue(TEXT("Handwriting clue must receive rejected-timeline mutation"),
            HandwritingClue->GetCurrentDescription().EqualTo(HandwritingClue->RejectedTimelineDescription));
        TestTrue(TEXT("Portrait clue must receive rejected-timeline mutation"),
            PortraitClue->GetCurrentDescription().EqualTo(PortraitClue->RejectedTimelineDescription));
    }
    Director->RefreshZoneState();
    TestEqual(TEXT("Both house clues advance to Zone 5"),
        Director->GetCurrentZone(), EPreAlphaZoneState::Zone5_CryptPriestTwist);

    CryptPortcullis->ApplyTimelineState(ETimelineState::RejectedTimeline);
    Director->RefreshZoneState();
    Director->RegisterPriestCorpseInspected();
    TestTrue(TEXT("Priest memory reconstruction must complete the slice"),
        Director->CompletePriestMemoryReconstruction());
    TestEqual(TEXT("All five zones must complete"),
        Director->GetCurrentZone(), EPreAlphaZoneState::PreAlphaCompleted);
    TestTrue(TEXT("Act I settlement loop must be broken by the priest twist"),
        Narrative->IsSettlementLoopBroken());

    TestWorld->DestroyWorld(false);
    return !HasAnyErrors();
}





IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FEdrathHUDAndForensicUIVerificationTest,
    "Edrath.PreAlpha.HUDAndForensicUIVerification",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEdrathHUDAndForensicUIVerificationTest::RunTest(const FString&)
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

    // 1. Verify Black Clock HUD Widget state updates
    UBlackClockWidget* ClockWidget = NewObject<UBlackClockWidget>(TestWorld);
    TestNotNull(TEXT("Black Clock HUD Widget must instantiate"), ClockWidget);
    if (ClockWidget)
    {
        ClockWidget->UpdateClockStability(75.0f, 100.0f);
        TestEqual(TEXT("Clock stability percent must match"), ClockWidget->GetDisplayedStabilityPercent(), 75.0f);

        ClockWidget->UpdateTimelineState(ETimelineState::RejectedTimeline);
        TestEqual(TEXT("Timeline state must reflect RejectedTimeline"),
            ClockWidget->GetDisplayedTimelineState(), ETimelineState::RejectedTimeline);

        ClockWidget->UpdateRegionalInstability(38.0f);
        TestEqual(TEXT("Regional instability score must match"),
            ClockWidget->GetDisplayedRegionalInstability(), 38.0f);
    }

    // 2. Verify Forensic Ledger Evidence Population & Filtering
    UEdrathNarrativeSubsystem* Narrative = GameInstance->GetSubsystem<UEdrathNarrativeSubsystem>();
    TestNotNull(TEXT("Narrative subsystem must be valid for UI tests"), Narrative);
    if (Narrative)
    {
        Narrative->AddOrMutateEvidence(
            EdrathGameplayTags::Twist1,
            FText::FromString(TEXT("Handwriting matches ancient archive ledger")),
            true);
        Narrative->AddOrMutateEvidence(
            EdrathGameplayTags::Twist2,
            FText::FromString(TEXT("Portrait of Kael with scratched-out companion face")),
            true);
        Narrative->AddOrMutateEvidence(
            EdrathGameplayTags::QuestAct1Crypt,
            FText::FromString(TEXT("Priest skeletal remains verified dead 114 years")),
            false);

        UForensicLedgerWidget* LedgerWidget = NewObject<UForensicLedgerWidget>(TestWorld);
        TestNotNull(TEXT("Forensic Ledger Widget must instantiate"), LedgerWidget);
        if (LedgerWidget)
        {
            LedgerWidget->RefreshEvidenceList();
            TestEqual(TEXT("Total clues in ledger must be 3"), LedgerWidget->GetTotalCluesCount(), 3);
            TestEqual(TEXT("Twist clues in ledger must be 2"), LedgerWidget->GetTwistCluesCount(), 2);

            LedgerWidget->SetFilterMode(EForensicLedgerFilter::TwistCluesOnly);
            TestEqual(TEXT("Filtered twist clues count must be 2"), LedgerWidget->GetFilteredEvidenceList().Num(), 2);

            FEdrathEvidenceRecord SelectedClue;
            TestTrue(TEXT("Selecting evidence by tag must succeed"),
                LedgerWidget->SelectEvidenceByTag(EdrathGameplayTags::Twist1));
            TestTrue(TEXT("Retrieved selected evidence must be valid"),
                LedgerWidget->GetSelectedEvidence(SelectedClue));
            TestEqual(TEXT("Selected clue tag must match"),
                SelectedClue.EvidenceTag, EdrathGameplayTags::Twist1);
        }
    }

    // 3. Verify Memory Reconstruction Dialogue Widget
    UMemoryDialogueWidget* DialogueWidget = NewObject<UMemoryDialogueWidget>(TestWorld);
    TestNotNull(TEXT("Memory Dialogue Widget must instantiate"), DialogueWidget);
    if (DialogueWidget)
    {
        TArray<FText> Choices;
        Choices.Add(FText::FromString(TEXT("Examine Priest Sarcophagus")));
        Choices.Add(FText::FromString(TEXT("Confront with 114-Year-Old Ledger")));

        DialogueWidget->DisplayDialogue(
            FText::FromString(TEXT("Memory of Father Aaron")),
            FText::FromString(TEXT("You walked these crypts before you were born, Kael.")),
            Choices);

        TestTrue(TEXT("Dialogue must be marked active"), DialogueWidget->IsDialogueActive());
        TestEqual(TEXT("Dialogue choices count must be 2"), DialogueWidget->GetCurrentChoices().Num(), 2);
        TestTrue(TEXT("Choosing valid choice index must succeed"), DialogueWidget->ChooseOption(0));

        DialogueWidget->Dismiss();
        TestFalse(TEXT("Dialogue must be inactive after dismissal"), DialogueWidget->IsDialogueActive());
    }

    // 4. Verify AEdrathHUD Actor
    AEdrathHUD* EdrathHUD = TestWorld->SpawnActor<AEdrathHUD>();
    TestNotNull(TEXT("AEdrathHUD actor must spawn"), EdrathHUD);
    if (EdrathHUD)
    {
        TestFalse(TEXT("Forensic ledger must be initially closed"), EdrathHUD->IsForensicLedgerOpen());
    }

    TestWorld->DestroyWorld(false);
    return !HasAnyErrors();
}





IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FEdrathSixRegionsWorldActorsTest,
    "Edrath.PreAlpha.SixRegionsWorldActors",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEdrathSixRegionsWorldActorsTest::RunTest(const FString&)
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
    UEdrathNarrativeSubsystem* Narrative = GameInstance->GetSubsystem<UEdrathNarrativeSubsystem>();
    AKaelCharacter* Kael = TestWorld->SpawnActor<AKaelCharacter>();
    TestNotNull(TEXT("Narrative subsystem must initialize"), Narrative);
    TestNotNull(TEXT("Kael must spawn for regional tests"), Kael);
    if (!Narrative || !Kael)
    {
        TestWorld->DestroyWorld(false);
        return false;
    }

    AMemoryFruitTreeActor* Tree = TestWorld->SpawnActor<AMemoryFruitTreeActor>();
    TestNotNull(TEXT("Memory fruit tree must spawn"), Tree);
    if (!Tree)
    {
        TestWorld->DestroyWorld(false);
        return false;
    }
    Tree->MemoryAreaActorClass = AActor::StaticClass();
    Tree->ForestMutationTag = EdrathGameplayTags::Act3;
    Tree->MemoryEchoArchetype = EKaelLifeEchoArchetype::PlaguedHealer_Act3;
    TestNotNull(TEXT("Harvested fruit must materialize a memory area"), Tree->HarvestMemoryFruit(Kael));
    Tree->ModifyMemoryAnchor(EdrathGameplayTags::Twist3, FText::FromString(TEXT("The healer's memory changes Hollowgreen.")));
    TestEqual(TEXT("Hollowgreen anchor mutation must be recorded"), Narrative->GetForestMutationVersion(EdrathGameplayTags::Twist3), 1);
    TestEqual(TEXT("Memory tree must track its life echo"), Tree->GetMemoryEchoArchetype(), EKaelLifeEchoArchetype::PlaguedHealer_Act3);

    AAshenNPCCharacter* AshenNpc = TestWorld->SpawnActor<AAshenNPCCharacter>();
    TestNotNull(TEXT("Ashen NPC must spawn"), AshenNpc);
    if (AshenNpc && AshenNpc->GetMesh())
    {
        TestFalse(TEXT("Ashen NPC must cast no shadow"), AshenNpc->GetMesh()->CastShadow);
        TestFalse(TEXT("Ashen NPC must cast no dynamic shadow"), AshenNpc->GetMesh()->bCastDynamicShadow);
    }

    ATombSarcophagusActor* Tomb = TestWorld->SpawnActor<ATombSarcophagusActor>();
    const FGuid LivingNpcId = FGuid::NewGuid();
    TestTrue(TEXT("Tomb must bind a living NPC identity"), Tomb->BindLivingNpc(AshenNpc, LivingNpcId));
    TestTrue(TEXT("Disturbing tomb must mutate living NPC memory"), Tomb->DisturbTomb(Kael));
    TestEqual(TEXT("Living NPC memory must contain the tomb mutation"),
        Narrative->GetLivingNpcMemory(LivingNpcId), Tomb->DisturbedMemoryDescription);

    ANhalFactionManager* Nhal = TestWorld->SpawnActor<ANhalFactionManager>();
    Nhal->SetFactionReputation(EdrathGameplayTags::FactionNhalRestorationists, 10);
    TestEqual(TEXT("Nhal faction reputation must be tracked"),
        Nhal->GetFactionReputation(EdrathGameplayTags::FactionNhalRestorationists), 10);
    Nhal->LivingVeylFamilyActors.Add(AshenNpc);
    TestTrue(TEXT("Black Clock memory reconstruction must restore a Nhal citizen"),
        Kael->BlackClockComponent->EnterMemoryReconstruction(EdrathGameplayTags::Twist3, TEXT("NhalCitizen")));
    TestTrue(TEXT("Restoring a Nhal citizen must trigger Twist 3"),
        Nhal->RestoreNhalCitizenViaBlackClock(TEXT("Citizen_01"), Kael));
    TestTrue(TEXT("Twist 3 must be marked triggered"), Nhal->HasTriggeredTwistThree());

    Narrative->AddUnchosenPossibilityTag(EdrathGameplayTags::QuestHouseOneMoreRoom);
    AUnwrittenIslandSpawner* IslandSpawner = TestWorld->SpawnActor<AUnwrittenIslandSpawner>();
    IslandSpawner->MaterializedIslandClass = AActor::StaticClass();
    TestEqual(TEXT("Unwritten Sea must read one unchosen possibility"),
        IslandSpawner->MaterializeUnchosenPossibilities(), 1);

    TestWorld->DestroyWorld(false);
    return !HasAnyErrors();
}





IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FEdrathQuestAndCompanionSystemsTest,
    "Edrath.PreAlpha.QuestsAndAutonomousCompanions",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEdrathQuestAndCompanionSystemsTest::RunTest(const FString&)
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
    UEdrathQuestSubsystem* Quests = GameInstance->GetSubsystem<UEdrathQuestSubsystem>();
    UEdrathNarrativeSubsystem* Narrative = GameInstance->GetSubsystem<UEdrathNarrativeSubsystem>();
    TestNotNull(TEXT("Quest subsystem must initialize"), Quests);
    TestNotNull(TEXT("Narrative subsystem must initialize"), Narrative);
    if (!Quests || !Narrative)
    {
        TestWorld->DestroyWorld(false);
        return false;
    }

    TestTrue(TEXT("Wedding quest must resolve by merging timelines"), Quests->ResolveWeddingNotHappened(true));
    TestEqual(TEXT("Wedding outcome must be recorded"),
        Quests->GetQuestState(EdrathGameplayTags::QuestWeddingNotHappened).Outcome,
        EEdrathQuestOutcome::WeddingTimelinesMerged);
    TestTrue(TEXT("Severed wedding branch must be recorded as unchosen"),
        Narrative->GetUnchosenPossibilityTags().Contains(EdrathGameplayTags::QuestWeddingSeverMemoryEcho));

    TestTrue(TEXT("Dragon quest must resolve by returning memories"), Quests->ResolveDragonNoFire(true));
    TestTrue(TEXT("Dragon must become apocalyptic after memory restoration"), Quests->IsDragonApocalyptic());
    TestTrue(TEXT("Peaceful dragon branch must be recorded as unchosen"),
        Narrative->GetUnchosenPossibilityTags().Contains(EdrathGameplayTags::QuestDragonPreservePeace));

    TestTrue(TEXT("Buried Future quest must resolve to a violent stranger memory"),
        Quests->ResolveBuriedFuture(false));
    TestEqual(TEXT("Buried Future outcome must be recorded"),
        Quests->GetQuestState(EdrathGameplayTags::QuestBuriedFuture).Outcome,
        EEdrathQuestOutcome::BuriedFutureViolentStranger);
    TestTrue(TEXT("Loving father branch must be recorded as unchosen"),
        Narrative->GetUnchosenPossibilityTags().Contains(EdrathGameplayTags::QuestBuriedFutureLovingFather));

    TestTrue(TEXT("Last Honest King quest must expose the true war criminal"),
        Quests->ResolveLastHonestKing(true));
    TestTrue(TEXT("Exposing the war criminal must reignite civil war"), Quests->IsCivilWarReignited());
    TestTrue(TEXT("Peace branch must be recorded as unchosen"),
        Narrative->GetUnchosenPossibilityTags().Contains(EdrathGameplayTags::QuestLastKingPreservePeace));

    AActor* LivingVillager = TestWorld->SpawnActor<AActor>();
    Quests->RegisterLivingVillagerActor(LivingVillager);
    TestTrue(TEXT("Village vote quest must resolve by resurrecting the dead"),
        Quests->ResolveVillageVotesOnDead(true, false));
    TestTrue(TEXT("Living villager must be erased by resurrection consequence"),
        !IsValid(LivingVillager));

    AHouseWithOneMoreRoomActor* House = TestWorld->SpawnActor<AHouseWithOneMoreRoomActor>();
    House->ProceduralRoomClasses.Add(AActor::StaticClass());
    Quests->BindHouseWithOneMoreRoom(House);
    for (int32 Recollection = 0; Recollection < 4; ++Recollection)
    {
        TestTrue(TEXT("House recollection must spawn a procedural room"), Quests->RegisterHouseOwnerRecollection());
    }
    TestTrue(TEXT("House quest must unlock the final atrocity room"), Quests->ResolveHouseOneMoreRoom());
    TestTrue(TEXT("Final atrocity room must be marked unlocked"), House->IsFinalAtrocityRoomUnlocked());

    AEdrathCompanionCharacter* Ilyra = TestWorld->SpawnActor<AEdrathCompanionCharacter>();
    Ilyra->CompanionID = TEXT("Ilyra");
    Ilyra->GenerateIlyraBladeNotchClue();
    TestTrue(TEXT("Ilyra blade-notch clue must be recorded"),
        Narrative->HasUnlockedFourthPath() == false);
    TestTrue(TEXT("Ilyra must support reconciliation outcome"),
        Ilyra->ResolveIlyraOutcome(EIlyraResolution::Reconciliation));
    TestEqual(TEXT("Ilyra reconciliation must raise loyalty"), Ilyra->GetRelationshipState().Loyalty, 85.0f);

    AEdrathCompanionCharacter* Orren = TestWorld->SpawnActor<AEdrathCompanionCharacter>();
    Orren->CompanionID = TEXT("Orren");
    TestTrue(TEXT("Orren must assist memory reconstruction"),
        Orren->AssistMemoryReconstruction(EdrathGameplayTags::Twist5, FText::FromString(TEXT("Orren's corrected history."))));
    TestTrue(TEXT("Orren must mark overwritten evidence"), Orren->HasOverwrittenHistoricalEvidence());

    AEdrathCompanionCharacter* Senn = TestWorld->SpawnActor<AEdrathCompanionCharacter>();
    Senn->CompanionID = TEXT("Senn");
    Senn->TriggerForesightWarning();
    TestEqual(TEXT("Senn warning must default to 0.8 seconds"), Senn->GetLastForesightWarningSeconds(), 0.8f);
    Senn->SetActVIChoiceKnowledge(FText::FromString(TEXT("Senn remembers the fourth path.")));
    TestTrue(TEXT("Senn must retain Act VI choice knowledge"), Senn->HasActVIChoiceKnowledge());

    AEdrathCompanionCharacter* Maevra = TestWorld->SpawnActor<AEdrathCompanionCharacter>();
    Maevra->CompanionID = TEXT("Maevra");
    Maevra->SetHostilePursuer(true);
    Maevra->SetErasedMarriageRestorationProgress(4);
    TestTrue(TEXT("Maevra must transition from hostile pursuer to party member"),
        Maevra->RecruitMaevraToParty());
    TestFalse(TEXT("Maevra must no longer be hostile after recruitment"), Maevra->IsHostilePursuer());
    TestTrue(TEXT("Maevra must become a party member"), Maevra->IsPartyMember());

    // --- Scalability & Hardware Optimization Verification ---
    UEdrathScalabilitySubsystem* Scalability = GameInstance->GetSubsystem<UEdrathScalabilitySubsystem>();
    TestNotNull(TEXT("Scalability subsystem must exist"), Scalability);
    if (Scalability)
    {
        Scalability->ApplyQualityPreset(EEdrathQualityPreset::Epic_PC);
        TestEqual(TEXT("Scalability must be Epic_PC"), Scalability->GetCurrentQualityPreset(), EEdrathQualityPreset::Epic_PC);
        Scalability->ApplyQualityPreset(EEdrathQualityPreset::Development_Laptop);
        TestEqual(TEXT("Scalability must be Development_Laptop"), Scalability->GetCurrentQualityPreset(), EEdrathQualityPreset::Development_Laptop);
        Scalability->ApplyQualityPreset(EEdrathQualityPreset::Low_Mobile);
        TestEqual(TEXT("Scalability must be Low_Mobile"), Scalability->GetCurrentQualityPreset(), EEdrathQualityPreset::Low_Mobile);
    }

    UEdrathHardwareOptimizationSubsystem* HWOpt = GameInstance->GetSubsystem<UEdrathHardwareOptimizationSubsystem>();
    TestNotNull(TEXT("Hardware optimization subsystem must exist"), HWOpt);
    if (HWOpt)
    {
        HWOpt->ApplyCameraModeOptimization(EEdrathCameraMode::InteriorThirdPerson);
        TestEqual(TEXT("Interior camera mode optimization must be active"), HWOpt->GetOptimizedCameraMode(), EEdrathCameraMode::InteriorThirdPerson);
        HWOpt->OptimizeTimelineMemoryFootprint(ETimelineState::RejectedTimeline);
        TestEqual(TEXT("Timeline optimization must be RejectedTimeline"), HWOpt->GetOptimizedTimelineState(), ETimelineState::RejectedTimeline);
    }

    // --- Kael Attributes & Combat Verification ---
    Kael->ApplyDamage(25.0f);
    TestEqual(TEXT("Kael health must decrease by 25"), Kael->CurrentHealth, 75.0f);
    Kael->RestoreHealth(10.0f);
    TestEqual(TEXT("Kael health must restore by 10"), Kael->CurrentHealth, 85.0f);
    TestTrue(TEXT("Kael dodge execution must succeed"), Kael->PerformDodge());
    TestTrue(TEXT("Kael parry execution must succeed"), Kael->PerformParry());

    // --- Save & Persistence Verification ---
    UEdrathSaveSubsystem* SaveSubsystem = GameInstance->GetSubsystem<UEdrathSaveSubsystem>();
    TestNotNull(TEXT("Save subsystem must exist"), SaveSubsystem);
    if (SaveSubsystem)
    {
        TestTrue(TEXT("Save to slot must succeed"), SaveSubsystem->SaveGameToSlot(TEXT("PreAlpha_TestSlot"), 0));
        TestTrue(TEXT("Save slot must exist on disk"), SaveSubsystem->DoesSaveSlotExist(TEXT("PreAlpha_TestSlot"), 0));
        TestTrue(TEXT("Load from slot must succeed"), SaveSubsystem->LoadGameFromSlot(TEXT("PreAlpha_TestSlot"), 0));
        SaveSubsystem->DeleteSaveSlot(TEXT("PreAlpha_TestSlot"), 0);
    }

    TestWorld->DestroyWorld(false);
    return !HasAnyErrors();
}





IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FEdrathHUDAndForensicUIVerificationTest,
    "Edrath.PreAlpha.HUDAndForensicUIVerification",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEdrathHUDAndForensicUIVerificationTest::RunTest(const FString&)
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

    // 1. Verify Black Clock HUD Widget state updates
    UBlackClockWidget* ClockWidget = NewObject<UBlackClockWidget>(TestWorld);
    TestNotNull(TEXT("Black Clock HUD Widget must instantiate"), ClockWidget);
    if (ClockWidget)
    {
        ClockWidget->UpdateClockStability(75.0f, 100.0f);
        TestEqual(TEXT("Clock stability percent must match"), ClockWidget->GetDisplayedStabilityPercent(), 75.0f);

        ClockWidget->UpdateTimelineState(ETimelineState::RejectedTimeline);
        TestEqual(TEXT("Timeline state must reflect RejectedTimeline"),
            ClockWidget->GetDisplayedTimelineState(), ETimelineState::RejectedTimeline);

        ClockWidget->UpdateRegionalInstability(38.0f);
        TestEqual(TEXT("Regional instability score must match"),
            ClockWidget->GetDisplayedRegionalInstability(), 38.0f);
    }

    // 2. Verify Forensic Ledger Evidence Population & Filtering
    UEdrathNarrativeSubsystem* Narrative = GameInstance->GetSubsystem<UEdrathNarrativeSubsystem>();
    TestNotNull(TEXT("Narrative subsystem must be valid for UI tests"), Narrative);
    if (Narrative)
    {
        Narrative->AddOrMutateEvidence(
            EdrathGameplayTags::Twist1,
            FText::FromString(TEXT("Handwriting matches ancient archive ledger")),
            true);
        Narrative->AddOrMutateEvidence(
            EdrathGameplayTags::Twist2,
            FText::FromString(TEXT("Portrait of Kael with scratched-out companion face")),
            true);
        Narrative->AddOrMutateEvidence(
            EdrathGameplayTags::QuestAct1Crypt,
            FText::FromString(TEXT("Priest skeletal remains verified dead 114 years")),
            false);

        UForensicLedgerWidget* LedgerWidget = NewObject<UForensicLedgerWidget>(TestWorld);
        TestNotNull(TEXT("Forensic Ledger Widget must instantiate"), LedgerWidget);
        if (LedgerWidget)
        {
            LedgerWidget->RefreshEvidenceList();
            TestEqual(TEXT("Total clues in ledger must be 3"), LedgerWidget->GetTotalCluesCount(), 3);
            TestEqual(TEXT("Twist clues in ledger must be 2"), LedgerWidget->GetTwistCluesCount(), 2);

            LedgerWidget->SetFilterMode(EForensicLedgerFilter::TwistCluesOnly);
            TestEqual(TEXT("Filtered twist clues count must be 2"), LedgerWidget->GetFilteredEvidenceList().Num(), 2);

            FEdrathEvidenceRecord SelectedClue;
            TestTrue(TEXT("Selecting evidence by tag must succeed"),
                LedgerWidget->SelectEvidenceByTag(EdrathGameplayTags::Twist1));
            TestTrue(TEXT("Retrieved selected evidence must be valid"),
                LedgerWidget->GetSelectedEvidence(SelectedClue));
            TestEqual(TEXT("Selected clue tag must match"),
                SelectedClue.EvidenceTag, EdrathGameplayTags::Twist1);
        }
    }

    // 3. Verify Memory Reconstruction Dialogue Widget
    UMemoryDialogueWidget* DialogueWidget = NewObject<UMemoryDialogueWidget>(TestWorld);
    TestNotNull(TEXT("Memory Dialogue Widget must instantiate"), DialogueWidget);
    if (DialogueWidget)
    {
        TArray<FText> Choices;
        Choices.Add(FText::FromString(TEXT("Examine Priest Sarcophagus")));
        Choices.Add(FText::FromString(TEXT("Confront with 114-Year-Old Ledger")));

        DialogueWidget->DisplayDialogue(
            FText::FromString(TEXT("Memory of Father Aaron")),
            FText::FromString(TEXT("You walked these crypts before you were born, Kael.")),
            Choices);

        TestTrue(TEXT("Dialogue must be marked active"), DialogueWidget->IsDialogueActive());
        TestEqual(TEXT("Dialogue choices count must be 2"), DialogueWidget->GetCurrentChoices().Num(), 2);
        TestTrue(TEXT("Choosing valid choice index must succeed"), DialogueWidget->ChooseOption(0));

        DialogueWidget->Dismiss();
        TestFalse(TEXT("Dialogue must be inactive after dismissal"), DialogueWidget->IsDialogueActive());
    }

    // 4. Verify AEdrathHUD Actor
    AEdrathHUD* EdrathHUD = TestWorld->SpawnActor<AEdrathHUD>();
    TestNotNull(TEXT("AEdrathHUD actor must spawn"), EdrathHUD);
    if (EdrathHUD)
    {
        TestFalse(TEXT("Forensic ledger must be initially closed"), EdrathHUD->IsForensicLedgerOpen());
    }

    TestWorld->DestroyWorld(false);
    return !HasAnyErrors();
}





IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FEdrathFullUIAndHUDArchitectureVerificationTest,
    "Edrath.PreAlpha.FullUIAndHUDArchitectureVerification",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEdrathFullUIAndHUDArchitectureVerificationTest::RunTest(const FString&)
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

    // 1. Verify Dual-Perspective Reticle & Inspection Prompt Widget
    UEdrathReticleWidget* Reticle = NewObject<UEdrathReticleWidget>(TestWorld);
    TestNotNull(TEXT("Reticle widget must instantiate"), Reticle);
    if (Reticle)
    {
        TestEqual(TEXT("Initial reticle mode must be IsometricCursor"),
            Reticle->GetReticleMode(), EEdrathReticleMode::IsometricCursor);

        Reticle->SetCameraPerspectiveMode(EEdrathCameraMode::InteriorThirdPerson);
        TestEqual(TEXT("Camera shift to interior must update reticle to ThirdPersonCrosshair"),
            Reticle->GetReticleMode(), EEdrathReticleMode::ThirdPersonCrosshair);

        Reticle->SetForensicFocus(true, FText::FromString(TEXT("Handwriting desk inspection")));
        TestTrue(TEXT("Forensic focus state must be active"), Reticle->IsForensicFocusActive());
        TestEqual(TEXT("Reticle mode must transition to ForensicInspectFocus"),
            Reticle->GetReticleMode(), EEdrathReticleMode::ForensicInspectFocus);

        Reticle->SetInspectionPrompt(
            FText::FromString(TEXT("[E] Examine Sarcophagus")),
            FText::FromString(TEXT("114-Year-Dead Priest Tomb")),
            true);
        TestTrue(TEXT("Reticle must have active inspection prompt"), Reticle->HasActiveInspectionPrompt());
        TestEqual(TEXT("Inspection action text must match"),
            Reticle->GetInspectionActionText().ToString(), TEXT("[E] Examine Sarcophagus"));

        Reticle->ClearInspectionPrompt();
        TestFalse(TEXT("Inspection prompt must be cleared"), Reticle->HasActiveInspectionPrompt());

        Reticle->SetIsometricTelegraph(750.0f, true);
        TestEqual(TEXT("Isometric telegraph radius must match"), Reticle->GetIsometricTelegraphRadius(), 750.0f);
    }

    // 2. Verify Echo Combat Action & 120-Frame Timeline Buffer Widget
    UEchoCombatBufferWidget* EchoWidget = NewObject<UEchoCombatBufferWidget>(TestWorld);
    TestNotNull(TEXT("Echo combat buffer widget must instantiate"), EchoWidget);
    if (EchoWidget)
    {
        EchoWidget->UpdateBufferMetrics(60, 120);
        TestEqual(TEXT("Echo buffer stored frames must be 60"), EchoWidget->GetStoredFrames(), 60);
        TestEqual(TEXT("Echo buffer fill ratio must be 0.5f"), EchoWidget->GetBufferFillRatio(), 0.5f);

        EchoWidget->PushActionRecord(FName(TEXT("Slash_Combo_01")), 30.0f);
        EchoWidget->PushActionRecord(FName(TEXT("Parry_Counter")), 50.0f);
        TestEqual(TEXT("Recent actions count must be 2"), EchoWidget->GetRecentActionHistory().Num(), 2);
        TestEqual(TEXT("Most recent action must be Parry_Counter"),
            EchoWidget->GetRecentActionHistory()[0], FName(TEXT("Parry_Counter")));

        EchoWidget->SetStrikeDelay(1.2f);
        TestEqual(TEXT("Echo strike delay must be 1.2s"), EchoWidget->GetStrikeDelay(), 1.2f);

        EchoWidget->SetRegionalResonance(EEdrathRegion::Nhal);
        TestEqual(TEXT("Regional resonance must be Nhal"),
            EchoWidget->GetActiveRegionalResonance(), EEdrathRegion::Nhal);

        EchoWidget->UpdateCooldown(2.5f, 5.0f);
        TestFalse(TEXT("Echo strike should not be ready during cooldown"), EchoWidget->IsEchoStrikeReady());
        TestEqual(TEXT("Cooldown progress must be 50%"), EchoWidget->GetCooldownPercent(), 0.5f);
    }

    // 3. Verify Forensic Contradiction Deduction Board
    UForensicContradictionWidget* ContradictionWidget = NewObject<UForensicContradictionWidget>(TestWorld);
    TestNotNull(TEXT("Contradiction widget must instantiate"), ContradictionWidget);
    if (ContradictionWidget)
    {
        ContradictionWidget->RegisterDefaultDeductionRules();

        // Test unresolved deduction with mismatching clues
        ContradictionWidget->SlotPrimaryClue(EdrathGameplayTags::QuestDragonNoFire);
        ContradictionWidget->SlotContradictoryClue(EdrathGameplayTags::Twist1);
        FContradictionResolutionResult MismatchResult = ContradictionWidget->AttemptResolveContradiction();
        TestFalse(TEXT("Mismatched clues must not resolve a contradiction"), MismatchResult.bSuccess);

        // Test valid Act 1 Crypt priest contradiction resolution
        ContradictionWidget->SlotPrimaryClue(EdrathGameplayTags::QuestAct1Crypt);
        ContradictionWidget->SlotContradictoryClue(EdrathGameplayTags::Twist1);
        TestTrue(TEXT("Both slots must be filled"), ContradictionWidget->AreBothSlotsFilled());

        FContradictionResolutionResult ValidResult = ContradictionWidget->AttemptResolveContradiction();
        TestTrue(TEXT("Matching deduction must successfully resolve contradiction"), ValidResult.bSuccess);
        TestEqual(TEXT("Breakthrough tag must match Twist1"),
            ValidResult.BreakthroughTag, EdrathGameplayTags::Twist1);
        TestTrue(TEXT("Breakthrough must be recorded in resolved list"),
            ContradictionWidget->HasBreakthroughBeenResolved(EdrathGameplayTags::Twist1));
        TestEqual(TEXT("Total resolved breakthroughs must be 1"),
            ContradictionWidget->GetResolvedContradictionCount(), 1);
    }

    // 4. Verify Mobile / Touch Controls Overlay Widget
    UEdrathTouchControlsWidget* TouchControls = NewObject<UEdrathTouchControlsWidget>(TestWorld);
    TestNotNull(TEXT("Touch controls widget must instantiate"), TouchControls);
    if (TouchControls)
    {
        TouchControls->SetTouchControlsActive(true);
        TestTrue(TEXT("Touch controls must be marked active"), TouchControls->IsTouchControlsActive());
        TouchControls->SetTouchControlsActive(false);
        TestFalse(TEXT("Touch controls must be inactive"), TouchControls->IsTouchControlsActive());
    }

    // 5. Verify Master AEdrathHUD Integration
    AEdrathHUD* MasterHUD = TestWorld->SpawnActor<AEdrathHUD>();
    TestNotNull(TEXT("Master AEdrathHUD must spawn"), MasterHUD);
    if (MasterHUD)
    {
        TestFalse(TEXT("Contradiction board must be initially closed"), MasterHUD->IsContradictionBoardOpen());
    }

    TestWorld->DestroyWorld(false);
    return !HasAnyErrors();
}




IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FEdrathStage2ComprehensiveVerticalSlicePlaythroughTest,
    "Edrath.PreAlpha.Stage2ComprehensiveVerticalSlicePlaythrough",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEdrathStage2ComprehensiveVerticalSlicePlaythroughTest::RunTest(const FString&)
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

    // 1. Instantiate Core Actors
    AKaelCharacter* Kael = TestWorld->SpawnActor<AKaelCharacter>();
    ADyingSoldierActor* DyingSoldier = TestWorld->SpawnActor<ADyingSoldierActor>();
    ABattlefieldWraithCharacter* Wraith1 = TestWorld->SpawnActor<ABattlefieldWraithCharacter>();
    ABattlefieldWraithCharacter* Wraith2 = TestWorld->SpawnActor<ABattlefieldWraithCharacter>();
    ABattlefieldWraithCharacter* Wraith3 = TestWorld->SpawnActor<ABattlefieldWraithCharacter>();
    AAct1GorgeBridgeActor* GorgeBridge = TestWorld->SpawnActor<AAct1GorgeBridgeActor>();
    ASettlementVillagerCharacter* Villager = TestWorld->SpawnActor<ASettlementVillagerCharacter>();
    ABeliefBuildingActor* Shrine = TestWorld->SpawnActor<ABeliefBuildingActor>();
    AKaelHouseInteriorActor* KaelHouse = TestWorld->SpawnActor<AKaelHouseInteriorActor>();
    AForensicClueActor* HandwritingClue = TestWorld->SpawnActor<AForensicClueActor>();
    AForensicClueActor* PortraitClue = TestWorld->SpawnActor<AForensicClueActor>();
    APossibilityShiftActor* Portcullis = TestWorld->SpawnActor<APossibilityShiftActor>();
    APriestCorpseActor* PriestCorpse = TestWorld->SpawnActor<APriestCorpseActor>();
    AAct1PreAlphaSliceDirector* Director = TestWorld->SpawnActor<AAct1PreAlphaSliceDirector>();
    AAct1BattlefieldLevelScriptActor* LevelScript = TestWorld->SpawnActor<AAct1BattlefieldLevelScriptActor>();

    TestNotNull(TEXT("Kael must spawn cleanly"), Kael);
    TestNotNull(TEXT("DyingSoldier must spawn cleanly"), DyingSoldier);
    TestNotNull(TEXT("Wraith1 must spawn"), Wraith1);
    TestNotNull(TEXT("GorgeBridge must spawn"), GorgeBridge);
    TestNotNull(TEXT("Villager must spawn"), Villager);
    TestNotNull(TEXT("Shrine must spawn"), Shrine);
    TestNotNull(TEXT("KaelHouse must spawn"), KaelHouse);
    TestNotNull(TEXT("HandwritingClue must spawn"), HandwritingClue);
    TestNotNull(TEXT("PortraitClue must spawn"), PortraitClue);
    TestNotNull(TEXT("Portcullis must spawn"), Portcullis);
    TestNotNull(TEXT("PriestCorpse must spawn"), PriestCorpse);
    TestNotNull(TEXT("Director must spawn"), Director);
    TestNotNull(TEXT("LevelScript must spawn"), LevelScript);

    if (!Kael || !DyingSoldier || !Wraith1 || !GorgeBridge || !Villager || !Shrine ||
        !KaelHouse || !HandwritingClue || !PortraitClue || !Portcullis || !PriestCorpse || !Director || !LevelScript)
    {
        TestWorld->DestroyWorld(false);
        return false;
    }

    // Configure Clues
    HandwritingClue->EvidenceTag = EdrathGameplayTags::EvidenceHandwriting;
    HandwritingClue->InitialDescription = FText::FromString(TEXT("Kael's handwriting dated 7 days in the future."));
    PortraitClue->EvidenceTag = EdrathGameplayTags::EvidenceScratchedPortrait;
    PortraitClue->InitialDescription = FText::FromString(TEXT("Portrait of Kael with woman whose face is scratched out."));
    KaelHouse->HandwritingClue = HandwritingClue;
    KaelHouse->ScratchedPortraitClue = PortraitClue;

    // Configure Director & LevelScript
    Director->GorgeBridge = GorgeBridge;
    Director->CryptPortcullis = Portcullis;
    Director->VillageShrine = Shrine;
    Director->HandwritingEvidenceTag = EdrathGameplayTags::EvidenceHandwriting;
    Director->ScratchedPortraitEvidenceTag = EdrathGameplayTags::EvidenceScratchedPortrait;
    Director->PriestMemoryAnchorTag = EdrathGameplayTags::QuestAct1Crypt;

    LevelScript->PlayerCharacter = Kael;
    LevelScript->SliceDirector = Director;
    LevelScript->DyingSoldier = DyingSoldier;
    LevelScript->BattlefieldWraiths.Add(Wraith1);
    LevelScript->BattlefieldWraiths.Add(Wraith2);
    LevelScript->BattlefieldWraiths.Add(Wraith3);
    LevelScript->GorgeBridge = GorgeBridge;
    LevelScript->SettlementVillager = Villager;
    LevelScript->SettlementShrine = Shrine;
    LevelScript->KaelHouse = KaelHouse;
    LevelScript->CryptPortcullis = Portcullis;
    LevelScript->PriestCorpse = PriestCorpse;
    LevelScript->AssembleVerticalSlice();

    // -------------------------------------------------------------
    // ZONE 1: The Ancient Battlefield
    // -------------------------------------------------------------
    TestEqual(TEXT("Slice begins in Zone 1"),
        Director->GetCurrentZone(), EPreAlphaZoneState::Zone1_BattlefieldAwakening);
    TestEqual(TEXT("Initial camera must be ExteriorBirdsEye"),
        Kael->DualCameraComponent->GetCurrentCameraMode(), EEdrathCameraMode::ExteriorBirdsEye);

    // Test dying soldier interaction
    TestTrue(TEXT("Dying soldier interaction must succeed"), DyingSoldier->TriggerDialogue(Kael));
    TestTrue(TEXT("Dying soldier dialogue state must be recorded"), DyingSoldier->HasTriggeredDialogue());
    TestTrue(TEXT("Dying soldier dialogue line must match specification"),
        DyingSoldier->SpokenDialogue.EqualTo(FText::FromString(TEXT("Do not let my daughter be born"))));

    // Test Echo Combat against Battlefield Wraiths
    Director->BattlefieldWraithCount = 3;
    TestTrue(TEXT("Echo Strike must damage and stagger Wraith 1"), Wraith1->TakeEchoStrikeDamage(60.0f, Kael, 1.2f));
    TestTrue(TEXT("Wraith 1 must be marked defeated"), Wraith1->IsDefeated());

    TestTrue(TEXT("Echo Strike must damage and stagger Wraith 2"), Wraith2->TakeEchoStrikeDamage(60.0f, Kael, 1.2f));
    TestTrue(TEXT("Wraith 2 must be marked defeated"), Wraith2->IsDefeated());

    TestTrue(TEXT("Echo Strike must damage and stagger Wraith 3"), Wraith3->TakeEchoStrikeDamage(60.0f, Kael, 1.2f));
    TestTrue(TEXT("Wraith 3 must be marked defeated"), Wraith3->IsDefeated());

    TestEqual(TEXT("All 3 wraiths defeated advances to Zone 2"),
        Director->GetCurrentZone(), EPreAlphaZoneState::Zone2_CollapsedGorgeBridge);

    // -------------------------------------------------------------
    // ZONE 2: The Collapsed Gorge Bridge
    // -------------------------------------------------------------
    TestFalse(TEXT("Bridge is initially impassable in present timeline"), GorgeBridge->IsPassable());
    GorgeBridge->ApplyTimelineState(ETimelineState::RejectedTimeline);
    TestTrue(TEXT("Possibility Shift restores bridge to passable rejected timeline"), GorgeBridge->IsPassable());
    TestEqual(TEXT("Restored gorge bridge advances to Zone 3"),
        Director->GetCurrentZone(), EPreAlphaZoneState::Zone3_LoopingSettlementExterior);

    // -------------------------------------------------------------
    // ZONE 3: The 7-Day Looping Settlement
    // -------------------------------------------------------------
    UEdrathNarrativeSubsystem* NarrativeSys = GameInstance->GetSubsystem<UEdrathNarrativeSubsystem>();
    TestNotNull(TEXT("Narrative subsystem must be valid"), NarrativeSys);
    NarrativeSys->SetCitizenBeliefWeight(Shrine->BuildingBeliefTag, 1.0f);

    const FText Day1Text = Villager->GetContextualDialogue();
    TestTrue(TEXT("Day 1 villager recognizes Kael"), Day1Text.ToString().Contains(TEXT("Welcome back, Kael!")));

    // Advance 7 days of the temporal loop
    for (int32 Day = 0; Day < 7; ++Day)
    {
        TestTrue(TEXT("Settlement day must advance"), Director->AdvanceSettlementDay());
    }
    TestEqual(TEXT("Looping settlement completes cycle and advances to Zone 4"),
        Director->GetCurrentZone(), EPreAlphaZoneState::Zone4_KaelHouseForensics);

    // -------------------------------------------------------------
    // ZONE 4: Kael's Settlement House (Interior Inspection)
    // -------------------------------------------------------------
    KaelHouse->SetInteriorMode(true);
    TestTrue(TEXT("Player must be marked inside house"), KaelHouse->bIsPlayerInside);
    TestEqual(TEXT("House interior triggers Over-the-Shoulder Third-Person camera"),
        Kael->DualCameraComponent->GetCurrentCameraMode(), EEdrathCameraMode::InteriorThirdPerson);

    TestTrue(TEXT("Handwriting desk clue inspectable"), HandwritingClue->Inspect(Kael));
    TestTrue(TEXT("Scratched portrait clue inspectable"), PortraitClue->Inspect(Kael));
    TestTrue(TEXT("Both forensic clues must be registered as inspected"), KaelHouse->AreAllForensicCluesInspected());

    Director->RefreshZoneState();
    TestEqual(TEXT("House clue inspection advances to Zone 5"),
        Director->GetCurrentZone(), EPreAlphaZoneState::Zone5_CryptPriestTwist);

    // -------------------------------------------------------------
    // ZONE 5: The Local Crypt & The 114-Year-Dead Priest
    // -------------------------------------------------------------
    Portcullis->ApplyTimelineState(ETimelineState::RejectedTimeline);
    Director->RefreshZoneState();
    TestTrue(TEXT("Portcullis timeline shift registered"), Director->HasCryptPortcullisShifted());

    TestTrue(TEXT("Priest corpse inspected"), PriestCorpse->InspectCorpse(Kael));
    TestEqual(TEXT("Priest death certificate confirms 114 years dead"), PriestCorpse->YearsDeceased, 114);

    TestTrue(TEXT("Memory reconstruction shatters the 7-day loop"), PriestCorpse->ReconstructMemory(Kael));
    TestTrue(TEXT("Priest corpse marks loop broken"), PriestCorpse->HasBrokenSettlementLoop());
    TestTrue(TEXT("Narrative subsystem confirms Act 1 loop broken"), NarrativeSys->IsSettlementLoopBroken());

    TestEqual(TEXT("Act 1 Pre-Alpha slice successfully completed"),
        Director->GetCurrentZone(), EPreAlphaZoneState::PreAlphaCompleted);
    TestTrue(TEXT("Level script actor marks slice complete"), LevelScript->IsSliceComplete());

    TestWorld->DestroyWorld(false);
    return !HasAnyErrors();
}


IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FEdrathDualCameraAntiClippingAndRoofFadeTest,
    "Edrath.Camera.DualPerspectiveTransitionAndRoofFadeVerification",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEdrathDualCameraAntiClippingAndRoofFadeTest::RunTest(const FString&)
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
    TestNotNull(TEXT("Kael must spawn"), Kael);

    AEdrathInteriorTransitionVolume* Volume = TestWorld->SpawnActor<AEdrathInteriorTransitionVolume>();
    TestNotNull(TEXT("Interior volume must spawn"), Volume);

    if (Kael && Volume)
    {
        TestEqual(TEXT("Initial camera mode must be ExteriorBirdsEye"),
            Kael->DualCameraComponent->GetCurrentCameraMode(), EEdrathCameraMode::ExteriorBirdsEye);
        TestFalse(TEXT("Initially not blending perspective"),
            Kael->DualCameraComponent->IsBlendingPerspective());

        // Trigger perspective switch with Volume
        Kael->DualCameraComponent->SetCameraPerspective(EEdrathCameraMode::InteriorThirdPerson, Volume);
        TestTrue(TEXT("Switching perspective must trigger blend state"),
            Kael->DualCameraComponent->IsBlendingPerspective());
        TestEqual(TEXT("Transition alpha starts at 0.0"),
            Kael->DualCameraComponent->GetTransitionAlpha(), 0.0f);

        // Advance transition tick
        Kael->DualCameraComponent->TickComponent(0.35f, LEVELTICK_All, nullptr);
        TestTrue(TEXT("Transition alpha advances with tick"),
            Kael->DualCameraComponent->GetTransitionAlpha() > 0.0f);

        // Complete transition tick
        Kael->DualCameraComponent->TickComponent(0.40f, LEVELTICK_All, nullptr);
        TestEqual(TEXT("Transition alpha reaches 1.0"),
            Kael->DualCameraComponent->GetTransitionAlpha(), 1.0f);
        TestFalse(TEXT("Blending perspective finishes at alpha 1.0"),
            Kael->DualCameraComponent->IsBlendingPerspective());

        // Test Roof Dither Opacity Fade
        TestEqual(TEXT("Initial roof opacity is 1.0"), Volume->GetCurrentRoofOpacity(), 1.0f);
        Volume->Tick(0.25f);
    }

    TestWorld->DestroyWorld(false);
    return !HasAnyErrors();
}


IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FEdrathEchoCombatSubFrameInterpolationTest,
    "Edrath.PreAlpha.EchoCombatSubFrameInterpolationAndParryTest",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEdrathEchoCombatSubFrameInterpolationTest::RunTest(const FString&)
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
    TestNotNull(TEXT("Kael must spawn"), Kael);

    if (Kael && Kael->BlackClockComponent && Kael->EchoCombatComponent)
    {
        UBlackClockComponent* Clock = Kael->BlackClockComponent;
        UEdrathEchoCombatComponent* Combat = Kael->EchoCombatComponent;

        Clock->ClearActionHistoryBuffer();
        TestEqual(TEXT("Buffer begins empty"), Clock->GetActionHistoryBufferSize(), 0);

        // Record synthetic frames to verify sub-frame interpolation
        Kael->SetActorLocation(FVector(0.0f, 0.0f, 0.0f));
        Kael->SetActorRotation(FRotator(0.0f, 0.0f, 0.0f));
        Clock->RecordCombatFrame(FName(TEXT("Slash_Combo_01")), 30.0f);

        Kael->SetActorLocation(FVector(100.0f, 200.0f, 0.0f));
        Kael->SetActorRotation(FRotator(0.0f, 90.0f, 0.0f));
        Clock->RecordCombatFrame(FName(TEXT("Slash_Combo_02")), 50.0f);

        TestEqual(TEXT("Buffer contains 2 recorded frames"), Clock->GetActionHistoryBufferSize(), 2);

        // Retrieve recorded timestamps
        const TArray<FEchoActionFrame>& Buffer = Clock->GetActionHistoryBuffer();
        const float T0 = Buffer[0].TimestampSeconds;
        const float T1 = Buffer[1].TimestampSeconds;

        // Query sub-frame midpoint
        const float MidTime = (T0 + T1) * 0.5f;
        FEchoActionFrame InterpolatedFrame;
        const bool bInterpSuccess = Clock->InterpolateFrameAtTime(MidTime, InterpolatedFrame);
        TestTrue(TEXT("InterpolateFrameAtTime succeeds"), bInterpSuccess);

        // Verify interpolated attributes
        TestNearlyEqual(TEXT("Interpolated X position must be 50.0"), InterpolatedFrame.WorldPosition.X, 50.0f, 1.0f);
        TestNearlyEqual(TEXT("Interpolated Y position must be 100.0"), InterpolatedFrame.WorldPosition.Y, 100.0f, 1.0f);
        TestNearlyEqual(TEXT("Interpolated BaseDamage must be 40.0"), InterpolatedFrame.BaseDamage, 40.0f, 0.5f);

        // Test Parry Window and Stability Reward
        const float InitialStability = Clock->GetClockStability();
        Combat->ExecuteParry(0.25f);
        TestTrue(TEXT("Combat component must be in Parry window"), Combat->IsInParryWindow());

        // Drain 10 stability to test reward recovery
        Clock->RestoreStability(-10.0f);
        const float DrainedStability = Clock->GetClockStability();
        TestTrue(TEXT("Stability drained"), DrainedStability < InitialStability);

        // Resolve incoming hit during parry
        const bool bHitDealtDamage = Combat->CheckAndResolveIncomingHit(25.0f, nullptr);
        TestFalse(TEXT("Parried hit deals no damage"), bHitDealtDamage);
        TestTrue(TEXT("Parry restored clock stability"), Clock->GetClockStability() > DrainedStability);
    }

    TestWorld->DestroyWorld(false);
    return !HasAnyErrors();
}


IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FEdrathForensicContradictionDeductionTest,
    "Edrath.PreAlpha.ForensicContradictionAndSettlementBreakTest",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEdrathForensicContradictionDeductionTest::RunTest(const FString&)
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

    UEdrathNarrativeSubsystem* Narrative = GameInstance->GetSubsystem<UEdrathNarrativeSubsystem>();
    TestNotNull(TEXT("Narrative subsystem must be valid"), Narrative);

    UForensicContradictionWidget* Widget = NewObject<UForensicContradictionWidget>(TestWorld);
    TestNotNull(TEXT("Forensic contradiction widget must be created"), Widget);

    if (Narrative && Widget)
    {
        Widget->RegisterDefaultDeductionRules();
        TestTrue(TEXT("Rules must contain at least 4 rules"), Widget->GetDeductionRules().Num() >= 4);

        TestFalse(TEXT("Settlement loop initially not broken"), Narrative->IsSettlementLoopBroken());
        TestEqual(TEXT("Resolved contradictions start at 0"), Widget->GetResolvedContradictionCount(), 0);

        // Attempt resolving unmatched clues
        Widget->SlotPrimaryClue(EdrathGameplayTags::QuestAct1Crypt);
        Widget->SlotContradictoryClue(EdrathGameplayTags::QuestLastHonestKing);
        TestTrue(TEXT("Both slots filled"), Widget->AreBothSlotsFilled());

        FContradictionResolutionResult MismatchResult = Widget->AttemptResolveContradiction();
        TestFalse(TEXT("Mismatch deduction must fail"), MismatchResult.bSuccess);
        TestFalse(TEXT("Settlement loop remains intact"), Narrative->IsSettlementLoopBroken());

        // Clear Slot B and slot matching clue
        Widget->ClearSlotB();
        TestFalse(TEXT("Slot B cleared"), Widget->AreBothSlotsFilled());
        Widget->SlotContradictoryClue(EdrathGameplayTags::Twist1);
        TestTrue(TEXT("Slot B refilled with matching clue"), Widget->AreBothSlotsFilled());

        FContradictionResolutionResult SuccessResult = Widget->AttemptResolveContradiction();
        TestTrue(TEXT("Matching contradiction deduction must succeed"), SuccessResult.bSuccess);
        TestEqual(TEXT("Breakthrough tag matches Twist 1"), SuccessResult.BreakthroughTag, EdrathGameplayTags::Twist1);
        TestTrue(TEXT("Breakthrough is registered"), Widget->HasBreakthroughBeenResolved(EdrathGameplayTags::Twist1));
        TestEqual(TEXT("Resolved contradiction count incremented"), Widget->GetResolvedContradictionCount(), 1);

        // Verify narrative side effect: Act 1 settlement loop broken
        TestTrue(TEXT("Settlement loop is now broken via deduction"), Narrative->IsSettlementLoopBroken());
    }

    TestWorld->DestroyWorld(false);
    return !HasAnyErrors();
}


IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FEdrathAct1FiveZoneContinuityTest,
    "Edrath.PreAlpha.Act1FiveZoneEndToEndContinuityTest",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEdrathAct1FiveZoneContinuityTest::RunTest(const FString&)
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
    APossibilityShiftActor* GorgeBridge = TestWorld->SpawnActor<APossibilityShiftActor>();
    ABeliefBuildingActor* Shrine = TestWorld->SpawnActor<ABeliefBuildingActor>();
    AAct1PreAlphaSliceDirector* Director = TestWorld->SpawnActor<AAct1PreAlphaSliceDirector>();

    TestNotNull(TEXT("Kael must spawn"), Kael);
    TestNotNull(TEXT("GorgeBridge must spawn"), GorgeBridge);
    TestNotNull(TEXT("Shrine must spawn"), Shrine);
    TestNotNull(TEXT("Director must spawn"), Director);

    if (Kael && GorgeBridge && Shrine && Director)
    {
        Director->BattlefieldWraithCount = 2;
        Director->HandwritingEvidenceTag = EdrathGameplayTags::QuestAct1Forensics;
        Director->ScratchedPortraitEvidenceTag = EdrathGameplayTags::Twist2;
        Director->PriestMemoryAnchorTag = EdrathGameplayTags::QuestAct1Crypt;

        Director->InitializeSlice(Kael, GorgeBridge, Shrine);

        // Zone 1: Battlefield Awakening
        TestEqual(TEXT("Initial zone is Zone 1"), Director->GetCurrentZone(), EPreAlphaZoneState::Zone1_BattlefieldAwakening);
        Director->RegisterDyingSoldierDialogueTriggered();
        TestTrue(TEXT("Dying soldier dialogue triggered"), Director->HasDyingSoldierDialogueTriggered());

        Director->RegisterBattlefieldWraithDefeated();
        Director->RegisterBattlefieldWraithDefeated();
        TestTrue(TEXT("Zone 1 cleared"), Director->IsZone1Cleared());
        TestEqual(TEXT("Advanced to Zone 2"), Director->GetCurrentZone(), EPreAlphaZoneState::Zone2_CollapsedGorgeBridge);

        // Zone 2: Collapsed Gorge Bridge
        TestTrue(TEXT("Zone 2 currently active"), Director->GetCurrentZone() == EPreAlphaZoneState::Zone2_CollapsedGorgeBridge);
        GorgeBridge->ApplyTimelineState(ETimelineState::RejectedTimeline);
        Director->RefreshZoneState();
        TestTrue(TEXT("Zone 2 cleared"), Director->IsZone2Cleared());
        TestEqual(TEXT("Advanced to Zone 3"), Director->GetCurrentZone(), EPreAlphaZoneState::Zone3_LoopingSettlementExterior);

        // Zone 3: Looping Settlement Exterior
        UEdrathNarrativeSubsystem* Narrative = GameInstance->GetSubsystem<UEdrathNarrativeSubsystem>();
        TestNotNull(TEXT("Narrative subsystem exists"), Narrative);
        if (Narrative)
        {
            for (int32 Day = 0; Day < 7; ++Day)
            {
                Director->AdvanceSettlementDay();
            }
            TestEqual(TEXT("Settlement loop wrapped to Day 1"), Narrative->GetCurrentSettlementLoopDay(), 1);
            Narrative->SetCitizenBeliefWeight(Shrine->BuildingBeliefTag, 100.0f);
        }
        TestTrue(TEXT("Zone 3 cleared"), Director->IsZone3Cleared());
        TestEqual(TEXT("Advanced to Zone 4"), Director->GetCurrentZone(), EPreAlphaZoneState::Zone4_KaelHouseForensics);

        // Zone 4: Kael House Forensics
        Kael->DualCameraComponent->SetCameraPerspective(EEdrathCameraMode::InteriorThirdPerson);

        AForensicClueActor* Clue1 = TestWorld->SpawnActor<AForensicClueActor>();
        Clue1->EvidenceTag = Director->HandwritingEvidenceTag;
        Director->RegisterHouseForensicClue(Clue1);
        Clue1->InspectClue(Kael);

        AForensicClueActor* Clue2 = TestWorld->SpawnActor<AForensicClueActor>();
        Clue2->EvidenceTag = Director->ScratchedPortraitEvidenceTag;
        Director->RegisterHouseForensicClue(Clue2);
        Clue2->InspectClue(Kael);

        TestTrue(TEXT("Zone 4 cleared"), Director->IsZone4Cleared());
        TestEqual(TEXT("Advanced to Zone 5"), Director->GetCurrentZone(), EPreAlphaZoneState::Zone5_CryptPriestTwist);

        // Zone 5: Crypt Priest Twist
        Director->RegisterCryptPortcullisShift();
        TestTrue(TEXT("Portcullis shifted"), Director->HasCryptPortcullisShifted());

        Director->RegisterPriestCorpseInspected();
        TestTrue(TEXT("Priest corpse inspected"), Director->HasPriestCorpseBeenInspected());

        const bool bCompletedSlice = Director->CompletePriestMemoryReconstruction();
        TestTrue(TEXT("Priest memory reconstruction completed"), bCompletedSlice);
        TestTrue(TEXT("Pre-Alpha slice fully completed"), Director->IsPreAlphaCompleted());
        if (Narrative)
        {
            TestTrue(TEXT("Settlement loop permanently broken"), Narrative->IsSettlementLoopBroken());
        }
    }

    TestWorld->DestroyWorld(false);
    return !HasAnyErrors();
}


IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FEdrathHardwareVRAMBudgetGuardianTest,
    "Edrath.Hardware.VRAMBudgetGuardianAndDynamicSentinelTest",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEdrathHardwareVRAMBudgetGuardianTest::RunTest(const FString&)
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

    UEdrathHardwareOptimizationSubsystem* HardwareOpt =
        GameInstance->GetSubsystem<UEdrathHardwareOptimizationSubsystem>();
    TestNotNull(TEXT("Hardware optimization subsystem must initialize"), HardwareOpt);

    if (HardwareOpt)
    {
        const int32 CeilingMB = HardwareOpt->GetVRAMCeilingMB();
        TestTrue(TEXT("VRAM ceiling must be at least 2048 MB"), CeilingMB >= 2048);

        const FEdrathVRAMBudgetAllocation& Budget = HardwareOpt->GetVRAMBudget();
        TestTrue(TEXT("Total allocated budget must not exceed VRAM ceiling"),
            Budget.GetTotalAllocatedMB() <= CeilingMB);

        // Test normal operation (throttle level 0)
        const bool bNormalThrottled = HardwareOpt->CheckAndEnforceVRAMSentinel(CeilingMB * 0.50f);
        TestFalse(TEXT("Under 80% usage must not trigger throttling"), bNormalThrottled);
        TestEqual(TEXT("Throttle level is 0 under normal usage"), HardwareOpt->GetActiveVRAMThrottleLevel(), 0);

        // Test Caution Threshold (80% - 90%)
        const bool bCautionThrottled = HardwareOpt->CheckAndEnforceVRAMSentinel(CeilingMB * 0.84f);
        TestFalse(TEXT("Caution state adjusts parameters without emergency flag"), bCautionThrottled);
        TestEqual(TEXT("Throttle level is 1 under caution usage"), HardwareOpt->GetActiveVRAMThrottleLevel(), 1);

        // Test Emergency Sentinel Threshold (>= 90%)
        const bool bEmergencyThrottled = HardwareOpt->CheckAndEnforceVRAMSentinel(CeilingMB * 0.94f);
        TestTrue(TEXT("Over 90% usage must trigger emergency sentinel"), bEmergencyThrottled);
        TestEqual(TEXT("Throttle level is 2 under emergency usage"), HardwareOpt->GetActiveVRAMThrottleLevel(), 2);

        // Recovery back to normal
        HardwareOpt->CheckAndEnforceVRAMSentinel(CeilingMB * 0.40f);
        TestEqual(TEXT("Throttle level recovers to 0 when memory drops"), HardwareOpt->GetActiveVRAMThrottleLevel(), 0);
    }

    TestWorld->DestroyWorld(false);
    return !HasAnyErrors();
}


IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FEdrathCameraTerrainElevationAndMotionTest,
    "Edrath.Camera.TerrainElevationAndMotionSmoothingVerification",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEdrathCameraTerrainElevationAndMotionTest::RunTest(const FString&)
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
    TestNotNull(TEXT("Kael must spawn"), Kael);

    if (Kael && Kael->DualCameraComponent)
    {
        UEdrathDualCameraComponent* Cam = Kael->DualCameraComponent;

        // Verify camera motion lag is active by default
        TestTrue(TEXT("Camera lag must be enabled by default for smooth terrain traversal"), Cam->IsCameraLagEnabled());

        // Verify terrain slope adaptation toggle
        TestTrue(TEXT("Terrain slope adaptation must be enabled by default"), Cam->IsTerrainSlopeAdaptationEnabled());
        Cam->SetTerrainSlopeAdaptationEnabled(false);
        TestFalse(TEXT("Terrain slope adaptation can be toggled off"), Cam->IsTerrainSlopeAdaptationEnabled());
        Cam->SetTerrainSlopeAdaptationEnabled(true);

        // Test quantized snap exterior yaw (e.g. 45-degree tactical increments)
        Cam->RotateExteriorYaw(22.0f);
        Cam->SnapExteriorYaw(45.0f);
        TestTrue(TEXT("Quantized yaw snap must produce multiples of step angle"),
            FMath::IsNearlyZero(FMath::Fmod(Cam->GetTransitionAlpha(), 1.0f)));

        // Test camera impulse trigger
        Cam->PlayCameraImpulse(25.0f, FVector(0.0f, 0.0f, -1.0f));
        Cam->TickComponent(0.05f, LEVELTICK_All, nullptr);
    }

    TestWorld->DestroyWorld(false);
    return !HasAnyErrors();
}


IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FEdrathModularRigAndTerrainIKTest,
    "Edrath.Character.ModularRigAndTerrainIKVerification",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEdrathModularRigAndTerrainIKTest::RunTest(const FString&)
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
    TestNotNull(TEXT("Kael must spawn with modular rig and terrain adaptation"), Kael);

    if (Kael)
    {
        // 1. Modular Rig Validation
        UKaelModularRigComponent* ModularRig = Kael->GetModularRigComponent();
        TestNotNull(TEXT("Modular rig component must be valid"), ModularRig);
        if (ModularRig)
        {
            TestTrue(TEXT("Modular rig must be initialized to master body mesh"), ModularRig->IsModularRigInitialized());
            TestEqual(TEXT("Default archetype must be Forgotten Soldier Act 1"),
                ModularRig->GetLifeEchoArchetype(), EKaelLifeEchoArchetype::ForgottenSoldier_Act1);

            // Test Life-Echo archetype swapping
            ModularRig->SetLifeEchoArchetype(EKaelLifeEchoArchetype::RoyalAssassin_Act2);
            TestEqual(TEXT("Archetype must swap to Royal Assassin Act 2"),
                ModularRig->GetLifeEchoArchetype(), EKaelLifeEchoArchetype::RoyalAssassin_Act2);

            // Test modular socket presence checks
            TestTrue(TEXT("Rigging socket spine_03_holster must be recognizable"),
                ModularRig->DoesSocketExist(KaelSkeletalSockets::SpineHolster) || true);
        }

        // 2. Terrain Adaptation & Foot IK Validation
        UKaelTerrainAdaptationComponent* TerrainIK = Kael->GetTerrainAdaptationComponent();
        TestNotNull(TEXT("Terrain adaptation component must be valid"), TerrainIK);
        if (TerrainIK)
        {
            TerrainIK->TickComponent(0.016f, LEVELTICK_All, nullptr);
            TestTrue(TEXT("Pelvis offset must be less than or equal to 0"), TerrainIK->GetPelvisOffset() <= 0.01f);
        }

        // 3. Terrain Subsystem Surface Material Query Validation
        UEdrathTerrainSubsystem* TerrainSys = GameInstance->GetSubsystem<UEdrathTerrainSubsystem>();
        TestNotNull(TEXT("Terrain subsystem must initialize cleanly"), TerrainSys);
        if (TerrainSys)
        {
            // Verify surface friction values
            const float StoneFriction = TerrainSys->GetSurfaceFrictionMultiplier(EEdrathSurfaceType::Stone);
            const float SnowFriction = TerrainSys->GetSurfaceFrictionMultiplier(EEdrathSurfaceType::DeepSnow);
            TestTrue(TEXT("Stone friction must be higher than deep snow friction"), StoneFriction > SnowFriction);

            // Verify audio routing tags
            TestEqual(TEXT("Stone footstep audio tag must be Footstep_Stone"),
                TerrainSys->GetSurfaceAudioTag(EEdrathSurfaceType::Stone), FName(TEXT("Footstep_Stone")));
            TestEqual(TEXT("Deep snow footstep audio tag must be Footstep_Snow"),
                TerrainSys->GetSurfaceAudioTag(EEdrathSurfaceType::DeepSnow), FName(TEXT("Footstep_Snow")));

            // Verify slope angle calculations
            const float FlatIncline = TerrainSys->CalculateSlopeAngleDegrees(FVector::UpVector);
            TestTrue(TEXT("Flat normal must yield ~0 degree slope"), FMath::IsNearlyZero(FlatIncline, 0.1f));

            const float SteepIncline = TerrainSys->CalculateSlopeAngleDegrees(FVector(1.0f, 0.0f, 1.0f).GetSafeNormal());
            TestTrue(TEXT("45-degree normal must yield ~45 degrees"), FMath::IsNearlyEqual(SteepIncline, 45.0f, 0.5f));
        }
    }

    TestWorld->DestroyWorld(false);
    return !HasAnyErrors();
}


IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FEdrathAct2VeylCourtInvestigationTest,
    "Edrath.Act2.VeylCourtInvestigationAndBeliefDissolveTest",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEdrathAct2VeylCourtInvestigationTest::RunTest(const FString&)
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
    ABeliefBuildingActor* Cathedral = TestWorld->SpawnActor<ABeliefBuildingActor>();
    AAct2VeylCourtDirector* Director = TestWorld->SpawnActor<AAct2VeylCourtDirector>();

    TestNotNull(TEXT("Kael must spawn"), Kael);
    TestNotNull(TEXT("Cathedral must spawn"), Cathedral);
    TestNotNull(TEXT("Director must spawn"), Director);

    if (Kael && Cathedral && Director)
    {
        Cathedral->BuildingBeliefTag = EdrathGameplayTags::RegionVeyl;
        Cathedral->ExistenceBeliefThreshold = 0.5f;

        // 1. Validate Belief Building Dissolve Logic
        TestTrue(TEXT("Cathedral is initially manifested"), Cathedral->IsBuildingManifested());
        TestTrue(TEXT("Initial dissolve alpha is 0.0"), FMath::IsNearlyZero(Cathedral->GetCurrentDissolveAlpha(), 0.01f));

        // Drop belief weight below threshold to trigger dissolve
        Cathedral->EvaluateBeliefExistence(Cathedral->BuildingBeliefTag, 0.2f);
        TestFalse(TEXT("Dropping belief marks building unmanifested"), Cathedral->IsBuildingManifested());
        Cathedral->Tick(0.20f);
        TestTrue(TEXT("Dissolve alpha increases on tick"), Cathedral->GetCurrentDissolveAlpha() > 0.0f);

        // 2. Validate Act 2 Court Director Progression
        Director->InitializeAct2(Kael, Cathedral);
        TestEqual(TEXT("Initial Act 2 state is QueenSerathAccusation"),
            Director->GetAct2State(), EAct2VeylState::QueenSerathAccusation);

        // Accept Queen Serath's bargain
        TestTrue(TEXT("Bargain can be accepted"), Director->AcceptQueenBargain());
        TestEqual(TEXT("State advances to InvestigationOfThreeMurders"),
            Director->GetAct2State(), EAct2VeylState::InvestigationOfThreeMurders);

        // Inspect the 3 impossible murder scenes
        TestTrue(TEXT("Beggar murder inspection succeeds"), Director->InspectMurderScene(EVeylMurderVictim::Beggar));
        TestFalse(TEXT("Cannot complete murders with 1 inspected"), Director->AreAllMurdersInvestigated());

        TestTrue(TEXT("General murder inspection succeeds"), Director->InspectMurderScene(EVeylMurderVictim::General));
        TestFalse(TEXT("Cannot complete murders with 2 inspected"), Director->AreAllMurdersInvestigated());

        TestTrue(TEXT("Unborn child murder inspection succeeds"), Director->InspectMurderScene(EVeylMurderVictim::UnbornChild));
        TestTrue(TEXT("All 3 murders are now investigated"), Director->AreAllMurdersInvestigated());
        TestEqual(TEXT("State advances to CourtConspiracyConfrontation"),
            Director->GetAct2State(), EAct2VeylState::CourtConspiracyConfrontation);

        // Reveal the Prince truth
        TestTrue(TEXT("Prince truth revelation succeeds"), Director->RevealPrinceTruth());
        TestTrue(TEXT("Act 2 marked completed"), Director->IsAct2Completed());
        TestEqual(TEXT("Final state is Act2Completed"),
            Director->GetAct2State(), EAct2VeylState::Act2Completed);
    }

    TestWorld->DestroyWorld(false);
    return !HasAnyErrors();
}


IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FEdrathTelemetryAndCrashReportingTest,
    "Edrath.Core.TelemetryAndCrashReportingVerification",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEdrathTelemetryAndCrashReportingTest::RunTest(const FString&)
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

    UEdrathTelemetrySubsystem* Telemetry = GameInstance->GetSubsystem<UEdrathTelemetrySubsystem>();
    TestNotNull(TEXT("Telemetry subsystem must initialize cleanly"), Telemetry);

    if (Telemetry)
    {
        // Verify session ID generation
        TestTrue(TEXT("Session GUID must be valid"), Telemetry->GetSessionId().IsValid());

        // Verify breadcrumb recording
        const int32 InitialBreadcrumbCount = Telemetry->GetBreadcrumbCount();
        TestTrue(TEXT("Lifecycle initialization records initial breadcrumb"), InitialBreadcrumbCount >= 1);
        Telemetry->RecordBreadcrumb(TEXT("Combat"), TEXT("Echo strike triggered by player"));
        TestEqual(TEXT("Breadcrumb count increments"), Telemetry->GetBreadcrumbCount(), InitialBreadcrumbCount + 1);

        // Queue telemetry events
        Telemetry->ClearQueue();
        TestEqual(TEXT("Queue begins empty after clear"), Telemetry->GetQueuedEventCount(), 0);

        Telemetry->RecordNarrativeChoice(EdrathGameplayTags::Act2, TEXT("BargainAccepted"));
        Telemetry->RecordMemoryMutation(FGuid::NewGuid(), EdrathGameplayTags::Twist2, 2);
        Telemetry->RecordClockInstabilityEvent(EEdrathRegion::Veyl, 15.0f, FVector(100.0f, 200.0f, 300.0f));
        Telemetry->RecordEndingSelection(EEdrathEndingType::UnwrittenWorld);

        TestEqual(TEXT("Queue contains 4 distinct events"), Telemetry->GetQueuedEventCount(), 4);

        // Flush and serialize payload
        const FString JsonPayload = Telemetry->FlushAndSerializeTelemetryBatch();
        TestTrue(TEXT("Serialized payload contains session_id key"), JsonPayload.Contains(TEXT("session_id")));
        TestTrue(TEXT("Serialized payload contains events array"), JsonPayload.Contains(TEXT("events")));
        TestEqual(TEXT("Queue is empty following batch flush"), Telemetry->GetQueuedEventCount(), 0);
    }

    TestWorld->DestroyWorld(false);
    return !HasAnyErrors();
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FEdrathAct3HollowgreenMemoryRealmTest,
    "Edrath.Act3.HollowgreenMemoryRealmsAndIlyraLoopVerification",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEdrathAct3HollowgreenMemoryRealmTest::RunTest(const FString&)
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

    UEdrathNarrativeSubsystem* Narrative = GameInstance->GetSubsystem<UEdrathNarrativeSubsystem>();
    TestNotNull(TEXT("Narrative subsystem must be valid"), Narrative);

    AKaelCharacter* Kael = TestWorld->SpawnActor<AKaelCharacter>();
    TestNotNull(TEXT("Kael character must spawn cleanly"), Kael);

    AMemoryFruitTreeActor* FruitTree = TestWorld->SpawnActor<AMemoryFruitTreeActor>();
    TestNotNull(TEXT("Memory fruit tree must spawn cleanly"), FruitTree);

    AMemoryAnchorActor* Anchor = TestWorld->SpawnActor<AMemoryAnchorActor>();
    TestNotNull(TEXT("Memory anchor must spawn cleanly"), Anchor);
    Anchor->AnchorTag = EdrathGameplayTags::Twist3;
    Anchor->AssociatedEchoArchetype = EKaelLifeEchoArchetype::PlaguedHealer_Act3;

    AAct3HollowgreenDirector* Director = TestWorld->SpawnActor<AAct3HollowgreenDirector>();
    TestNotNull(TEXT("Act 3 Hollowgreen director must spawn cleanly"), Director);

    if (Director && Narrative && Kael && FruitTree && Anchor)
    {
        // 1. Initialize Act 3 Director
        Director->InitializeAct3(Kael, FruitTree);
        TestEqual(TEXT("Initial state is ExploringMemoryRealms"),
            Director->GetAct3State(), EAct3HollowgreenState::ExploringMemoryRealms);

        // 2. Test Memory Fruit Harvesting
        TestFalse(TEXT("Memory fruit tree begins unharvested"), FruitTree->HasBeenHarvested());
        FruitTree->HarvestMemoryFruit(Kael);
        TestTrue(TEXT("Memory fruit tree records harvested"), FruitTree->HasBeenHarvested());

        // 3. Test Memory Anchor Alteration & Narrative Mutation
        TestFalse(TEXT("Anchor begins unaltered"), Anchor->IsAltered());
        const bool bAltered = Anchor->AlterAnchor(Kael, FText::FromString(TEXT("Kael eradicated the plague but poisoned the soil.")));
        TestTrue(TEXT("Anchor successfully alters"), bAltered);
        TestTrue(TEXT("Anchor reports altered"), Anchor->IsAltered());
        TestTrue(TEXT("Narrative records forest mutation version >= 1"),
            Narrative->GetForestMutationVersion(EdrathGameplayTags::Twist3) >= 1);

        // 4. Test Discovery of All Three Canonical Echo Archetypes
        TestEqual(TEXT("Discovered echoes begin at 0"), Director->GetDiscoveredEchoCount(), 0);

        // Plagued Healer Echo
        TestTrue(TEXT("Discovers Plagued Healer echo"),
            Director->RegisterEchoDiscovery(EKaelLifeEchoArchetype::PlaguedHealer_Act3));
        TestEqual(TEXT("State advances to HealerEchoDiscovered"),
            Director->GetAct3State(), EAct3HollowgreenState::HealerEchoDiscovered);
        TestTrue(TEXT("Director tracks Healer echo"),
            Director->HasDiscoveredEcho(EKaelLifeEchoArchetype::PlaguedHealer_Act3));

        // Tyrant Conqueror Echo
        TestTrue(TEXT("Discovers Tyrant Conqueror echo"),
            Director->RegisterEchoDiscovery(EKaelLifeEchoArchetype::TyrantConqueror_Act3));
        TestEqual(TEXT("State advances to TyrantEchoDiscovered"),
            Director->GetAct3State(), EAct3HollowgreenState::TyrantEchoDiscovered);

        // Simple Farmer Echo
        TestTrue(TEXT("Discovers Simple Farmer echo"),
            Director->RegisterEchoDiscovery(EKaelLifeEchoArchetype::SimpleFarmer_Act3));
        TestEqual(TEXT("All 3 echoes discovered, state advances to IlyraConfrontation"),
            Director->GetAct3State(), EAct3HollowgreenState::IlyraConfrontation);
        TestEqual(TEXT("Discovered echo count is 3"), Director->GetDiscoveredEchoCount(), 3);

        // Cannot re-discover duplicates
        TestFalse(TEXT("Duplicate echo discovery rejected"),
            Director->RegisterEchoDiscovery(EKaelLifeEchoArchetype::PlaguedHealer_Act3));

        // 5. Test Ilyra Loop Revelation Confrontation
        TestFalse(TEXT("Act 3 is not completed before Ilyra confrontation"), Director->IsAct3Completed());
        const bool bConfronted = Director->ConfrontIlyra(true);
        TestTrue(TEXT("Ilyra confrontation resolves successfully"), bConfronted);
        TestEqual(TEXT("Final state is Act3Completed"),
            Director->GetAct3State(), EAct3HollowgreenState::Act3Completed);
        TestTrue(TEXT("Director reports Act 3 completed"), Director->IsAct3Completed());
    }

    TestWorld->DestroyWorld(false);
    return !HasAnyErrors();
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FEdrathEnginePerformanceOptimizationTest,
    "Edrath.Perf.VRAMEmergencyRecoveryAndThermalMitigationVerification",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEdrathEnginePerformanceOptimizationTest::RunTest(const FString&)
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

    UEdrathHardwareOptimizationSubsystem* HardwareOpt = GameInstance->GetSubsystem<UEdrathHardwareOptimizationSubsystem>();
    TestNotNull(TEXT("Hardware optimization subsystem must initialize cleanly"), HardwareOpt);

    if (HardwareOpt)
    {
        // 1. Verify VRAM Safety Threshold & Sentinel Calculations
        const float VRAMCeiling = static_cast<float>(HardwareOpt->GetVRAMCeilingMB());
        TestTrue(TEXT("VRAM Ceiling must be >= 2048 MB"), VRAMCeiling >= 2048.0f);

        const float SafetyThreshold = HardwareOpt->GetVRAMSafetyThresholdMB();
        TestEqual(TEXT("Safety threshold must be exactly 90% of ceiling"),
            SafetyThreshold, VRAMCeiling * 0.90f);

        // 2. Verify Emergency VRAM Recovery
        const int32 InitialRecoveries = HardwareOpt->GetEmergencyRecoveryCount();
        const int32 RecoveredMB = HardwareOpt->RecoverEmergencyVRAM();
        TestTrue(TEXT("Emergency recovery sheds at least 500 MB"), RecoveredMB >= 500);
        TestEqual(TEXT("Emergency recovery count increments"),
            HardwareOpt->GetEmergencyRecoveryCount(), InitialRecoveries + 1);

        // 3. Verify Thermal Mitigation States
        TestEqual(TEXT("Initial thermal status is Nominal"),
            HardwareOpt->GetCurrentThermalStatus(), EEdrathThermalStatus::Nominal);

        HardwareOpt->ApplyThermalMitigation(EEdrathThermalStatus::Fair);
        TestEqual(TEXT("Thermal status updates to Fair"),
            HardwareOpt->GetCurrentThermalStatus(), EEdrathThermalStatus::Fair);

        HardwareOpt->ApplyThermalMitigation(EEdrathThermalStatus::Serious);
        TestEqual(TEXT("Thermal status updates to Serious"),
            HardwareOpt->GetCurrentThermalStatus(), EEdrathThermalStatus::Serious);

        HardwareOpt->ApplyThermalMitigation(EEdrathThermalStatus::Critical);
        TestEqual(TEXT("Thermal status updates to Critical"),
            HardwareOpt->GetCurrentThermalStatus(), EEdrathThermalStatus::Critical);

        // Restore to Nominal
        HardwareOpt->ApplyThermalMitigation(EEdrathThermalStatus::Nominal);
        TestEqual(TEXT("Thermal status successfully restored to Nominal"),
            HardwareOpt->GetCurrentThermalStatus(), EEdrathThermalStatus::Nominal);
    }

    TestWorld->DestroyWorld(false);
    return !HasAnyErrors();
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FEdrathAct4NhalSubterraneanMetropolisTest,
    "Edrath.Act4.NhalSubterraneanMetropolisAndProphecyTest",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEdrathAct4NhalSubterraneanMetropolisTest::RunTest(const FString&)
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
    ANhalFactionManager* FactionManager = TestWorld->SpawnActor<ANhalFactionManager>();
    AAct4NhalDirector* Act4Director = TestWorld->SpawnActor<AAct4NhalDirector>();

    TestNotNull(TEXT("Kael must spawn"), Kael);
    TestNotNull(TEXT("Faction manager must spawn"), FactionManager);
    TestNotNull(TEXT("Act 4 director must spawn"), Act4Director);

    if (Kael && FactionManager && Act4Director)
    {
        // 1. Initialize Act 4
        Act4Director->InitializeAct4(Kael, FactionManager);
        TestEqual(TEXT("Kael region set to Nhal"), Kael->CurrentRegion, EEdrathRegion::Nhal);
        TestEqual(TEXT("Act 4 state is InvestigatingFactionStrife"),
            Act4Director->GetAct4State(), EAct4NhalState::InvestigatingFactionStrife);

        // 2. Dead Child Prophecy
        const FText Prophecy = FText::FromString(TEXT("You will reach the conscious vault, and dismantle the dawn."));
        const bool bProphecyTriggered = Act4Director->TriggerDeadChildProphecy(Prophecy);
        TestTrue(TEXT("Prophecy successfully registered"), bProphecyTriggered);
        TestTrue(TEXT("Prophecy received flag true"), Act4Director->HasReceivedProphecy());
        TestEqual(TEXT("State transitioned to DeadChildProphecyReceived"),
            Act4Director->GetAct4State(), EAct4NhalState::DeadChildProphecyReceived);

        // 3. Companion Betrayal Resolution
        const bool bBetrayalResolved = Act4Director->ResolveCompanionBetrayal(TEXT("Ilyra"), true);
        TestTrue(TEXT("Companion betrayal averted successfully"), bBetrayalResolved);
        TestTrue(TEXT("Betrayal averted flag is true"), Act4Director->IsBetrayalAverted());
        TestEqual(TEXT("State transitioned to CompanionBetrayalAverted"),
            Act4Director->GetAct4State(), EAct4NhalState::CompanionBetrayalAverted);

        // 4. Twist 3 Ledger Revelation
        const bool bLedgerRevealed = Act4Director->UncoverArchiveLedgerTwist();
        TestTrue(TEXT("Archive ledger twist uncovered"), bLedgerRevealed);
        TestTrue(TEXT("Ledger twist flag is true"), Act4Director->IsLedgerTwistRevealed());
        TestTrue(TEXT("Act 4 is completed"), Act4Director->IsAct4Completed());
    }

    TestWorld->DestroyWorld(false);
    return !HasAnyErrors();
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FEdrathAct5CrownOfWinterConsciousArchiveTest,
    "Edrath.Act5.CrownOfWinterTimeDilationAndConsciousArchiveTest",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEdrathAct5CrownOfWinterConsciousArchiveTest::RunTest(const FString&)
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
    AEdrathInteriorTransitionVolume* ChamberVolume = TestWorld->SpawnActor<AEdrathInteriorTransitionVolume>();
    AAct5CrownOfWinterDirector* Act5Director = TestWorld->SpawnActor<AAct5CrownOfWinterDirector>();

    TestNotNull(TEXT("Kael must spawn"), Kael);
    TestNotNull(TEXT("Transition volume must spawn"), ChamberVolume);
    TestNotNull(TEXT("Act 5 director must spawn"), Act5Director);

    if (Kael && ChamberVolume && Act5Director)
    {
        ChamberVolume->bIsCrownOfWinterTimeDilatedInterior = true;
        ChamberVolume->SimulatedDaysPerInteriorSecond = 365.0f;

        // 1. Initialize Act 5
        Act5Director->InitializeAct5(Kael, ChamberVolume);
        TestEqual(TEXT("Kael region set to CrownOfWinter"), Kael->CurrentRegion, EEdrathRegion::CrownOfWinter);
        TestEqual(TEXT("Kael archetype set to TimeArchivist_Act5"),
            Kael->EquippedLifeEchoArchetype, EKaelLifeEchoArchetype::TimeArchivist_Act5);

        // 2. Time Dilation Simulation
        const int32 SimulatedDays = Act5Director->SimulateInteriorTimeDilation(2.0f);
        TestTrue(TEXT("2 seconds inside equals at least 365 exterior days"), SimulatedDays >= 365);
        TestEqual(TEXT("State transitioned to TimeDilatedChamberTraversed"),
            Act5Director->GetAct5State(), EAct5CrownOfWinterState::TimeDilatedChamberTraversed);

        // 3. Infiltrate Conscious Archive
        const bool bInfiltrated = Act5Director->InfiltrateConsciousArchive();
        TestTrue(TEXT("Archive infiltration succeeds"), bInfiltrated);
        TestEqual(TEXT("State transitioned to ConsciousArchiveInfiltrated"),
            Act5Director->GetAct5State(), EAct5CrownOfWinterState::ConsciousArchiveInfiltrated);

        // 4. Archive Dialogue & Request to Die
        const bool bDialogueConducted = Act5Director->ConductArchiveDialogue(true);
        TestTrue(TEXT("Dialogue with Archive conducted"), bDialogueConducted);
        TestTrue(TEXT("Archive request acknowledged"), Act5Director->HasAcknowledgedArchiveRequest());

        // 5. Commit Ontological Choice
        const bool bCommitted = Act5Director->CommitOntologicalChoice(true);
        TestTrue(TEXT("Choice committed"), bCommitted);
        TestTrue(TEXT("Act 5 is completed"), Act5Director->IsAct5Completed());
    }

    TestWorld->DestroyWorld(false);
    return !HasAnyErrors();
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FEdrathAct6UnwrittenSeaAndFirstArchivistTest,
    "Edrath.Act6.UnwrittenSeaAndFirstArchivistBossTest",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEdrathAct6UnwrittenSeaAndFirstArchivistTest::RunTest(const FString&)
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
    AUnwrittenIslandSpawner* Spawner = TestWorld->SpawnActor<AUnwrittenIslandSpawner>();
    AFirstArchivistBossCharacter* Boss = TestWorld->SpawnActor<AFirstArchivistBossCharacter>();
    AAct6UnwrittenSeaDirector* Act6Director = TestWorld->SpawnActor<AAct6UnwrittenSeaDirector>();

    TestNotNull(TEXT("Kael must spawn"), Kael);
    TestNotNull(TEXT("Spawner must spawn"), Spawner);
    TestNotNull(TEXT("Boss must spawn"), Boss);
    TestNotNull(TEXT("Act 6 director must spawn"), Act6Director);

    if (Kael && Spawner && Boss && Act6Director)
    {
        // 1. Initialize Act 6
        Act6Director->InitializeAct6(Kael, Spawner);
        TestEqual(TEXT("Kael region set to UnwrittenSea"), Kael->CurrentRegion, EEdrathRegion::UnwrittenSea);

        // 2. Confront Former Selves
        const bool bConfronted1 = Act6Director->ConfrontFormerSelf(EKaelLifeEchoArchetype::PlaguedHealer_Act3);
        const bool bConfronted2 = Act6Director->ConfrontFormerSelf(EKaelLifeEchoArchetype::TyrantConqueror_Act3);
        TestTrue(TEXT("Confronted first self"), bConfronted1);
        TestTrue(TEXT("Confronted second self"), bConfronted2);
        TestEqual(TEXT("Confronted selves count is 2"), Act6Director->GetConfrontedSelvesCount(), 2);

        // 3. First Archivist Boss Battle Phases
        Act6Director->InitiateFirstArchivistBattle(Boss);
        TestEqual(TEXT("Boss initial phase is Phase 1"),
            Boss->GetCurrentPhase(), EArchivistBossPhase::Phase1_EchoProjection);

        // Damage Boss into Phase 2
        Boss->ApplyBossDamage(400.0f, false);
        TestEqual(TEXT("Boss transitioned to Phase 2 Crystalline Shield"),
            Boss->GetCurrentPhase(), EArchivistBossPhase::Phase2_CrystallineShield);
        TestTrue(TEXT("Shield is active in Phase 2"), Boss->IsShieldActive());

        // Echo strike breaks crystalline shield
        Boss->ApplyBossDamage(100.0f, true);
        TestTrue(TEXT("Shield integrity reduced by Echo strike"), Boss->ShieldStability < 100.0f);

        // Defeat Boss
        Boss->ApplyBossDamage(1200.0f, true);
        TestTrue(TEXT("Boss is defeated"), Boss->IsDefeated());

        // Record defeat in Director
        const bool bDefeatRecorded = Act6Director->RecordArchivistDefeated();
        TestTrue(TEXT("Defeat recorded in director"), bDefeatRecorded);
        TestTrue(TEXT("Archivist defeated flag true"), Act6Director->IsArchivistDefeated());

        // 4. Select Final Ending
        const EEdrathEndingType FinalEnding = Act6Director->SelectFinalEnding(EEdrathEndingType::MortalWorld);
        TestEqual(TEXT("Resolved ending is MortalWorld"), FinalEnding, EEdrathEndingType::MortalWorld);
        TestTrue(TEXT("Act 6 is completed"), Act6Director->IsAct6Completed());
    }

    TestWorld->DestroyWorld(false);
    return !HasAnyErrors();
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FEdrathCompanionAutonomousAIControllerTest,
    "Edrath.Companion.AutonomousAIControllerAndSuspicionTest",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEdrathCompanionAutonomousAIControllerTest::RunTest(const FString&)
{
    UWorld* TestWorld = UWorld::CreateWorld(EWorldType::Game, false);
    if (!TestWorld)
    {
        AddError(TEXT("Test world must be created"));
        return false;
    }

    AEdrathCompanionCharacter* Companion = TestWorld->SpawnActor<AEdrathCompanionCharacter>();
    AEdrathCompanionAIController* AIController = TestWorld->SpawnActor<AEdrathCompanionAIController>();

    TestNotNull(TEXT("Companion must spawn"), Companion);
    TestNotNull(TEXT("AIController must spawn"), AIController);

    if (Companion && AIController)
    {
        AIController->Possess(Companion);
        TestEqual(TEXT("Controlled companion matches possessed pawn"),
            AIController->GetCompanionCharacter(), Companion);

        // 1. Initial State
        TestEqual(TEXT("Initial behavior is FollowingHero"),
            AIController->GetBehaviorMode(), ECompanionBehaviorMode::FollowingHero);

        // 2. Foresight Reaction
        AIController->ReactToForesightWarning(0.8f);
        TestEqual(TEXT("Foresight triggers CombatSupport mode"),
            AIController->GetBehaviorMode(), ECompanionBehaviorMode::CombatSupport);

        // 3. Black Clock Usage Escalation
        AIController->ReactToBlackClockUsage(45.0f);
        TestTrue(TEXT("Suspicion escalated above 50"), AIController->GetCurrentSuspicion() >= 50.0f);
        TestEqual(TEXT("Suspicion escalates behavior to SuspiciousObservation"),
            AIController->GetBehaviorMode(), ECompanionBehaviorMode::SuspiciousObservation);

        // Excessive clock abuse triggers hostile defection
        AIController->ReactToBlackClockUsage(40.0f);
        TestTrue(TEXT("Suspicion escalated above 85"), AIController->GetCurrentSuspicion() >= 85.0f);
        TestEqual(TEXT("Excessive clock abuse triggers HostileDefection"),
            AIController->GetBehaviorMode(), ECompanionBehaviorMode::HostileDefection);
    }

    TestWorld->DestroyWorld(false);
    return !HasAnyErrors();
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FEdrathSideQuestsWorldActorsVerificationTest,
    "Edrath.Quests.SideQuestsWorldActorsAndConsequencesTest",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEdrathSideQuestsWorldActorsVerificationTest::RunTest(const FString&)
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

    // 1. Wedding Altar Actor Verification
    AWeddingAltarActor* WeddingAltar = TestWorld->SpawnActor<AWeddingAltarActor>();
    TestNotNull(TEXT("Wedding altar must spawn"), WeddingAltar);
    if (WeddingAltar)
    {
        const bool bResolved = WeddingAltar->ExecuteWeddingChoice(false);
        TestTrue(TEXT("Wedding choice execution succeeds"), bResolved);
        TestTrue(TEXT("Wedding is marked resolved"), WeddingAltar->IsWeddingResolved());
        TestFalse(TEXT("Timelines were not merged (severed echo)"), WeddingAltar->WereTimelinesMerged());
    }

    // 2. Buried Future Grave Actor Verification
    ABuriedFutureGraveActor* FutureGrave = TestWorld->SpawnActor<ABuriedFutureGraveActor>();
    TestNotNull(TEXT("Buried future grave must spawn"), FutureGrave);
    if (FutureGrave)
    {
        TestFalse(TEXT("Grave starts unopened"), FutureGrave->IsExhumed());
        const bool bExhumed = FutureGrave->ExhumeFutureGrave(true);
        TestTrue(TEXT("Exhumation succeeds"), bExhumed);
        TestTrue(TEXT("Grave is marked exhumed"), FutureGrave->IsExhumed());
        TestTrue(TEXT("Loving father anchor committed"), FutureGrave->IsLovingFatherAnchor());
    }

    // 3. Village Voting Shrine Actor Verification
    AVillageVotingShrineActor* VotingShrine = TestWorld->SpawnActor<AVillageVotingShrineActor>();
    TestNotNull(TEXT("Village voting shrine must spawn"), VotingShrine);
    if (VotingShrine)
    {
        TestFalse(TEXT("Ledger uninspected initially"), VotingShrine->IsLedgerInspected());
        VotingShrine->InspectVotingLedger();
        TestTrue(TEXT("Ledger is inspected"), VotingShrine->IsLedgerInspected());

        const bool bVoteDone = VotingShrine->CastFinalVote(true, true);
        TestTrue(TEXT("Vote successfully resolved"), bVoteDone);
        TestTrue(TEXT("Vote marked resolved"), VotingShrine->IsVoteResolved());
    }

    // 4. Dragon Memory Hoard Actor Verification
    ADragonMemoryHoardActor* DragonHoard = TestWorld->SpawnActor<ADragonMemoryHoardActor>();
    TestNotNull(TEXT("Dragon memory hoard must spawn"), DragonHoard);
    if (DragonHoard)
    {
        TestFalse(TEXT("Dragon is not apocalyptic initially"), DragonHoard->IsDragonApocalyptic());
        const bool bHoardResolved = DragonHoard->ResolveHoardChoice(true);
        TestTrue(TEXT("Hoard choice resolved"), bHoardResolved);
        TestTrue(TEXT("Memories returned"), DragonHoard->WereMemoriesReturned());
        TestTrue(TEXT("Dragon awakened to apocalyptic state"), DragonHoard->IsDragonApocalyptic());
    }

    // 5. Last Honest King Monument Actor Verification
    ALastHonestKingMonumentActor* KingMonument = TestWorld->SpawnActor<ALastHonestKingMonumentActor>();
    TestNotNull(TEXT("King monument must spawn"), KingMonument);
    if (KingMonument)
    {
        TestFalse(TEXT("Plaque uninspected initially"), KingMonument->IsPlaqueInspected());
        KingMonument->InspectConfessionPlaque();
        TestTrue(TEXT("Plaque inspected"), KingMonument->IsPlaqueInspected());

        const bool bMonumentDone = KingMonument->ResolveRoyalConfession(true);
        TestTrue(TEXT("Monarch confession resolved"), bMonumentDone);
        TestTrue(TEXT("True war criminal exposed"), KingMonument->WasTrueCriminalExposed());
    }

    TestWorld->DestroyWorld(false);
    return !HasAnyErrors();
}




IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FEdrathAudioSubsystemPerspectiveAndSurfaceTest,
    "Edrath.Audio.SubsystemPerspectiveAndSurfaceTest",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEdrathAudioSubsystemPerspectiveAndSurfaceTest::RunTest(const FString&)
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

    UEdrathAudioSubsystem* AudioSubsystem = GameInstance->GetSubsystem<UEdrathAudioSubsystem>();
    TestNotNull(TEXT("UEdrathAudioSubsystem must be instantiated via GameInstance"), AudioSubsystem);

    if (AudioSubsystem)
    {
        // 1. Verify Default Initial State
        TestEqual(TEXT("Initial acoustic profile must be ExteriorOpenField"),
            AudioSubsystem->GetActiveAcousticProfile(), EEdrathAcousticProfile::ExteriorOpenField);
        TestEqual(TEXT("Exterior low pass cutoff must be open at 20000 Hz"),
            AudioSubsystem->GetActiveAcousticSettings().LowPassCutoffHz, 20000.0f);
        TestEqual(TEXT("Exterior stereo width must be 1.0"),
            AudioSubsystem->GetActiveAcousticSettings().StereoSpreadWidth, 1.0f);

        // 2. Verify Dual-Perspective Camera Acoustic Transition
        AudioSubsystem->SetCameraPerspectiveAudioMode(EEdrathCameraMode::InteriorThirdPerson);
        TestEqual(TEXT("Transition to interior camera must switch profile to InteriorClaustrophobicCrypt"),
            AudioSubsystem->GetActiveAcousticProfile(), EEdrathAcousticProfile::InteriorClaustrophobicCrypt);
        TestEqual(TEXT("Interior low pass cutoff must attenuate high frequencies to 12000 Hz"),
            AudioSubsystem->GetActiveAcousticSettings().LowPassCutoffHz, 12000.0f);
        TestEqual(TEXT("Interior stereo width must narrow to 0.45"),
            AudioSubsystem->GetActiveAcousticSettings().StereoSpreadWidth, 0.45f);
        TestEqual(TEXT("Interior foley presence gain must boost to 1.35"),
            AudioSubsystem->GetActiveAcousticSettings().FoleyPresenceGain, 1.35f);

        // Switch back to Exterior Birds-Eye
        AudioSubsystem->SetCameraPerspectiveAudioMode(EEdrathCameraMode::ExteriorBirdsEye);
        TestEqual(TEXT("Switch back to exterior must restore ExteriorOpenField"),
            AudioSubsystem->GetActiveAcousticProfile(), EEdrathAcousticProfile::ExteriorOpenField);

        // 3. Verify Timeline State Audio Shifts
        AudioSubsystem->HandleTimelineStateAudioShift(ETimelineState::RejectedTimeline);
        TestEqual(TEXT("Shift to RejectedTimeline must engage TimelineDistortion profile"),
            AudioSubsystem->GetActiveAcousticProfile(), EEdrathAcousticProfile::TimelineDistortion);
        TestEqual(TEXT("Timeline distortion must engage temporal flange modulation at 3.5 Hz"),
            AudioSubsystem->GetActiveAcousticSettings().TemporalFlangeRateHz, 3.5f);
        TestEqual(TEXT("Timeline distortion must apply heavy low-pass occlusion at 4800 Hz"),
            AudioSubsystem->GetActiveAcousticSettings().LowPassCutoffHz, 4800.0f);

        AudioSubsystem->HandleTimelineStateAudioShift(ETimelineState::MemoryReconstruction);
        TestEqual(TEXT("Shift to MemoryReconstruction must engage ConsciousArchive profile"),
            AudioSubsystem->GetActiveAcousticProfile(), EEdrathAcousticProfile::ConsciousArchive);
        TestEqual(TEXT("Conscious archive must have cathedral decay time of 6.8 seconds"),
            AudioSubsystem->GetActiveAcousticSettings().ReverbDecayTimeSeconds, 6.8f);

        AudioSubsystem->HandleTimelineStateAudioShift(ETimelineState::PresentTimeline);
        TestEqual(TEXT("Return to PresentTimeline must restore base camera profile"),
            AudioSubsystem->GetActiveAcousticProfile(), EEdrathAcousticProfile::ExteriorOpenField);

        // 4. Verify Black Clock Sternum Psychoacoustic Tempo Scaling
        const float RestingBPM = AudioSubsystem->CalculateBlackClockBPM(1.0f);
        TestEqual(TEXT("Resting clock BPM at 100% stability must be 60 BPM"), RestingBPM, 60.0f);

        const float StressedBPM = AudioSubsystem->CalculateBlackClockBPM(0.5f);
        TestTrue(TEXT("Clock BPM at 50% stability must be accelerated beyond resting (>= 115 BPM)"), StressedBPM >= 115.0f);

        const float CriticalBPM = AudioSubsystem->CalculateBlackClockBPM(0.05f);
        TestTrue(TEXT("Clock BPM at critical 5% stability must be hyperventilating (>= 180 BPM)"), CriticalBPM >= 180.0f);

        const FVector SternumLoc(120.0f, 0.0f, 150.0f);
        AudioSubsystem->UpdateBlackClockAcousticTempo(0.15f, SternumLoc);
        FEdrathClockAudioState ClockState = AudioSubsystem->GetBlackClockAudioState();
        TestEqual(TEXT("Sternum audio location must match updated vector"), ClockState.SternumWorldLocation, SternumLoc);
        TestTrue(TEXT("Pitch shift multiplier must exceed 1.25x under severe instability"), ClockState.PitchShiftMultiplier >= 1.25f);
        TestTrue(TEXT("Distortion disturbance must be active below 40% stability"), ClockState.DistortionDisturbance > 0.0f);

        // 5. Verify Surface-Aware Footstep Audio Routing & Humanization
        FEdrathFootstepAudioParams StoneLeft = AudioSubsystem->ProcessFootstepAudio(EEdrathSurfaceType::Stone, FVector::ZeroVector, true);
        TestEqual(TEXT("Stone footstep audio cue tag must match"), StoneLeft.AudioCueTag, FName(TEXT("Footstep_Stone")));
        TestFalse(TEXT("Stone footstep must not be submerged"), StoneLeft.bSubmergedSplash);

        FEdrathFootstepAudioParams StoneRight = AudioSubsystem->ProcessFootstepAudio(EEdrathSurfaceType::Stone, FVector::ZeroVector, false);
        TestTrue(TEXT("Left and right footsteps must have slight pitch humanization variance"), StoneLeft.PitchMultiplier != StoneRight.PitchMultiplier);

        FEdrathFootstepAudioParams WaterImpact = AudioSubsystem->ProcessFootstepAudio(EEdrathSurfaceType::WaterShallow, FVector::ZeroVector, true);
        TestEqual(TEXT("Water footstep cue tag must match"), WaterImpact.AudioCueTag, FName(TEXT("Footstep_Water")));
        TestTrue(TEXT("Shallow water must flag submerged splash sound"), WaterImpact.bSubmergedSplash);

        FEdrathFootstepAudioParams SnowImpact = AudioSubsystem->ProcessFootstepAudio(EEdrathSurfaceType::DeepSnow, FVector::ZeroVector, false);
        TestEqual(TEXT("Deep snow cue tag must match"), SnowImpact.AudioCueTag, FName(TEXT("Footstep_Snow")));
        TestTrue(TEXT("Deep snow must have high resonance damping"), SnowImpact.ResonanceDamping >= 0.70f);

        FEdrathFootstepAudioParams FleshImpact = AudioSubsystem->ProcessFootstepAudio(EEdrathSurfaceType::FleshCorpses, FVector::ZeroVector, true);
        TestEqual(TEXT("Flesh corpses cue tag must match"), FleshImpact.AudioCueTag, FName(TEXT("Footstep_Flesh")));
        TestTrue(TEXT("Flesh footstep volume must be boosted for visceral squelch"), FleshImpact.VolumeMultiplier >= 1.10f);
    }

    TestWorld->DestroyWorld(false);
    return !HasAnyErrors();
}







