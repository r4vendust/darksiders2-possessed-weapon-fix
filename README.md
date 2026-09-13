# Darksiders 2 - Possessed Weapon Overflow Fix

Fixes the **"INVALID ID"** bug that occurs when feeding a Possessed Weapon
after it reaches the maximum number of attribute slots.

**Author:** r4vendust
**Version:** 1.0.0
**Game Version:** Darksiders 2 Deathinitive Edition (Epic Games)

---

## 🐛 The Bug

When repeatedly feeding a Possessed Weapon, the game tries to add a **7th
attribute** to an array that only supports **6 items**. This causes a buffer
overflow that corrupts the weapon's data structure, resulting in:

- The **"INVALID ID"** error displayed in the Level Up UI.
- Memory addresses being written into attribute slots (Slot 7, 8, etc.).
- **Permanent loss of all stats** after several failed feeds.
- UI corruption and, eventually, save file damage.

The bug was never officially fixed by the developers and has affected players
for over a decade.

---

## ✅ The Fix

This mod intercepts the engine function responsible for adding items to
arrays (`FUN_140149da8`) and **silently blocks** attempts to exceed the
6-item limit. The `_ReturnAddress()` filter ensures that **only the weapon
upgrade context** is affected — all other uses of the function (inventory,
quests, save loading) continue to work normally.

### What this mod **does**:

- Prevents the "INVALID ID" bug from **corrupting your weapon's memory**.
- Preserves all existing attributes on your Possessed Weapon.
- Works with **any** Possessed Weapon (universal filter).
- Is compatible with **save files** (no corruption, no editing required).
- Leaves the game's normal behavior for inventory, quests, and save loading
  completely untouched.

### What this mod **doesn't** do:

- It does **not** add new attribute slots beyond the intended 6.
- It does **not** remove the visual **"INVALID ID"** text. The text will
  still appear in the Level Up UI when you try to select a 7th attribute,
  but **no memory corruption occurs** and your weapon remains safe.

---

## ⚠️ Important Note About the Visual "INVALID ID" Text

You will **still see** the "INVALID ID" text in the Level Up screen when
attempting to add a 7th attribute. **This is expected.**

- The text is purely **cosmetic** and is rendered by the game's Scaleform UI.
- The actual bug (memory corruption) is **prevented** by the hook.
- Your weapon's stats **remain intact**, and your save file is **safe**.

Removing the visual text would require hooking into the Scaleform UI system,
which is significantly more complex and can introduce new bugs. The current
fix targets the **root cause** (memory corruption) without touching the UI.

---

## 📦 Installation (End Users)

### Prerequisites

- **Darksiders 2 Deathinitive Edition** (Epic Games version).
- [Ultimate ASI Loader](https://github.com/ThirteenAG/Ultimate-ASI-Loader/releases)
  (free, open-source).

### Steps

1. **Download the Ultimate ASI Loader** from the link above.
2. **Copy `dxgi.dll`** from the ASI Loader package into your Darksiders 2
   installation folder (the same folder as `Darksiders2.exe`).
   - Default Epic Games path:
     `C:\Program Files\Epic Games\Darksiders2\`
   - If you installed the game on another drive, look for the folder that
     contains `Darksiders2.exe`.
3. **Create a folder named `scripts`** inside the game folder (if it doesn't
   already exist).
4. **Place `Darksiders2Fix.asi`** inside the `scripts` folder.
   - ⚠️ **Note:** The file must have the **`.asi`** extension, not `.dll`.
     If you downloaded the file as `.dll`, simply rename it to
     `Darksiders2Fix.asi`.
5. **Launch the game normally.** The fix is applied automatically.

### Final folder structure

---

## 🛠️ Building from Source (Developers)

If you want to compile the mod yourself (instead of downloading the pre-built
release), follow these steps.

### Prerequisites

- **Visual Studio 2026** (or newer) with the **Desktop development with C++**
  workload installed.
- **MinHook** library (already included in the project, or download from
  [GitHub](https://github.com/TsudaKageyu/minhook)).
- **Windows SDK** (10 or 11).

### Compilation Steps

1. **Clone or download** this repository.
2. **Open the solution file** in Visual Studio:

- ⚠️ **Note:** The `.slnx` extension is the new XML-based solution format
  introduced in Visual Studio 2026. It requires VS 2026 or newer.
3. **Set the build configuration** to:
- **Configuration:** `Release`
- **Platform:** `x64`
- ⚠️ **Important:** The game is 64-bit. Building for `x86` will NOT work.
4. **Build the project** (`Ctrl + Shift + B`).
5. Locate the compiled DLL in:

6. **Rename the file** from `Darksiders2Fix.dll` to
**`Darksiders2Fix.asi`**.
- ⚠️ This step is **mandatory**. The ASI Loader only loads files with
  the `.asi` extension.
7. **Place the renamed file** in the `scripts` folder of the game.

### Why rename `.dll` to `.asi`?

The **Ultimate ASI Loader** (which you installed as `dxgi.dll`) scans the
`scripts` folder for files with the `.asi` extension. Internally, the file
is still a standard Windows DLL — it just has a different extension that
the ASI Loader recognizes.

This is the standard convention for ASI-based mods (used by GTA, Bully,
Darksiders, and many other games). It allows the loader to distinguish
mod DLLs from system DLLs.

### Required Dependencies

| Dependency | Purpose | Link |
|---|---|---|
| **MinHook** | Runtime function hooking | [GitHub](https://github.com/TsudaKageyu/minhook) |
| **Ultimate ASI Loader** | DLL injection | [GitHub](https://github.com/ThirteenAG/Ultimate-ASI-Loader) |
| **Visual Studio 2026** | C++ compiler | [Microsoft](https://visualstudio.microsoft.com/) |

### Project Structure

---

## 🔧 How It Works (Technical Details)

This mod was built using **reverse engineering** with:

- **Ghidra** — Static analysis of the game's executable.
- **Cheat Engine** — Dynamic analysis, breakpoints, and memory inspection.
- **MinHook** — Runtime function hooking.

### Key offsets discovered:

| Offset     | Description                                      |
|------------|--------------------------------------------------|
| `0x149DA8` | Generic function that adds items to arrays       |
| `0x5BE300` | Return address of the weapon upgrade caller      |
| `+0x08`    | Offset of the array size within the array struct |
| `+0x360`   | Offset of the upgrade counter in the weapon      |

### How the hook works:

1. The game calls `FUN_140149da8` to add an item to an array.
2. The hook intercepts the call and reads `_ReturnAddress()`.
3. If the return address matches `baseAddress + 0x5BE300`, the call came
   from the **weapon upgrade context**.
4. The hook reads the current array size at offset `+0x08`.
5. If the size is already `>= 6`, the hook returns early (blocking the
   addition).
6. Otherwise, the original function runs normally.

---

## 🧪 Verified Scenarios

This mod has been tested with:

- ✅ Feeding a Possessed Weapon repeatedly past the hard cap.
- ✅ Multiple drop-and-pick glitches on the same weapon.
- ✅ Switching between weapons and back.
- ✅ Saving and loading with a maxed-out weapon.
- ✅ Multiple different Possessed Weapons.
- ✅ Normal gameplay (combat, inventory, quests, save/load).

**Note:** All tests were performed on the **Epic Games version** of
Darksiders 2 Deathinitive Edition.

---

## ❓ Frequently Asked Questions

**Q: Why do I still see "INVALID ID" in the Level Up screen?**

A: Because that text is rendered by the game's UI system, and this mod only
fixes the underlying memory corruption. The text is purely cosmetic and does
not affect your weapon's stats or your save file.

**Q: Will my weapon lose stats if I keep feeding it?**

A: No. The hook prevents the overflow that would corrupt the weapon. Your
stats will remain intact even if you feed the weapon repeatedly.

**Q: Does this mod work with an existing save file?**

A: Yes. The mod does not modify save files. It only prevents real-time memory
corruption.

**Q: Will this mod let me add more than 6 attributes?**

A: No. The game's engine has a fixed array size of 6 items. This mod does not
change that limit — it simply prevents the corruption that occurs when the
game tries to exceed it.

**Q: Does this mod work with the Steam version?**

A: This mod was developed and tested for the **Epic Games version** of
Darksiders 2 Deathinitive Edition. It **may** work with the Steam version if
the offsets are identical, but this has **not been tested**. If you try it
on Steam, please leave feedback on the release page.

**Q: The file I downloaded is a `.dll`. Do I need to rename it?**

A: **Yes.** The ASI Loader only loads files with the `.asi` extension. Rename
the file from `Darksiders2Fix.dll` to `Darksiders2Fix.asi` before placing
it in the `scripts` folder.

**Q: I compiled the code myself. What do I do with the DLL?**

A: Rename the compiled `Darksiders2Fix.dll` to **`Darksiders2Fix.asi`** and
place it in the `scripts` folder. The `.asi` extension is required by the
Ultimate ASI Loader.

**Q: Can I use this mod with other Darksiders 2 mods?**

A: Yes, as long as they use the ASI Loader and don't conflict with the same
memory offsets. This mod only hooks `FUN_140149da8`, so conflicts are
unlikely.

**Q: The `.slnx` file won't open. What should I do?**

A: The `.slnx` format is only supported in **Visual Studio 2026 or newer**.
If you're using an older version, you'll need to upgrade or create a new
project manually.

---

## 🗑️ Uninstallation

Simply delete the file:

The mod leaves **no permanent changes** to your save files or the game
installation.

---

## 📜 License

**Copyright (c) 2026 r4vendust.**

This project is released under a **permissive license with attribution and
share-alike terms**. In plain English:

### You MAY:

- ✅ Use this mod for personal, non-commercial gameplay.
- ✅ **Study, learn from, and reuse** the source code in your own projects.
- ✅ **Modify, improve, and build upon** this mod.
- ✅ **Redistribute** your modified version.
- ✅ Include this mod in mod packs, guides, or tutorials.

### You MUST:

- 📝 **Give credit** to the original author (r4vendust).
- 📝 **Link back** to the original release page.
- 📝 **Clearly indicate** any changes you made to the code.
- 📝 Keep the `LICENSE` file intact in any redistribution.

### You may NOT:

- ❌ **Sell** this mod or your modified version as a commercial product.
- ❌ **Remove or alter** the copyright notice and license terms.
- ❌ **Claim authorship** of the original work.

### Why this license?

This mod was built through extensive reverse engineering work and is
released to help the Darksiders 2 community. The goal is to encourage
collaboration and improvement — if you find a better way to fix this bug,
or if you want to expand the fix to other systems, you are **free to do
so**. Just remember to credit the original work.

For commercial use or special licensing requests, please contact the
author directly via the official release page.

---

## 🙏 Credits

- **Author:** r4vendust
- **Reverse Engineering Tools:** Ghidra, Cheat Engine
- **Hooking Library:** [MinHook](https://github.com/TsudaKageyu/minhook)
- **ASI Loader:** [Ultimate ASI Loader](https://github.com/ThirteenAG/Ultimate-ASI-Loader)
- **Compiler:** Visual Studio 2026

**Special thanks** to the Darksiders 2 modding community for their
documentation and tools.

---

## 💬 Feedback & Support

Found a bug or have a suggestion? Please leave a comment on the **official
release page**. If you've modified or improved this mod, feel free to share
it — just remember to credit the original author and link back to this page.

**Enjoy your fixed Possessed Weapon!** 🎉