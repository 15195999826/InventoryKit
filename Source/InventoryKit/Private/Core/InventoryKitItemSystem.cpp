// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/InventoryKitItemSystem.h"

#include "Core/InventoryKitVoidContainer.h"

UInventoryKitItemSystem::UInventoryKitItemSystem()
    : NextItemID(1)
{
}

void UInventoryKitItemSystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    ItemLocationMap.Empty();
    ItemMap.Empty();
    
    auto NewVoidContainer = NewObject<UInventoryKitVoidContainer>(this);
    VoidContainerID = RegisterContainer(NewVoidContainer);
    NewVoidContainer->SetContainerID(VoidContainerID);
}

void UInventoryKitItemSystem::Deinitialize()
{
    ItemLocationMap.Empty();
    ItemMap.Empty();
    Super::Deinitialize();
}

bool UInventoryKitItemSystem::GetItemLocation(int32 ItemId, FItemLocation& OutLocation) const
{
    if (const FItemLocation* Location = ItemLocationMap.Find(ItemId))
    {
        OutLocation = *Location;
        return true;
    }
    return false;
}

bool UInventoryKitItemSystem::MoveItem(int32 ItemId, const FItemLocation& TargetLocation)
{
    // 验证物品当前位置
    FItemLocation CurrentLocation;
    if (!GetItemLocation(ItemId, CurrentLocation))
    {
        UE_LOG(LogTemp, Error, TEXT("Item %d not found!"), ItemId);
        return false;
    }

    // 更新位置
    ItemLocationMap[ItemId] = TargetLocation;

    // 触发位置变更事件
    if (ContainerMap.Contains(CurrentLocation.ContainerID))
    {
        ContainerMap[CurrentLocation.ContainerID]->OnItemRemoved(ItemId);
    }

    if (ContainerMap.Contains(TargetLocation.ContainerID))
    {
        ContainerMap[TargetLocation.ContainerID]->OnItemAdded(ItemId);
    }
    return true;
}

int32 UInventoryKitItemSystem::CreateItem(FName ConfigId, FItemLocation Location)
{
    // 生成新的物品ID
    int32 NewItemId = NextItemID++;

    // 创建物品实例
    FItemBaseInstance NewItem;
    NewItem.ItemId = NewItemId;
    NewItem.ConfigRowName = ConfigId;

    // 添加到映射表
    ItemMap.Add(NewItemId, NewItem);
    ItemLocationMap.Add(NewItemId, Location);

    if (ContainerMap.Contains(Location.ContainerID))
    {
        ContainerMap[Location.ContainerID]->OnItemAdded(NewItemId);
    }

    return NewItemId;
}

TArray<int32> UInventoryKitItemSystem::GetItemsInContainer(int32 Identifier) const
{
    TArray<int32> Result;
    for (const auto& Pair : ItemLocationMap)
    {
        if (Pair.Value.ContainerID == Identifier)
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
    return ID;
}

void UInventoryKitItemSystem::UnregisterContainer(IInventoryKitContainerInterface* InContainer)
{
    auto ID = InContainer->GetContainerID();
    check(ContainerMap.Contains(ID));
    ContainerMap.Remove(ID);
} 