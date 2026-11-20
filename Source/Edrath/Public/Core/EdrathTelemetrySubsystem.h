#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Core/EdrathTypes.h"
#include "GameplayTagContainer.h"
#include "EdrathTelemetrySubsystem.generated.h"

/**
 * Categorization for recorded telemetry events.
 */
UENUM(BlueprintType)
enum class ETelemetryEventType : uint8
{
    NarrativeChoice,
    MemoryMutation,
    ClockInstability,
    EndingReached,
    CrashBreadcrumb
};

/**
 * A single structured telemetry data payload record.
 */
USTRUCT(BlueprintType)
struct FEdrathTelemetryEvent
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category="Edrath|Telemetry")
    ETelemetryEventType EventType = ETelemetryEventType::NarrativeChoice;

    UPROPERTY(BlueprintReadOnly, Category="Edrath|Telemetry")
    FString EventName;

    UPROPERTY(BlueprintReadOnly, Category="Edrath|Telemetry")
    FString PayloadJson;

    UPROPERTY(BlueprintReadOnly, Category="Edrath|Telemetry")
    float TimestampSeconds = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category="Edrath|Telemetry")
    FGuid SessionId;
};

/**
 * Ring buffer breadcrumb record for native crash dump symbolication.
 */
USTRUCT(BlueprintType)
struct FEdrathCrashBreadcrumb
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category="Edrath|Crash")
    FString Category;

    UPROPERTY(BlueprintReadOnly, Category="Edrath|Crash")
    FString Message;

    UPROPERTY(BlueprintReadOnly, Category="Edrath|Crash")
    float TimestampSeconds = 0.0f;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTelemetryBatchDispatchedSignature, int32, EventsDispatchedCount);

/**
 * UEdrathTelemetrySubsystem
 * Asynchronous GameInstance subsystem that aggregates gameplay telemetry (narrative branches,
 * memory mutations, clock instability events, ending selections) and Sentry-compatible
 * crash breadcrumbs without blocking the main game thread.
 */
UCLASS()
class EDRATH_API UEdrathTelemetrySubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    /** Records a player narrative branching choice. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Telemetry")
    void RecordNarrativeChoice(FGameplayTag QuestTag, const FString& ChoiceKey);

    /** Records a witness or living NPC memory mutation. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Telemetry")
    void RecordMemoryMutation(FGuid NpcId, FGameplayTag MemoryTag, int32 MutationVersion);

    /** Records Black Clock temporal instability event at world coordinates for heatmap generation. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Telemetry")
    void RecordClockInstabilityEvent(EEdrathRegion Region, float InstabilityAmount, const FVector& Epicenter);

    /** Records the ultimate story ending selected by the player. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Telemetry")
    void RecordEndingSelection(EEdrathEndingType ChosenEnding);

    /** Records an in-memory breadcrumb log for Sentry crash diagnostics. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Telemetry|Crash")
    void RecordBreadcrumb(const FString& Category, const FString& Message);

    /** Flushes the event buffer into a serialized JSON payload string ready for async HTTP dispatch. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Telemetry")
    FString FlushAndSerializeTelemetryBatch();

    /** Clears all queued telemetry events. */
    UFUNCTION(BlueprintCallable, Category="Edrath|Telemetry")
    void ClearQueue();

    /** Returns count of pending queued telemetry events. */
    UFUNCTION(BlueprintPure, Category="Edrath|Telemetry")
    int32 GetQueuedEventCount() const { return EventQueue.Num(); }

    /** Returns count of recorded breadcrumbs in ring buffer. */
    UFUNCTION(BlueprintPure, Category="Edrath|Telemetry")
    int32 GetBreadcrumbCount() const { return BreadcrumbRingBuffer.Num(); }

    /** Returns the unique session GUID for current run. */
    UFUNCTION(BlueprintPure, Category="Edrath|Telemetry")
    FGuid GetSessionId() const { return CurrentSessionId; }

    UPROPERTY(BlueprintAssignable, Category="Edrath|Telemetry")
    FOnTelemetryBatchDispatchedSignature OnTelemetryBatchDispatched;

protected:
    UPROPERTY(EditDefaultsOnly, Category="Edrath|Telemetry|Config")
    int32 MaxQueueCapacity = 256;

    UPROPERTY(EditDefaultsOnly, Category="Edrath|Telemetry|Config")
    int32 MaxBreadcrumbCapacity = 64;

private:
    void EnqueueEvent(ETelemetryEventType Type, const FString& Name, const FString& Payload);

    TArray<FEdrathTelemetryEvent> EventQueue;
    TArray<FEdrathCrashBreadcrumb> BreadcrumbRingBuffer;
    FGuid CurrentSessionId;
};
