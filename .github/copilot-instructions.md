# GitHub Copilot Instructions for Edrath: The World That Forgot Tomorrow

1. **ENGINE & STANDARD:** Target Unreal Engine 5.5+ using C++20 (`CppStandardVersion.Cpp20`).
2. **ARCHITECTURAL ORDER:** Build the Phase 1 Core Base before level scripts, quests, or UI.
3. **DUAL CAMERA PERSPECTIVE:** Exterior exploration uses `ExteriorBirdsEye`; interiors use the blended `InteriorThirdPerson` perspective with the specified pitch, arm length, socket offset, and field of view.
4. **THE BLACK CLOCK:** Route timeline mutations, memory reconstruction, and Echo Combat through the Black Clock and narrative subsystem.
5. **NO PLACEHOLDERS:** Every generated Unreal source, configuration, and build file must be complete, compilable, and free of TODO or abbreviated snippets.
