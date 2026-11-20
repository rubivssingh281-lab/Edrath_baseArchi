#include "Core/EdrathTelemetrySubsystem.h"
#include "Engine/World.h"

void UEdrathTelemetrySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    CurrentSessionId = FGuid::NewGuid();
    RecordBreadcrumb(TEXT("Lifecycle"), TEXT("Telemetry subsystem initialized with new session."));
}

void UEdrathTelemetrySubsystem::Deinitialize()
{
    RecordBreadcrumb(TEXT("Lifecycle"), TEXT("Telemetry subsystem shutting down."));
    ClearQueue();
    Super::Deinitialize();
}

void UEdrathTelemetrySubsystem::RecordBreadcrumb(const FString& Category, const FString& Message)
{
    FEdrathCrashBreadcrumb Breadcrumb;
    Breadcrumb.Category = Category;
    Breadcrumb.Message = Message;
    Breadcrumb.TimestampSeconds = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;

    if (BreadcrumbRingBuffer.Num() >= MaxBreadcrumbCapacity)
    {
        BreadcrumbRingBuffer.RemoveAt(0);
    }
    BreadcrumbRingBuffer.Add(Breadcrumb);
}

void UEdrathTelemetrySubsystem::EnqueueEvent(ETelemetryEventType Type, const FString& Name, const FString& Payload)
{
    if (EventQueue.Num() >= MaxQueueCapacity)
    {
        // Drop oldest event to avoid memory growth under network disconnections
        EventQueue.RemoveAt(0);
    }

    FEdrathTelemetryEvent Event;
    Event.EventType = Type;
    Event.EventName = Name;
    Event.PayloadJson = Payload;
    Event.TimestampSeconds = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
    Event.SessionId = CurrentSessionId;

    EventQueue.Add(Event);
}

void UEdrathTelemetrySubsystem::RecordNarrativeChoice(FGameplayTag QuestTag, const FString& ChoiceKey)
{
    const FString Payload = FString::Printf(TEXT("{\"quest\":\"%s\",\"choice\":\"%s\"}"),
        *QuestTag.ToString(), *ChoiceKey);
    EnqueueEvent(ETelemetryEventType::NarrativeChoice, TEXT("NarrativeChoice"), Payload);
    RecordBreadcrumb(TEXT("Narrative"), FString::Printf(TEXT("Choice %s on %s"), *ChoiceKey, *QuestTag.ToString()));
}

void UEdrathTelemetrySubsystem::RecordMemoryMutation(FGuid NpcId, FGameplayTag MemoryTag, int32 MutationVersion)
{
    const FString Payload = FString::Printf(TEXT("{\"npc_id\":\"%s\",\"memory_tag\":\"%s\",\"version\":%d}"),
        *NpcId.ToString(), *MemoryTag.ToString(), MutationVersion);
    EnqueueEvent(ETelemetryEventType::MemoryMutation, TEXT("MemoryMutation"), Payload);
    RecordBreadcrumb(TEXT("Memory"), FString::Printf(TEXT("Mutated memory %s (v%d)"), *MemoryTag.ToString(), MutationVersion));
}

void UEdrathTelemetrySubsystem::RecordClockInstabilityEvent(EEdrathRegion Region, float InstabilityAmount, const FVector& Epicenter)
{
    const FString Payload = FString::Printf(TEXT("{\"region\":%d,\"amount\":%.2f,\"x\":%.1f,\"y\":%.1f,\"z\":%.1f}"),
        static_cast<uint8>(Region), InstabilityAmount, Epicenter.X, Epicenter.Y, Epicenter.Z);
    EnqueueEvent(ETelemetryEventType::ClockInstability, TEXT("ClockInstability"), Payload);
}

void UEdrathTelemetrySubsystem::RecordEndingSelection(EEdrathEndingType ChosenEnding)
{
    const FString Payload = FString::Printf(TEXT("{\"ending\":%d}"), static_cast<uint8>(ChosenEnding));
    EnqueueEvent(ETelemetryEventType::EndingReached, TEXT("EndingReached"), Payload);
    RecordBreadcrumb(TEXT("Ending"), FString::Printf(TEXT("Ending %d selected"), static_cast<uint8>(ChosenEnding)));
}

FString UEdrathTelemetrySubsystem::FlushAndSerializeTelemetryBatch()
{
    const int32 Count = EventQueue.Num();
    if (Count == 0)
    {
        return TEXT("{\"events\":[]}");
    }

    FString OutputJson = FString::Printf(TEXT("{\"session_id\":\"%s\",\"event_count\":%d,\"events\":["),
        *CurrentSessionId.ToString(), Count);

    for (int32 i = 0; i < Count; ++i)
    {
        const FEdrathTelemetryEvent& Ev = EventQueue[i];
        OutputJson += FString::Printf(TEXT("{\"type\":%d,\"name\":\"%s\",\"data\":%s,\"t\":%.2f}%s"),
            static_cast<uint8>(Ev.EventType),
            *Ev.EventName,
            *Ev.PayloadJson,
            Ev.TimestampSeconds,
            (i < Count - 1) ? TEXT(",") : TEXT(""));
    }

    OutputJson += TEXT("]}");

    ClearQueue();
    OnTelemetryBatchDispatched.Broadcast(Count);
    return OutputJson;
}

void UEdrathTelemetrySubsystem::ClearQueue()
{
    EventQueue.Empty();
}
