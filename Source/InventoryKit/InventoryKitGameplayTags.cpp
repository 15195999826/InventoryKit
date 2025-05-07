// Copyright Epic Games, Inc. All Rights Reserved.

#include "InventoryKitGameplayTags.h"
#include "Engine/EngineTypes.h"
#include "GameplayTagsManager.h"
#define LOCTEXT_NAMESPACE "InventoryKitGameplayTags"
namespace InventoryKitGameplayTags
{
	
	UE_DEFINE_GAMEPLAY_TAG(Inventory_Container_Empty, "Inventory.Container.Empty");
	UE_DEFINE_GAMEPLAY_TAG(Inventory_Container_Bag, "Inventory.Container.Bag");
	UE_DEFINE_GAMEPLAY_TAG(Inventory_Container_Equipment, "Inventory.Container.Equipment");
	UE_DEFINE_GAMEPLAY_TAG(Inventory_Container_Loot, "Inventory.Container.Loot");
	UE_DEFINE_GAMEPLAY_TAG(Inventory_Container_World, "Inventory.Container.World");
}

#undef LOCTEXT_NAMESPACE

