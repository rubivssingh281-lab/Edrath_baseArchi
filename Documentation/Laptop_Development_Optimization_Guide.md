# Edrath — Laptop Development Optimization Guide

## Target Hardware Specification

| Component | Specification | Role in Engine |
|-----------|--------------|----------------|
| **CPU** | AMD Ryzen 7 253x (8C/16T) | Shader compilation, game logic, physics, AI |
| **RAM** | 16 GB DDR5 | Editor memory, shader compiler, asset streaming |
| **GPU** | NVIDIA RTX 5070 8GB GDDR7 | Rendering, Nanite, Lumen, VSM, TSR |
| **Storage** | 512 GB Gen4 NVMe M.2 | DDC, asset loading, shader caching |

---

## VRAM Budget Architecture (8192 MB Total)

The optimization system reserves 512 MB for GPU driver and OS compositor overhead, leaving a **7680 MB usable ceiling**. The budget is allocated across rendering subsystems to push visual quality to the hardware limit without page thrashing:

```
┌─────────────────────────────────────────────────────────┐
│                    RTX 5070 — 8192 MB                   │
├──────────────────────────────────┬──────────────────────┤
│  Texture Streaming Pool         │           2560 MB     │
│  Lumen Software Tracing         │         ~1200 MB      │
│  Nanite Geometry Streaming      │           1024 MB     │
│  Virtual Shadow Map Pages       │           ~768 MB     │
│  Framebuffer + TSR History      │           ~500 MB     │
│  Shader / PSO Cache             │           ~500 MB     │
│  Driver + OS Compositor         │           ~256 MB     │
├──────────────────────────────────┼──────────────────────┤
│  Safety Headroom                │           ~384 MB     │
└──────────────────────────────────┴──────────────────────┘
```

### Why Software Lumen Instead of Hardware Ray Tracing?

Hardware RT on the RTX 5070 adds ~2 GB VRAM overhead for BVH acceleration structures. Disabling HWRT frees this VRAM, allowing us to:

- **Double Lumen probe density** (DownsampleFactor 16 → 8) for sharper indirect lighting
- **Full-resolution Lumen reflections** (DownsampleFactor 2 → 1)
- **1 GB Nanite streaming pool** (vs 768 MB) for maximum geometric detail
- **2048 VSM pages** with zero LOD bias for razor-sharp shadows

The net visual result is **comparable or better** than HWRT at lower settings, because every other system runs at maximum quality.

---

## System RAM Budget (16384 MB Total)

```
┌─────────────────────────────────────────────────────────┐
│                  System RAM — 16 GB                     │
├──────────────────────────────────┬──────────────────────┤
│  UE5 Editor                     │         4000-6000 MB  │
│  Game Simulation (PIE)          │         2000-4000 MB  │
│  Shader Compilation (6 workers) │         ~1536 MB max  │
│  Operating System + Services    │         2000-3000 MB  │
│  DDC / Asset Cooking            │          500-1000 MB  │
├──────────────────────────────────┼──────────────────────┤
│  Available Headroom             │         1500-3000 MB  │
└──────────────────────────────────┴──────────────────────┘
```

The shader compiler is capped at 6 parallel jobs (of 16 logical threads) and 1536 MB memory to prevent OOM during heavy compilation passes while leaving headroom for the editor and OS.

---

## Scalability Presets

| Preset | Target Hardware | Scalability Level | Lumen | VSM | Nanite | Dynamic Res |
|--------|----------------|:-:|:-:|:-:|:-:|:-:|
| `Low_Mobile` | Android / iOS | 0 | Off | Off | Off | On (30fps) |
| `Medium` | Dedicated < 4GB | 1 | Off | Off | Basic | On (30fps) |
| `High` | Dedicated 4-6GB | 2 | SW Low | 1024 pgs | Yes | On (60fps) |
| **`Development_Laptop`** | **RTX 5070 8GB** | **3** | **SW Max** | **2048 pgs** | **Full** | **On (60fps)** |
| `Epic_PC` | 12GB+ VRAM | 3 | HW RT | 2048 pgs | Full | Off (uncapped) |

The `Development_Laptop` preset is **auto-selected** when the system detects 6-12 GB dedicated VRAM. It runs scalability level 3 (all quality groups at maximum) with software Lumen and dynamic resolution enabled.

---

## Context-Aware Runtime Optimization

### Camera Mode Transitions

The `EdrathHardwareOptimizationSubsystem` dynamically adjusts rendering when the camera switches:

| Setting | Bird's-Eye (Exterior) | Third-Person (Interior) |
|---------|:---------------------:|:----------------------:|
| View Distance Scale | 1.1 | 0.75 |
| Mesh LOD Scale | 0.75 | 0.5 |
| Skeletal LOD Bias | +1 | 0 |
| Nanite Pixels/Edge | 1.2 | 0.8 |
| VSM Pages | 1536 | 2048 |
| VSM LOD Bias | 0.5 | 0.0 |
| Hidden Primitive Scale | 0.25 | 0.0 |

**Bird's-eye** trades close-up detail for draw distance (more geometry visible).
**Third-person** trades draw distance for close-up detail (fewer objects, higher quality per object).

### Timeline State Transitions (Possibility Shifting)

During timeline shifts, two world states may briefly coexist in memory:

| Setting | Present | Rejected | Memory Reconstruction |
|---------|:-------:|:--------:|:---------------------:|
| Texture Pool | 2560 MB | 2048 MB | 1792 MB |
| Stream Rate | 4/frame | 2/frame | 1/frame |
| Nanite Pixels/Edge | 1.0-1.2 | 1.5 | 1.25 |
| Hidden Prim Scale | 0.5 | 0.0 | 0.0 |

The reduced quality in Rejected Timeline and Memory Reconstruction is **thematically appropriate** — reality is degraded/fragmented in these states.

---

## Editor Performance Tips

### Viewport Settings (Manual — Editor Preferences)
1. **Edit → Editor Preferences → Performance**
   - Set "Monitoring" to show FPS, GPU time, and VRAM in the viewport
2. **Viewport Options (top-left dropdown)**
   - Use "Lit" mode for final look; switch to "Unlit" or "Wireframe" when editing geometry
   - Disable "Realtime" in inactive viewports (saves 20-30% GPU)
   - Use "Low" or "Medium" engine scalability while editing, "Epic" only for final previews
3. **View → Engine Scalability Settings**
   - Set to "High" during general editing, "Epic" for visual verification

### Shader Compilation
- First launch after changes: expect 15-30 minutes of shader compilation
- Subsequent launches: cached shaders load in seconds
- During heavy compilation, the editor may stutter — this is normal with 6 workers
- Monitor RAM usage in Task Manager; if > 14GB, reduce `r.ShaderCompiler.MaxShaderJobs` to 4

### DDC (Derived Data Cache) Management
- DDC is capped at **28 GB** to protect the 512 GB NVMe
- Unused cache files are cleaned after **7 days**
- To manually clean: delete `DerivedDataCache/` and `Intermediate/` folders
- Use `Scripts/CleanBuildArtifacts.sh` (Linux/macOS) or `Scripts/CleanBuildArtifacts.ps1` (Windows)

### Play In Editor (PIE) Performance
- PIE adds ~500 MB RAM overhead on top of the editor
- For best PIE performance, close Content Browser thumbnails and other editor panels
- Use "Selected Viewport" PIE mode instead of "New Editor Window" to save GPU memory

---

## Monitoring & Debugging Performance

### In-Editor Console Commands
```
stat fps              — Frame rate
stat unit             — Frame time breakdown (Game, Draw, GPU, RHIT)
stat gpu              — Per-pass GPU timing
stat streaming        — Texture streaming status and pool usage
stat rhi              — RHI memory and draw call stats
stat scenerendering   — Scene rendering breakdown
stat nanite           — Nanite rasterization stats
stat lumen            — Lumen GI and reflection stats

r.ScreenPercentage X  — Adjust TSR input resolution (50-100)
r.Streaming.PoolSize X — Adjust texture pool (MB) at runtime
```

### VRAM Monitoring
The `EdrathHardwareOptimizationSubsystem` logs the full VRAM budget breakdown at startup. Check the Output Log for lines starting with `Edrath VRAM Budget:` and `Edrath HW Profile:`.

### If You Experience Stuttering
1. Check `stat gpu` — if GPU time > 16ms, rendering is the bottleneck
2. Check `stat streaming` — if "Over Budget" appears, reduce `r.Streaming.PoolSize`
3. Check Task Manager RAM — if > 14 GB, close background apps or reduce shader jobs
4. Try reducing `r.Lumen.ScreenProbeGather.DownsampleFactor` from 8 to 12 or 16
5. Try reducing `r.Shadow.Virtual.MaxPhysicalPages` from 2048 to 1536

---

## File Reference

| File | Purpose |
|------|---------|
| `Config/DefaultEngine.ini` | Engine rendering, streaming, shader, and DDC configuration |
| `Config/DefaultScalability.ini` | 4-tier quality presets with VRAM-appropriate budgets |
| `Source/Edrath/Public/Core/EdrathTypes.h` | Quality preset enum (`EEdrathQualityPreset`) |
| `Source/Edrath/Public/Core/EdrathScalabilitySubsystem.h` | Scalability subsystem with VRAM-aware auto-detection |
| `Source/Edrath/Private/Core/EdrathScalabilitySubsystem.cpp` | Hardware detection, preset application, laptop overrides |
| `Source/Edrath/Public/Core/EdrathHardwareOptimizationSubsystem.h` | Hardware profile, VRAM budget structs, optimization subsystem |
| `Source/Edrath/Private/Core/EdrathHardwareOptimizationSubsystem.cpp` | VRAM budget allocation, camera/timeline runtime optimization |
| `Source/Edrath/Private/Core/EdrathGameInstance.cpp` | Startup logging for hardware detection results |
