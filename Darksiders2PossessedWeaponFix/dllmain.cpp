// ============================================================================
// Darksiders 2 Deathinitive Edition - Possessed Weapon Overflow Fix
// ============================================================================
//
// This mod fixes the "INVALID ID" bug that occurs when feeding a Possessed
// Weapon after it reaches the maximum number of attribute slots. The original
// bug caused corruption of the weapon's data structure, leading to permanent
// loss of all stats and, eventually, destruction of the item.
//
// How it works:
//   - The generic function FUN_140149da8 (offset 0x149DA8) is responsible for
//     adding items to arrays throughout the game engine.
//   - A Possessed Weapon's attribute array holds 6 usable slots:
//         2 damage slots (min/max, auto-populated on drop)
//       + 4 attribute slots (the game's hardcap)
//   - The field at pArray+0x08 is a 1-based "next free slot" index, NOT the
//     number of items currently stored. The correct thresholds are:
//         count = 6  -> next add targets slot #6 (last valid)  -> ALLOW
//         count = 7  -> next add targets slot #7 (overflow)    -> BLOCK
//   - The hook intercepts the addition and returns early only when an actual
//     overflow would occur.
//   - The _ReturnAddress() filter ensures that only calls originating from the
//     weapon upgrade context are affected. All other uses of the function
//     (inventory, quests, save loading) continue to work normally.
//
// IMPORTANT NOTE:
//   - The visual "INVALID ID" text WILL still appear in the Level Up UI when
//     the player attempts to select a 7th slot. This is purely cosmetic.
//   - The actual bug (memory corruption) is prevented by this hook, so no
//     data is lost and the weapon remains fully functional.
//   - The visual text cannot be removed without hooking the Scaleform UI
//     system, which is out of scope for this fix.
//
// CHANGELOG:
//   - v2 (2026-09): Fixed an off-by-one that blocked the legitimate 4th
//     attribute. The previous build used a threshold of 6, which caused the
//     hook to reject a valid addition when the weapon had 2 damage slots and
//     3 attributes. The correct threshold is 7.
//
// ============================================================================
// LICENSE
// ============================================================================
// Copyright (c) 2026 [r4vendust]
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, subject to the following conditions:
//
//   1. The above copyright notice and this permission notice shall be
//      included in all copies or substantial portions of the Software.
//
//   2. Any derivative work, modification, or redistribution must give
//      appropriate credit to the original author, including a link to the
//      original release page.
//
//   3. Commercial use of this software (selling it as a standalone product)
//      is NOT permitted without prior written permission from the author.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES, OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT, OR OTHERWISE,
// ARISING FROM, OUT OF, OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
// ============================================================================

#include "pch.h"
#include <windows.h>
#include <cstdint>
#include <intrin.h>
#include "MinHook.h"

// ============================================================================
// GLOBAL VARIABLES
// ============================================================================

// Pointer to the original game function
typedef void(__fastcall* tAddToArray)(uintptr_t pArray, uintptr_t pItem);
tAddToArray oAddToArray = nullptr;

// Offsets discovered via reverse engineering
const uintptr_t OFFSET_ADD_ITEM_FUNCTION = 0x149DA8; // FUN_140149da8
const uintptr_t OFFSET_WEAPON_UPGRADE_CALLER = 0x5BE300; // Return address of upgrade

// The field at pArray+0x08 is a 1-based "next free slot" index. When it
// reaches 7, the next write would target slot index 6 (the 7th item), which
// is past the end of the 6-slot array. Block at >= 7 to prevent overflow
// while still allowing the legitimate 6th item (the game's 4th attribute).
const int BLOCK_THRESHOLD = 7;

// ============================================================================
// HOOK FUNCTION
// ============================================================================

void __fastcall Hooked_AddToArray(uintptr_t pArray, uintptr_t pItem) {
    // Get the return address to identify the call context
    uintptr_t returnAddress = (uintptr_t)_ReturnAddress();
    uintptr_t baseAddress = (uintptr_t)GetModuleHandleA(NULL);

    // Check if the call came from the weapon upgrade context
    bool isUpgradeContext = (returnAddress == (baseAddress + OFFSET_WEAPON_UPGRADE_CALLER));

    // If it's a weapon upgrade, check the limit before allowing the addition
    if (isUpgradeContext) {
        int* nextSlotPtr = (int*)(pArray + 0x08);
        if (*nextSlotPtr >= BLOCK_THRESHOLD) {
            // Silently block the addition that would cause the overflow.
            // The game's UI will still show "INVALID ID" visually, but no
            // memory corruption occurs and the weapon remains safe.
            return;
        }
    }

    // In all other contexts, call the original game function
    oAddToArray(pArray, pItem);
}

// ============================================================================
// INITIALIZATION
// ============================================================================

DWORD WINAPI InitThread(LPVOID lpParam) {
    // Wait for the main executable to fully load
    Sleep(3000);

    uintptr_t baseAddress = (uintptr_t)GetModuleHandleA(NULL);
    if (!baseAddress) return 1;

    // Absolute address of the function to hook
    LPVOID targetAddress = (LPVOID)(baseAddress + OFFSET_ADD_ITEM_FUNCTION);

    // Initialize MinHook
    if (MH_Initialize() != MH_OK) return 1;

    // Create the hook
    MH_STATUS status = MH_CreateHook(
        targetAddress,
        &Hooked_AddToArray,
        reinterpret_cast<LPVOID*>(&oAddToArray)
    );

    // Enable the hook
    if (status == MH_OK) {
        MH_EnableHook(targetAddress);
    }

    return 0;
}

// ============================================================================
// DLL MAIN
// ============================================================================

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)InitThread, NULL, 0, NULL);
        break;

    case DLL_PROCESS_DETACH:
        MH_DisableHook(MH_ALL_HOOKS);
        MH_Uninitialize();
        break;
    }
    return TRUE;
}