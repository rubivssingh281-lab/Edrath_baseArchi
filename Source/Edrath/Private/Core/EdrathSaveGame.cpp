#include "Core/EdrathSaveGame.h"

UEdrathSaveGame::UEdrathSaveGame()
{
    SaveTimestamp = FDateTime::UtcNow();
    RegionalInstabilityMap.Add(EEdrathRegion::Veyl, 0.0f);
    RegionalInstabilityMap.Add(EEdrathRegion::Hollowgreen, 0.0f);
    RegionalInstabilityMap.Add(EEdrathRegion::AshenExpanse, 0.0f);
    RegionalInstabilityMap.Add(EEdrathRegion::Nhal, 0.0f);
    RegionalInstabilityMap.Add(EEdrathRegion::CrownOfWinter, 0.0f);
    RegionalInstabilityMap.Add(EEdrathRegion::UnwrittenSea, 0.0f);
}
