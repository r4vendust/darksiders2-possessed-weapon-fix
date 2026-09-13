# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [1.0.0] - 2026-09-13

### Added
- Initial release of the Possessed Weapon Overflow Fix.
- Hook on `FUN_140149da8` (offset `0x149DA8`) to intercept item additions
  to arrays.
- `_ReturnAddress()` filter to ensure only the weapon upgrade context is
  affected (`0x5BE300`).
- Limit of 6 items enforced to prevent buffer overflow.
- Prevention of permanent stat loss on Possessed Weapons.

### Fixed
- The "INVALID ID" bug that corrupted Possessed Weapons after exceeding
  6 attribute slots.
- Memory corruption that caused attribute slots (7, 8, etc.) to be filled
  with memory addresses instead of values.
- Permanent loss of all attributes after repeated failed feeds.
- UI corruption caused by overflow in the weapon's data structure.

### Verified
- Feeding a Possessed Weapon repeatedly past the hard cap.
- Multiple drop-and-pick glitches on the same weapon.
- Switching between weapons and back.
- Saving and loading with a maxed-out weapon.
- Multiple different Possessed Weapons.
- Normal gameplay (combat, inventory, quests, save/load).

### Technical Notes
- **Hook target:** `Darksiders2.exe + 0x149DA8` (`FUN_140149da8`)
- **Filter caller:** `Darksiders2.exe + 0x5BE300`
- **Max items allowed:** 6
- **Compiler:** Visual Studio 2026
- **Library:** MinHook
- **ASI Loader:** Ultimate ASI Loader

### Compatibility
- **Game:** Darksiders 2 Deathinitive Edition (Epic Games)
- **Platform:** Windows x64

### Known Limitations
- The visual "INVALID ID" text still appears in the Level Up UI when
  attempting to add a 7th attribute. This is cosmetic only and does not
  affect gameplay or save data.
- This mod does not increase the maximum number of attribute slots.
  The intended limit is 6 items (2 damage + 4 upgrade attributes).
- Not tested on the Steam version. It may work if the offsets are
  identical, but this has not been verified.

---

## [Unreleased]

### Planned
- (Nothing planned yet — future updates will be added here.)