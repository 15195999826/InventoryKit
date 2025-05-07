// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/InventoryKitBagComponent.h"
#include "Core/InventoryKitItemSystem.h"
#include "Engine/World.h"

UInventoryKitBagComponent::UInventoryKitBagComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    
    // 默认值
    ID = -1;
    Capacity = 20;
    WeightLimit = 100.0f;
    CurrentWeight = 0.0f;
}

void UInventoryKitBagComponent::BeginPlay()
{
    Super::BeginPlay();

    if (UInventoryKitItemSystem* ItemSystem = GetWorld()->GetSubsystem<UInventoryKitItemSystem>())
    {
        // 注册容器
        ID = ItemSystem->RegisterContainer(this);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("ItemSystem not found!"));
    }
}

const int32 UInventoryKitBagComponent::GetContainerID() const
{
    return ID;
}

bool UInventoryKitBagComponent::CanAddItem(int32 ItemId) const
{
    // 检查容量限制
    if (ItemIds.Num() >= Capacity)
    {
        return false;
    }
    
    // 获取物品系统
    UInventoryKitItemSystem* ItemSystem = GetWorld()->GetSubsystem<UInventoryKitItemSystem>();
    if (!ItemSystem)
    {
        return false;
    }
    
    // TODO: 检查重量限制 (需要项目实现)
    // 这里需要扩展物品结构体包含重量信息，或从数据表中查询
    
    return true;
}

void UInventoryKitBagComponent::OnItemAdded(int32 ItemId)
{
    // 如果物品已经在背包中，不重复添加
    if (!ItemIds.Contains(ItemId))
    {
        ItemIds.Add(ItemId);
        
        // TODO: 更新当前重量
        
        // 触发背包变更事件
        OnBagChanged.Broadcast();
    }
}

void UInventoryKitBagComponent::OnItemRemoved(int32 ItemId)
{
    if (ItemIds.Contains(ItemId))
    {
        ItemIds.Remove(ItemId);
        
        // TODO: 更新当前重量
        
        // 触发背包变更事件
        OnBagChanged.Broadcast();
    }
}

const TArray<int32>& UInventoryKitBagComponent::GetAllItems() const
{
    return ItemIds;
}

void UInventoryKitBagComponent::SetCapacity(int32 NewCapacity)
{
    Capacity = NewCapacity;
}

int32 UInventoryKitBagComponent::GetCapacity() const
{
    return Capacity;
}

int32 UInventoryKitBagComponent::GetItemCount() const
{
    return ItemIds.Num();
}

float UInventoryKitBagComponent::GetCurrentWeight() const
{
    return CurrentWeight;
}

void UInventoryKitBagComponent::SetWeightLimit(float NewWeightLimit)
{
    WeightLimit = NewWeightLimit;
}

float UInventoryKitBagComponent::GetWeightLimit() const
{
    return WeightLimit;
}

bool UInventoryKitBagComponent::ContainsItem(int32 ItemId) const
{
    return ItemIds.Contains(ItemId);
} 