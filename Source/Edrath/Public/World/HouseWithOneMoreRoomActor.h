#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HouseWithOneMoreRoomActor.generated.h"

UCLASS()
class EDRATH_API AHouseWithOneMoreRoomActor : public AActor
{
    GENERATED_BODY()

public:
    AHouseWithOneMoreRoomActor();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|SideQuests")
    TArray<TSubclassOf<AActor>> ProceduralRoomClasses;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edrath|SideQuests")
    FVector RoomExtensionOffset = FVector(800.0f, 0.0f, 0.0f);

    UFUNCTION(BlueprintCallable, Category="Edrath|SideQuests")
    AActor* OnOwnerRememberedSpawnNextRoom();

    UFUNCTION(BlueprintPure, Category="Edrath|SideQuests")
    int32 GetSpawnedRoomCount() const { return SpawnedRooms.Num(); }

private:
    UPROPERTY()
    TArray<TObjectPtr<AActor>> SpawnedRooms;
};
