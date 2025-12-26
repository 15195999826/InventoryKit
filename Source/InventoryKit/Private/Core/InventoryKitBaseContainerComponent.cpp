// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/InventoryKitBaseContainerComponent.h"

#include "ContainerSpace/ContainerSpaceManager.h"
#include "Core/InventoryKitItemSystem.h"
#include "Engine/World.h"

UInventoryKitBaseContainerComponent::UInventoryKitBaseContainerComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    
    // 默认值
    ID = -1;
}

void UInventoryKitBaseContainerComponent::BeginPlay()
{
    Super::BeginPlay();
    
    if (UInventoryKitItemSystem* ItemSystem = GetWorld()->GetSubsystem<UInventoryKitItemSystem>())
    {
        // 注册容器
        ItemSystem->RegisterContainer(this);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("ItemSystem not found!"));
    }
}

void UInventoryKitBaseContainerComponent::InitContainer(int32 InContainerID)
{
    ID = InContainerID;
    
    SpaceManager = CreateSpaceManager(this, SpaceConfig);
}

const int32 UInventoryKitBaseContainerComponent::GetContainerID() const
{
    return ID;
}

bool UInventoryKitBaseContainerComponent::CanAddItem(const FItemBaseInstance& InItem, int32 DstSlotIndex)
{
    if (SpaceManager->GetCapacity() < 0)
    {
        return true;
    }
    
    // 检查容量限制
    if (ItemIDs.Num() >= SpaceManager->GetCapacity())
    {
        return false;
    }

    // 检查槽位是否可用
    if (!SpaceManager->IsSlotAvailable(DstSlotIndex))
    {
        return false;
    }
    
    // TODO: 检查重量限制
    
    return true;
}

bool UInventoryKitBaseContainerComponent::CanMoveItem(const FItemBaseInstance& InItem, int32 DstSlotIndex)
{
    // 检查槽位是否可用
    if (!SpaceManager->IsSlotAvailable(DstSlotIndex))
    {
        return false;
    }

    // 项目实际使用如果需要判断堆叠等情况， 继承当前组件，然后重写这个方法

    return true;
}

void UInventoryKitBaseContainerComponent::OnItemAdded(const FItemBaseInstance& InItem)
{
    // 如果物品已经在背包中，不重复添加
    if (!ItemIDs.Contains(InItem.ItemID))
    {
        ItemIDs.Add(InItem.ItemID);
        SpaceManager->UpdateSlotState(InItem.ItemLocation.SlotIndex, 1);
        // TODO: 更新当前重量
    }
}

void UInventoryKitBaseContainerComponent::OnItemMoved(const FItemLocation& OldLocation, const FItemBaseInstance& InItem)
{
    SpaceManager->UpdateSlotState(OldLocation.SlotIndex, 0);
    SpaceManager->UpdateSlotState(InItem.ItemLocation.SlotIndex, 1);
}

void UInventoryKitBaseContainerComponent::OnItemRemoved(const FItemBaseInstance& InItem)
{
    if (ItemIDs.Contains(InItem.ItemID))
    {
        ItemIDs.Remove(InItem.ItemID);
        SpaceManager->UpdateSlotState(InItem.ItemLocation.SlotIndex, 0);
        // TODO: 更新当前重量
    }
}

const TArray<int32>& UInventoryKitBaseContainerComponent::GetAllItems() const
{
    return ItemIDs;
}

UContainerSpaceManager* UInventoryKitBaseContainerComponent::GetSpaceManager()
{
    return SpaceManager;
}

bool UInventoryKitBaseContainerComponent::ContainsItem(int32 ItemId) const
{
    return ItemIDs.Contains(ItemId);
} 