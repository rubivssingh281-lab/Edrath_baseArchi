#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Core/EdrathSaveGame.h"
#include "EdrathSaveSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSaveGameCompleted, bool, bSuccess, const FString&, SlotName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLoadGameCompleted, bool, bSuccess, const FString&, SlotName);

UCLASS()
class EDRATH_API UEdrathSaveSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintCallable, Category="Edrath|SaveSystem")
    bool SaveGameToSlot(const FString& SlotName = TEXT("Edrath_AutoSave"), int32 UserIndex = 0);

    UFUNCTION(BlueprintCallable, Category="Edrath|SaveSystem")
    bool LoadGameFromSlot(const FString& SlotName = TEXT("Edrath_AutoSave"), int32 UserIndex = 0);

    UFUNCTION(BlueprintPure, Category="Edrath|SaveSystem")
    bool DoesSaveSlotExist(const FString& SlotName = TEXT("Edrath_AutoSave"), int32 UserIndex = 0) const;

    UFUNCTION(BlueprintCallable, Category="Edrath|SaveSystem")
    bool DeleteSaveSlot(const FString& SlotName = TEXT("Edrath_AutoSave"), int32 UserIndex = 0);

    UFUNCTION(BlueprintPure, Category="Edrath|SaveSystem")
    UEdrathSaveGame* GetActiveSaveData() const { return ActiveSaveData; }

    UPROPERTY(BlueprintAssignable, Category="Edrath|SaveSystem")
    FOnSaveGameCompleted OnSaveGameCompleted;

    UPROPERTY(BlueprintAssignable, Category="Edrath|SaveSystem")
    FOnLoadGameCompleted OnLoadGameCompleted;

private:
    UPROPERTY()
    TObjectPtr<UEdrathSaveGame> ActiveSaveData;

    void PopulateSaveFromWorld(UEdrathSaveGame* SaveObject);
    void ApplySaveToWorld(const UEdrathSaveGame* SaveObject);
};
