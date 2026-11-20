# Edrath: 3D Modeling, Rigging & Technical Art Pipeline Guide

This guide defines the production standards for 3D artists, modelers, and technical animators building assets for **Edrath: The World That Forgot Tomorrow** in **Unreal Engine 5.5+**.

---

## 1. Laptop Performance & Polycount Budgets

To ensure high-fidelity rendering without causing thermal throttling or stuttering on developer laptops (e.g., RTX 3050/3060/4060 Laptop GPUs or Apple Silicon / AMD APUs):

| Asset Category | Target Polygon Count | Nanite Enabled | Collision Type | Texture Resolution |
| :--- | :--- | :--- | :--- | :--- |
| **Hero Character (Kael Veyr)** | 40,000 – 65,000 Tris | No (Skeletal Mesh) | Physics Asset / Capsule | 4096 x 4096 (Head + Body + Armor) |
| **Companions & Elite Bosses** | 30,000 – 50,000 Tris | No (Skeletal Mesh) | Physics Asset / Capsule | 4096 x 4096 |
| **Common Enemies & NPCs** | 15,000 – 25,000 Tris | No (Skeletal Mesh) | Physics Asset / Capsule | 2048 x 2048 |
| **Modular Architectural Kits** | 50,000 – 500,000 Tris | **Yes (`bEnableNanite=true`)** | Simplified Box / Convex Hull | 2048 x 2048 (Tiling ORM) |
| **Hero Crypts & Ancient Ruins** | 200,000 – 2,000,000 Tris | **Yes (`bEnableNanite=true`)** | Complex as Simple (Nanite Raycast) | 4096 x 4096 Virtual Texture |
| **Small Props & Clues** | 1,500 – 8,000 Tris | Optional | Box / Sphere | 1024 x 1024 or 2048 x 2048 |
| **Foliage & Hollowgreen Roots** | 10,000 – 80,000 Tris | **Yes (`bEnableNanite=true`)** | No Collision or Capsule | 2048 x 2048 |

---

## 2. Skeletal Hierarchy & Mandatory Sockets

All humanoid characters must adhere to the **UE5 Mannequin Skeleton standard** (`SKEL_Kael`). The following sockets are strictly required by the C++ gameplay subsystems:

```text
root
└── pelvis
    ├── spine_01
    │   └── spine_02
    │       └── spine_03
    │           ├── [Socket] spine_03_holster (Greatsword / Staff back mounting)
    │           ├── clavicle_l -> upperarm_l -> lowerarm_l -> hand_l
    │           │   └── [Socket] hand_l_relic (Clue inspection / Off-hand relic)
    │           ├── clavicle_r -> upperarm_r -> lowerarm_r -> hand_r
    │           │   └── [Socket] hand_r_weapon (Primary combat sword socket)
    │           ├── clavicle_center (Added bone/socket)
    │           │   └── [Socket] bone_chest_clock (Black Clock chest mesh & Niagara anchor)
    │           └── neck_01 -> head
    │               └── [Socket] socket_camera_focus (Over-the-shoulder look target)
    └── thigh_l/r -> calf_l/r -> foot_l/r (IK foot bones for Control Rig)
```

---

## 3. Texture Channel Packing Standard (ORM)

All 3D assets in Edrath use the **PBR Metallic/Roughness workflow** with standardized 3-texture material slots:

### Texture 1: Base Color (`T_*_BC`)
* **Format**: DXGI_FORMAT_BC7_UNORM_SRGB
* **RGB**: Albedo / Diffuse base color in sRGB space.
* **Alpha (Optional)**: Opacity / Alpha cutout mask (for hair, foliage, and chainmail).

### Texture 2: Packed ORM (`T_*_ORM`)
* **Format**: DXGI_FORMAT_BC7_UNORM (Linear, `sRGB = false`)
* **Red Channel**: **Ambient Occlusion (AO)**
* **Green Channel**: **Roughness** (0.0 = Glass smooth, 1.0 = Matte rough)
* **Blue Channel**: **Metallic** (0.0 = Dielectric, 1.0 = Pure Conductor)

### Texture 3: Normal Map (`T_*_N`)
* **Format**: DXGI_FORMAT_BC5_UNORM (Two-channel compressed normal)
* **Standard**: DirectX standard (Normal Y / Green channel pointing **down**; Unreal Engine standard).

### Texture 4 (Hero Characters Only): Packed Customizer Mask (`T_*_Mask`)
* **Red Channel**: Primary Cloth Tint Zone
* **Green Channel**: Secondary Leather / Armor Plate Tint Zone
* **Blue Channel**: Black Clock Inlay Metal (Gold, Obsidian, Silver)
* **Alpha Channel**: Dirt, Blood, and Battle Wear Mask

---

## 4. Modeling in Unreal Engine 5.5 Editor Mode

Thanks to `ModelingToolsEditorMode` and `GeometryScripting` enabled in `Edrath.uproject`, you can create and modify 3D geometry directly inside the Unreal Engine Editor on your laptop:

1. **Switch to Modeling Mode**: Press `Shift + 5` or select **Modeling** from the Modes dropdown.
2. **Greybox Prototyping**:
   * Use **Shape** tools (`Box`, `Cylinder`, `Stairs`) to block out Veyl buildings and crypt rooms.
   * Use **PolyModel** (`PolyExtrude`, `InsertEdgeLoop`, `Bevel`) to refine doorways and arches.
3. **Sculpting Ruined Stones & Crypt Walls**:
   * Select a mesh and open **Deform -> Sculpt**.
   * Choose brushes like `Smooth`, `Flatten`, or `Pinch` to sculpt ancient wear onto sarcophagi and shattered bridge geometry.
4. **Boolean Reality States**:
   * Create an intact bridge mesh (`SM_GorgeBridge_Intact`).
   * Duplicate it and use **Model -> Mesh Boolean (Difference)** with stone debris to produce the collapsed version (`SM_GorgeBridge_Collapsed`) for `APossibilityShiftActor`.
5. **Convert to Nanite**:
   * Right-click the newly generated static mesh in the Content Browser -> **Nanite -> Enable**.

---

## 5. DCC Export Settings (Blender & Autodesk Maya)

When exporting assets from external 3D software:

### Blender (v4.0+) FBX Export Settings
* **Units**: Metric, Unit Scale `0.01` (1 unit = 1 centimeter).
* **Geometry**:
  * Smoothing: `Face`
  * Apply Modifiers: `Checked`
* **Armature**:
  * Primary Bone Axis: `Y Axis`
  * Secondary Bone Axis: `X Axis`
  * Add Leaf Bones: `Unchecked`
  * Only Deform Bones: `Checked`
* **Transform**:
  * Forward: `-Z Forward`
  * Up: `Y Up` (or standard `Apply Transform` enabled)

### Autodesk Maya FBX Export Settings
* **Up Axis**: `Z-Up`
* **Units**: `Centimeters`
* **FBX Version**: FBX 2020 or later
* **Smoothing Groups**: `Checked`
* **Tangents and Binormals**: `Checked`
* **Triangulate**: `Unchecked` (let Unreal Engine tessellate Nanite geometry)

---

## 6. Viewport Performance Tips for Laptops

When working inside the Unreal Engine Editor on laptop battery or mid-tier mobile GPUs:

1. **Activate Laptop Scalability Preset**:
   * Run console command `Windows_Laptop_Balanced` or `Windows_Laptop_Battery`.
2. **Limit Viewport Frame Rate**:
   * Run console command: `t.MaxFPS 60`
3. **Toggle Nanite Visualization**:
   * In Viewport: `View Mode -> Nanite Visualization -> Triangles` to inspect mesh density in real time.
4. **Monitor VRAM Usage**:
   * Run console command: `stat RHI` and `stat Streaming` to verify texture pool memory stays under 3GB.
