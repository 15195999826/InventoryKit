// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/InventoryKitVoidContainer.h"

void UInventoryKitVoidContainer::InitContainer(int32 InContainerID)
{
	ID = InContainerID;
	static FContainerSpaceConfig DefaultConfig{};
	SpaceManager = CreateSpaceManager(this, DefaultConfig);
	
	UE_LOG(LogTemp, Warning, TEXT("虚空容器ID为: %d"), ID);
}

const int32 UInventoryKitVoidContainer::GetContainerID() const
{
	return ID;
}

bool UInventoryKitVoidContainer::CanAddItem(const FItemBaseInstance& InItem, int32 DstSlotIndex) const
{
	return true;
}

bool UInventoryKitVoidContainer::CanMoveItem(const FItemBaseInstance& InItem, int32 DstSlotIndex) const
{
	return true;
}

void UInventoryKitVoidContainer::OnItemAdded(const FItemBaseInstance& InItem)
{
	// 如果物品已经在背包中，不重复添加
	if (!ItemIds.Contains(InItem.ItemId))
	{
		ItemIds.Add(InItem.ItemId);
	}
}

void UInventoryKitVoidContainer::OnItemRemoved(const FItemBaseInstance& InItem)
{
	if (ItemIds.Contains(InItem.ItemId))
	{
		ItemIds.Remove(InItem.ItemId);
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
