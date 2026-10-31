#include "World/HouseWithOneMoreRoomActor.h"
#include "Engine/World.h"

AHouseWithOneMoreRoomActor::AHouseWithOneMoreRoomActor()
{
    PrimaryActorTick.bCanEverTick = false;
}

AActor* AHouseWithOneMoreRoomActor::OnOwnerRememberedSpawnNextRoom()
{
    const int32 RoomIndex = SpawnedRooms.Num();
    if (!GetWorld() || !ProceduralRoomClasses.IsValidIndex(RoomIndex) || !ProceduralRoomClasses[RoomIndex])
    {
        return nullptr;
    }
    const FVector SpawnLocation = GetActorLocation() + RoomExtensionOffset * static_cast<float>(RoomIndex + 1);
    AActor* Room = GetWorld()->SpawnActor<AActor>(ProceduralRoomClasses[RoomIndex], SpawnLocation, GetActorRotation());
    if (Room)
    {
        SpawnedRooms.Add(Room);
    }
    return Room;
}

bool AHouseWithOneMoreRoomActor::UnlockFinalAtrocityRoom()
{
    if (bFinalAtrocityRoomUnlocked)
    {
        return false;
    }
    bFinalAtrocityRoomUnlocked = true;
    return true;
}
