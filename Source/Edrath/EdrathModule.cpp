#include "EdrathModule.h"
#include "Misc/Paths.h"
#include "ShaderCore.h"

void FEdrathModule::StartupModule()
{
    const FString ShaderDirectory = FPaths::Combine(FPaths::ProjectDir(), TEXT("Shaders"));
    if (FPaths::DirectoryExists(ShaderDirectory))
    {
        AddShaderSourceDirectoryMapping(TEXT("/Project/Shaders"), ShaderDirectory);
    }
}

void FEdrathModule::ShutdownModule()
{
    ResetAllShaderSourceDirectoryMappings();
}

IMPLEMENT_PRIMARY_GAME_MODULE(FEdrathModule, Edrath, "Edrath");
