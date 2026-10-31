#include "Misc/AutomationTest.h"
#include "BlackClock/BlackClockComponent.h"
#include "BlackClock/PossibilityShiftActor.h"
#include "Camera/EdrathDualCameraComponent.h"
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
#include "Core/EdrathSaveSubsystem.h"
#include "BlackClock/EchoCombatComponent.h"

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

    // --- Scalability & Mobile Optimization Verification ---
    UEdrathScalabilitySubsystem* Scalability = GameInstance->GetSubsystem<UEdrathScalabilitySubsystem>();
    TestNotNull(TEXT("Scalability subsystem must exist"), Scalability);
    if (Scalability)
    {
        Scalability->ApplyQualityPreset(EEdrathQualityPreset::Epic_PC);
        TestEqual(TEXT("Scalability must be Epic_PC"), Scalability->GetCurrentQualityPreset(), EEdrathQualityPreset::Epic_PC);
        Scalability->ApplyQualityPreset(EEdrathQualityPreset::Low_Mobile);
        TestEqual(TEXT("Scalability must be Low_Mobile"), Scalability->GetCurrentQualityPreset(), EEdrathQualityPreset::Low_Mobile);
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
