// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/InventoryKitItemSystem.h"

#include "Core/InventoryKitVoidContainer.h"


void UInventoryKitItemSystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    ItemMap.Empty();
    auto NewVoidContainer = NewObject<UInventoryKitVoidContainer>(this);
    VoidContainerID = RegisterContainer(NewVoidContainer);
    NewVoidContainer->SetContainerID(VoidContainerID);
}

void UInventoryKitItemSystem::Deinitialize()
{
    ItemMap.Empty();
    ContainerMap.Empty();
    Super::Deinitialize();
}

bool UInventoryKitItemSystem::GetItemLocation(int32 ItemId, FItemLocation& OutLocation) const
{
    if (const FItemBaseInstance* Location = ItemMap.Find(ItemId))
    {
        OutLocation = Location->ItemLocation;
        return true;
    }
    return false;
}

bool UInventoryKitItemSystem::MoveItem(int32 ItemId, const FItemLocation& TargetLocation)
{
    if (!ItemMap.Contains(ItemId))
    {
        UE_LOG(LogInventoryKitSystem, Error, TEXT("Item %d not found!"), ItemId);
        return false;
    }
    
    // 验证物品当前位置
    FItemBaseInstance CopyOldItem = ItemMap[ItemId];
    if (!ContainerMap.Contains(TargetLocation.ContainerID))
    {
        UE_LOG(LogInventoryKitSystem, Error, TEXT("Target container %d not found!"), TargetLocation.ContainerID);
        return false;
    }
    
    auto TargetContainer = ContainerMap[TargetLocation.ContainerID];
    if (CopyOldItem.ItemLocation.ContainerID != TargetLocation.ContainerID && !TargetContainer->CanAddItem(CopyOldItem, TargetLocation.SlotIndex))
    {
        UE_LOG(LogInventoryKitSystem, Warning, TEXT("Cannot move item %d to container %d!"), ItemId, TargetLocation.ContainerID);
        return false;
    }

    if (CopyOldItem.ItemLocation.ContainerID == TargetLocation.ContainerID && !TargetContainer->CanMoveItem(CopyOldItem, TargetLocation.SlotIndex))
    {
        UE_LOG(LogInventoryKitSystem, Warning, TEXT("Cannot move item %d to container %d!"), ItemId, TargetLocation.ContainerID);
        return false;
    }
    
    // 更新位置
    ItemMap[ItemId].ItemLocation = TargetLocation;

    // 触发位置变更事件
    if (ContainerMap.Contains(CopyOldItem.ItemLocation.ContainerID))
    {
        ContainerMap[CopyOldItem.ItemLocation.ContainerID]->OnItemRemoved(CopyOldItem);
    }

    TargetContainer->OnItemAdded(ItemMap[ItemId]);
    return true;
}

int32 UInventoryKitItemSystem::CreateItem(FName ConfigId, const FItemLocation& Location)
{
    // 生成新的物品ID
    int32 NewItemId = NextItemID++;

    // 创建物品实例
    FItemBaseInstance NewItem;
    NewItem.ItemId = NewItemId;
    NewItem.ConfigRowName = ConfigId;
    NewItem.ItemLocation = Location;

    // 添加到映射表
    ItemMap.Add(NewItemId, NewItem);

    if (ContainerMap.Contains(Location.ContainerID))
    {
        ContainerMap[Location.ContainerID]->OnItemAdded(NewItem);
    }

    return NewItemId;
}

TArray<int32> UInventoryKitItemSystem::GetItemsInContainer(int32 Identifier) const
{
    TArray<int32> Result;
    for (const auto& Pair : ItemMap)
    {
        if (Pair.Value.ItemLocation.ContainerID == Identifier)
        {
            Result.Add(Pair.Key);
        }
    }
    return Result;
}

FItemBaseInstance UInventoryKitItemSystem::GetItemData(int32 ItemId) const
{
    if (const FItemBaseInstance* Item = ItemMap.Find(ItemId))
    {
        return *Item;
    }
    
    return FItemBaseInstance();
}

int32 UInventoryKitItemSystem::RegisterContainer(IInventoryKitContainerInterface* InContainer)
{
    auto ID = NextContainerID++;
    ContainerMap.Add(ID, InContainer);
    InContainer->InitContainer();
    return ID;
}

void UInventoryKitItemSystem::UnregisterContainer(IInventoryKitContainerInterface* InContainer)
{
    auto ID = InContainer->GetContainerID();
    check(ContainerMap.Contains(ID));
    ContainerMap.Remove(ID);
} 