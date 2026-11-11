#!/usr/bin/env python3
"""
Edrath: The World That Forgot Tomorrow
Pure-Python + Git CLI Universal Stage 2 Commits Applier

Applies the 5 discrete sequential git commits for Stage 2 Pre-Alpha Playable Vertical Slice.
Works via git CLI when available, or directly via native Git object serialization (SHA-1/zlib)
when shell binary execution permissions are restricted in Termux / Android environments.
"""

import os
import sys
import time
import zlib
import hashlib
import subprocess

REPO_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
GIT_DIR = os.path.join(REPO_ROOT, ".git")

AUTHOR_NAME = "rubivssingh281"
AUTHOR_EMAIL = "rubivssingh281@gmail.com"
TIMEZONE = "+0530"

COMMIT_SPECS = [
    {
        "scope": "feat(act1): implement Zone 1 battlefield awakening, dying soldier narrative actor, and battlefield wraith echo combat enemy",
        "files": [
            "Source/Edrath/Public/Core/EdrathGameplayTags.h",
            "Source/Edrath/Private/Core/EdrathGameplayTags.cpp",
            "Source/Edrath/Public/Character/BattlefieldWraithCharacter.h",
            "Source/Edrath/Private/Character/BattlefieldWraithCharacter.cpp",
            "Source/Edrath/Public/World/DyingSoldierActor.h",
            "Source/Edrath/Private/World/DyingSoldierActor.cpp",
        ]
    },
    {
        "scope": "feat(act1): implement Zone 2 gorge bridge possibility shifting and Zone 3 looping settlement villager agency",
        "files": [
            "Source/Edrath/Public/BlackClock/PossibilityShiftActor.h",
            "Source/Edrath/Public/World/Act1GorgeBridgeActor.h",
            "Source/Edrath/Private/World/Act1GorgeBridgeActor.cpp",
            "Source/Edrath/Public/Character/SettlementVillagerCharacter.h",
            "Source/Edrath/Private/Character/SettlementVillagerCharacter.cpp",
        ]
    },
    {
        "scope": "feat(act1): implement Zone 4 house forensic clues and Zone 5 crypt 114-year-dead priest memory reconstruction",
        "files": [
            "Source/Edrath/Public/World/KaelHouseInteriorActor.h",
            "Source/Edrath/Private/World/KaelHouseInteriorActor.cpp",
            "Source/Edrath/Public/World/PriestCorpseActor.h",
            "Source/Edrath/Private/World/PriestCorpseActor.cpp",
        ]
    },
    {
        "scope": "feat(act1): scaffold PreAlpha_Act1 level script actor, world partition data layers, and automated 5-zone vertical slice tests",
        "files": [
            "Source/Edrath/Public/World/Act1BattlefieldLevelScriptActor.h",
            "Source/Edrath/Private/World/Act1BattlefieldLevelScriptActor.cpp",
            "Content/Maps/PreAlpha_Act1/L_PreAlpha_Act1_Manifest.json",
            "Content/Maps/PreAlpha_Act1/DataLayers/DataLayer_PresentTimeline.json",
            "Content/Maps/PreAlpha_Act1/DataLayers/DataLayer_RejectedTimeline.json",
            "Content/Maps/PreAlpha_Act1/DataLayers/DataLayer_MemoryEcho.json",
            "Source/Edrath/Private/Tests/EdrathPreAlphaAutomationTest.cpp",
        ]
    },
    {
        "scope": "docs(stage2): update CHANGELOG, README, and provide 5-commit automated git packaging scripts",
        "files": [
            "CHANGELOG.md",
            "README.md",
            "Scripts/create_stage2_commits.sh",
            "Scripts/create_stage2_commits.bat",
            "Scripts/apply_stage2_git_commits.py",
        ]
    }
]

def try_git_cli():
    """Attempt committing via Git command line interface."""
    try:
        ver = subprocess.run(["git", "--version"], stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=True)
        print(f"[OK] Found Git CLI: {ver.stdout.decode().strip()}")
        for i, spec in enumerate(COMMIT_SPECS, 1):
            print(f"[{i}/5] Committing: {spec['scope']}")
            for f in spec["files"]:
                full_path = os.path.join(REPO_ROOT, f)
                if os.path.exists(full_path):
                    subprocess.run(["git", "add", f], cwd=REPO_ROOT, check=True)
            subprocess.run(["git", "commit", "-m", spec["scope"]], cwd=REPO_ROOT, check=True)
        print("\n[SUCCESS] All 5 commits created via Git CLI!")
        subprocess.run(["git", "log", "-n", "5", "--oneline"], cwd=REPO_ROOT)
        return True
    except Exception as exc:
        print(f"[INFO] Git CLI not executable or failed ({exc}). Falling back to native Python Git engine...")
        return False

# --- Native Python Git Implementation ---

def write_git_object(obj_type: str, data: bytes) -> str:
    header = f"{obj_type} {len(data)}\0".encode()
    store = header + data
    sha = hashlib.sha1(store).hexdigest()
    obj_dir = os.path.join(GIT_DIR, "objects", sha[:2])
    obj_file = os.path.join(obj_dir, sha[2:])
    if not os.path.exists(obj_file):
        os.makedirs(obj_dir, exist_ok=True)
        with open(obj_file, "wb") as f:
            f.write(zlib.compress(store))
    return sha

def read_git_object(sha: str):
    obj_file = os.path.join(GIT_DIR, "objects", sha[:2], sha[2:])
    with open(obj_file, "rb") as f:
        decomp = zlib.decompress(f.read())
    nul_pos = decomp.find(b"\0")
    header = decomp[:nul_pos].decode()
    obj_type, size = header.split(" ")
    content = decomp[nul_pos + 1:]
    return obj_type, content

def parse_tree(data: bytes):
    entries = []
    idx = 0
    while idx < len(data):
        space_pos = data.find(b" ", idx)
        mode = data[idx:space_pos].decode()
        null_pos = data.find(b"\0", space_pos)
        name = data[space_pos + 1:null_pos].decode()
        sha_bytes = data[null_pos + 1:null_pos + 21]
        sha = sha_bytes.hex()
        entries.append((mode, name, sha))
        idx = null_pos + 21
    return entries

def serialize_tree(entries):
    # Sort: directories are compared as if they had a trailing '/'
    def sort_key(item):
        mode, name, _ = item
        return name + "/" if mode.startswith("40") else name
    sorted_entries = sorted(entries, key=sort_key)
    out = b""
    for mode, name, sha in sorted_entries:
        out += f"{mode} {name}\0".encode() + bytes.fromhex(sha)
    return write_git_object("tree", out)

def get_tree_map(tree_sha: str, prefix=""):
    """Recursively unpacks a tree SHA into {rel_path: (mode, blob_sha)}."""
    obj_type, data = read_git_object(tree_sha)
    assert obj_type == "tree"
    mapping = {}
    for mode, name, sha in parse_tree(data):
        rel_path = os.path.join(prefix, name) if prefix else name
        if mode.startswith("40"): # tree
            mapping.update(get_tree_map(sha, rel_path))
        else:
            mapping[rel_path] = (mode, sha)
    return mapping

def build_tree_from_map(mapping: dict) -> str:
    """Builds nested git trees from a flat {rel_path: (mode, blob_sha)} dictionary."""
    tree_nodes = {} # dir_path -> list of (mode, basename, sha)
    dirs = set()
    for rel_path, (mode, sha) in mapping.items():
        parts = rel_path.replace("\\", "/").split("/")
        filename = parts[-1]
        parent = "/".join(parts[:-1])
        tree_nodes.setdefault(parent, []).append((mode, filename, sha))
        for i in range(len(parts) - 1):
            dirs.add("/".join(parts[:i + 1]))

    sorted_dirs = sorted(dirs, key=lambda d: len(d.split("/")), reverse=True)
    for d in sorted_dirs:
        sub_entries = tree_nodes.get(d, [])
        sub_tree_sha = serialize_tree(sub_entries)
        parts = d.split("/")
        basename = parts[-1]
        parent = "/".join(parts[:-1])
        tree_nodes.setdefault(parent, []).append(("40000", basename, sub_tree_sha))

    root_entries = tree_nodes.get("", [])
    return serialize_tree(root_entries)

def apply_native_python_commits():
    print("[1/2] Initializing Native Python Git Serializer...")
    ref_path = os.path.join(GIT_DIR, "refs", "heads", "main")
    with open(ref_path, "r") as f:
        parent_sha = f.read().strip()

    print(f"  * Current HEAD Commit: {parent_sha}")
    _, commit_data = read_git_object(parent_sha)
    # Parse tree from commit
    for line in commit_data.decode(errors="replace").split("\n"):
        if line.startswith("tree "):
            base_tree_sha = line.split(" ")[1].strip()
            break

    working_map = get_tree_map(base_tree_sha)
    now_ts = int(time.time())

    for idx, spec in enumerate(COMMIT_SPECS, 1):
        print(f"[{idx}/5] Staging and writing objects for: {spec['scope']}")
        for rel_file in spec["files"]:
            full_path = os.path.join(REPO_ROOT, rel_file)
            if not os.path.exists(full_path):
                continue
            with open(full_path, "rb") as f:
                content = f.read()
            blob_sha = write_git_object("blob", content)
            norm_path = rel_file.replace("\\", "/")
            mode = "100755" if (norm_path.endswith(".sh") or norm_path.endswith(".py")) else "100644"
            working_map[norm_path] = (mode, blob_sha)

        new_tree_sha = build_tree_from_map(working_map)
        commit_body = (
            f"tree {new_tree_sha}\n"
            f"parent {parent_sha}\n"
            f"author {AUTHOR_NAME} <{AUTHOR_EMAIL}> {now_ts + idx * 60} {TIMEZONE}\n"
            f"committer {AUTHOR_NAME} <{AUTHOR_EMAIL}> {now_ts + idx * 60} {TIMEZONE}\n\n"
            f"{spec['scope']}\n"
        ).encode()
        new_commit_sha = write_git_object("commit", commit_body)

        # Update logs
        log_entry = f"{parent_sha} {new_commit_sha} {AUTHOR_NAME} <{AUTHOR_EMAIL}> {now_ts + idx * 60} {TIMEZONE}\tcommit: {spec['scope']}\n"
        for log_file in [os.path.join(GIT_DIR, "logs", "HEAD"), os.path.join(GIT_DIR, "logs", "refs", "heads", "main")]:
            if os.path.exists(log_file):
                with open(log_file, "a") as f:
                    f.write(log_entry)

        parent_sha = new_commit_sha
        print(f"  -> Created Commit: {new_commit_sha[:7]} {spec['scope']}")

    with open(ref_path, "w") as f:
        f.write(parent_sha + "\n")

    print("\n==========================================================")
    print(" [SUCCESS] All 5 Stage 2 commits committed to .git repository!")
    print(f" New HEAD: {parent_sha}")
    print("==========================================================")

if __name__ == "__main__":
    if not try_git_cli():
        apply_native_python_commits()
