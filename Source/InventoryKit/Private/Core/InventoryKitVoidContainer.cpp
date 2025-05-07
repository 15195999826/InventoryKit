// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/InventoryKitVoidContainer.h"

void UInventoryKitVoidContainer::InitContainer()
{
	static FContainerSpaceConfig DefaultConfig{};
	SpaceManager = CreateSpaceManager(this, DefaultConfig);
}

const int32 UInventoryKitVoidContainer::GetContainerID() const
{
	return ID;
}

bool UInventoryKitVoidContainer::CanAddItem(int32 ItemId) const
{
	return true;
}

void UInventoryKitVoidContainer::OnItemAdded(int32 ItemId)
{
	// 如果物品已经在背包中，不重复添加
	if (!ItemIds.Contains(ItemId))
	{
		ItemIds.Add(ItemId);
	}
}

void UInventoryKitVoidContainer::OnItemRemoved(int32 ItemId)
{
	if (ItemIds.Contains(ItemId))
	{
		ItemIds.Remove(ItemId);
	}
}

const TArray<int32>& UInventoryKitVoidContainer::GetAllItems() const
{
	return ItemIds;
}
