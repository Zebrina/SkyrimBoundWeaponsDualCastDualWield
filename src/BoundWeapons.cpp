#include "BoundWeapons.h"

using namespace RE;

using namespace BoundWeapons;

BGSEquipSlot* BoundWeapons::RightHand;
BGSEquipSlot* BoundWeapons::BothHands;

using ActorEquipManager_EquipObject_Method = decltype(&ActorEquipManager::EquipObject);
REL::Relocation<ActorEquipManager_EquipObject_Method> ActorEquipManager_EquipObject;

using ActorEquipManager_UnequipObject_Method = decltype(&ActorEquipManager::UnequipObject);
REL::Relocation<ActorEquipManager_UnequipObject_Method> ActorEquipManager_UnequipObject;

ActiveEffect* GetDualCastBoundWeaponEffect(Actor* actor)
{
	auto activeEffectList = actor->GetActiveEffectList();
	if (activeEffectList)
	{
		for (auto effect : *activeEffectList)
		{
			if (effect->effect->baseEffect->GetArchetype() == EffectSetting::Archetype::kBoundWeapon && effect->flags.all(ActiveEffect::Flag::kDual))
			{
				return effect;
			}
		}
	}
	return nullptr;
}

void ActorEquipManager_EquipObject_Hook(ActorEquipManager* this__, Actor* actor, TESBoundObject* object, ExtraDataList* extraData, uint32_t count, const BGSEquipSlot* slot, bool queueEquip, bool forceEquip, bool playSounds, bool applyNow)
{
	if (object->IsWeapon() && slot != BothHands && static_cast<TESObjectWEAP*>(object)->IsBound() && GetDualCastBoundWeaponEffect(actor))
	{
		//BGSEquipSlot* secondEquipSlot = slot == LeftHand ? RightHand : LeftHand;
		//ActorEquipManager_EquipObject(this__, actor, object, extraData, count, secondEquipSlot, queueEquip, forceEquip, playSounds, applyNow);
		ActorEquipManager_EquipObject(this__, actor, object, extraData, count, RightHand, queueEquip, forceEquip, playSounds, applyNow);
	}
	ActorEquipManager_EquipObject(this__, actor, object, extraData, count, slot, queueEquip, forceEquip, playSounds, applyNow);
}

bool ActorEquipManager_UnequipObject_Hook(ActorEquipManager* this__, Actor* actor, TESBoundObject* object, ExtraDataList* extraData, uint32_t count, const BGSEquipSlot* slot, bool queueEquip, bool forceEquip, bool playSounds, bool applyNow, const BGSEquipSlot* slotToReplace)
{
	ActiveEffect* effect;
	if (slot == nullptr && object->IsWeapon() && static_cast<TESObjectWEAP*>(object)->IsBound() && (effect = GetDualCastBoundWeaponEffect(actor)))
	{
		// Re-equip spell to right hand.
		bool returnValue = ActorEquipManager_UnequipObject(this__, actor, object, extraData, count, slot, queueEquip, forceEquip, playSounds, applyNow, slotToReplace);
		this__->EquipSpell(actor, static_cast<SpellItem*>(effect->spell), RightHand);
		return returnValue;
	}
	return ActorEquipManager_UnequipObject(this__, actor, object, extraData, count, slot, queueEquip, forceEquip, playSounds, applyNow, slotToReplace);
}

void BoundWeapons::Init(SKSE::Trampoline& trampoline)
{
	// ActorEquipManager::EquipObject
	ActorEquipManager_EquipObject = trampoline.write_call<5>(REL::ID(38898).address() + 0x25F, ActorEquipManager_EquipObject_Hook);
	// ActorEquipManager::UnequipObject
	ActorEquipManager_UnequipObject = trampoline.write_call<5>(REL::ID(37975).address() + 0x189, ActorEquipManager_UnequipObject_Hook);
}
void BoundWeapons::UpdateData()
{
	RightHand = static_cast<BGSEquipSlot*>(TESForm::LookupByID(0x013F42));
	//LeftHand = static_cast<BGSEquipSlot*>(TESForm::LookupByID(0x013F43));
	BothHands = static_cast<BGSEquipSlot*>(TESForm::LookupByID(0x013F45));
}