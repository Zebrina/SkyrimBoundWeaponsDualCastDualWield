#pragma once

namespace BoundWeapons
{
    extern RE::BGSEquipSlot* RightHand;
    extern RE::BGSEquipSlot* BothHands;

    void Init(SKSE::Trampoline& trampoline);
    void UpdateData();
};