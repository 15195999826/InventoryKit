// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/InventoryKitVoidContainer.h"

void UInventoryKitVoidContainer::InitContainer(int32 InContainerID)
{
	ID = InContainerID;
	SpaceManager = CreateSpaceManager(this, ContainerSpaceConfig);
}

const int32 UInventoryKitVoidContainer::GetContainerID() const
{
	return ID;
}

bool UInventoryKitVoidContainer::CanAddItem(const FItemBaseInstance& InItem, int32 DstSlotIndex)
{
	return true;
}

bool UInventoryKitVoidContainer::CanMoveItem(const FItemBaseInstance& InItem, int32 DstSlotIndex)
{
	return true;
}

void UInventoryKitVoidContainer::OnItemAdded(const FItemBaseInstance& InItem)
{
	// 如果物品已经在背包中，不重复添加
	if (!ItemIds.Contains(InItem.ItemID))
	{
		ItemIds.Add(InItem.ItemID);
	}
}

void UInventoryKitVoidContainer::OnItemMoved(const FItemLocation& OldLocation, const FItemBaseInstance& InItem)
{
}

void UInventoryKitVoidContainer::OnItemRemoved(const FItemBaseInstance& InItem)
{
	if (ItemIds.Contains(InItem.ItemID))
	{
		ItemIds.Remove(InItem.ItemID);
	}
}

const TArray<int32>& UInventoryKitVoidContainer::GetAllItems() const
{
	return ItemIds;
}

UContainerSpaceManager* UInventoryKitVoidContainer::GetSpaceManager()
{
	return SpaceManager;
}
