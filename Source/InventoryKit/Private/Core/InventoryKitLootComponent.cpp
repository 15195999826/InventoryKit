// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/InventoryKitLootComponent.h"
#include "Core/InventoryKitItemSystem.h"
#include "Engine/World.h"
#include "TimerManager.h"

UInventoryKitLootComponent::UInventoryKitLootComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    
    // 默认值
    bLootGenerated = false;
}

void UInventoryKitLootComponent::BeginPlay()
{
    Super::BeginPlay();
}

const FContainerIdentifier& UInventoryKitLootComponent::GetContainerID() const
{
    return ID;
}

bool UInventoryKitLootComponent::CanAddItem(int32 ItemId) const
{
    // 掉落容器通常不允许直接添加物品
    // 物品是通过生成函数创建的
    return false;
}

void UInventoryKitLootComponent::OnItemAdded(int32 ItemId)
{
    if (!GeneratedItemIds.Contains(ItemId))
    {
        GeneratedItemIds.Add(ItemId);
        OnLootChanged.Broadcast();
    }
}

void UInventoryKitLootComponent::OnItemRemoved(int32 ItemId)
{
    if (GeneratedItemIds.Contains(ItemId))
    {
        GeneratedItemIds.Remove(ItemId);
        OnLootChanged.Broadcast();
        
        // 如果所有物品都被拾取，可以考虑销毁掉落物体
        if (GeneratedItemIds.Num() == 0)
        {
            // TODO: 可选择性地销毁Actor
            // GetOwner()->Destroy();
        }
    }
}

const TArray<int32>& UInventoryKitLootComponent::GetAllItems() const
{
    return GeneratedItemIds;
}

void UInventoryKitLootComponent::SetLootId(int32 NewLootId)
{
    LootId = NewLootId;
}

int32 UInventoryKitLootComponent::GetLootId() const
{
    return LootId;
}

void UInventoryKitLootComponent::AddLootItem(FName ItemConfigId, float DropChance, int32 MinCount, int32 MaxCount)
{
    FLootItem NewItem(ItemConfigId, DropChance, MinCount, MaxCount);
    PredefinedLootItems.Add(NewItem);
}

void UInventoryKitLootComponent::ClearLootItems()
{
    PredefinedLootItems.Empty();
    
    // 如果已经生成了物品，也需要清除
    if (bLootGenerated && GeneratedItemIds.Num() > 0)
    {
        UInventoryKitItemSystem* ItemSystem = GetWorld()->GetSubsystem<UInventoryKitItemSystem>();
        if (ItemSystem)
        {
            // 移动所有物品到"无处"
            FItemLocation NoneLocation(EContainerType::None, -1);
            for (int32 ItemId : GeneratedItemIds)
            {
                FItemLocation CurrentLocation = GetContainerLocation();
                ItemSystem->MoveItem(ItemId, CurrentLocation, NoneLocation);
            }
        }
        
        GeneratedItemIds.Empty();
        bLootGenerated = false;
        OnLootChanged.Broadcast();
    }
}

bool UInventoryKitLootComponent::GenerateLoot(bool bForceRegenerate)
{
    // 如果已经生成过且不强制重新生成，则直接返回
    if (bLootGenerated && !bForceRegenerate)
    {
        return true;
    }
    
    // 如果强制重新生成，先清空现有物品
    if (bForceRegenerate && GeneratedItemIds.Num() > 0)
    {
        ClearLootItems();
    }
    
    // 获取物品系统
    UInventoryKitItemSystem* ItemSystem = GetWorld()->GetSubsystem<UInventoryKitItemSystem>();
    if (!ItemSystem)
    {
        return false;
    }
    
    // 根据预定义的掉落项生成物品
    bool bAnyGenerated = false;
    for (const FLootItem& LootItem : PredefinedLootItems)
    {
        // 根据掉落几率决定是否生成
        float RandomRoll = FMath::FRand();  // 0.0 - 1.0
        if (RandomRoll <= LootItem.DropChance)
        {
            // 决定生成数量
            int32 Count = FMath::RandRange(LootItem.MinCount, LootItem.MaxCount);
            for (int32 i = 0; i < Count; ++i)
            {
                // 创建物品并放入掉落容器
                int32 ItemId = ItemSystem->CreateItem(LootItem.ItemConfigId, GetContainerLocation());
                if (ItemId != -1)
                {
                    bAnyGenerated = true;
                }
            }
        }
    }
    
    bLootGenerated = true;
    
    if (bAnyGenerated)
    {
        OnLootChanged.Broadcast();
    }
    
    return bAnyGenerated;
}

int32 UInventoryKitLootComponent::GetLootItemCount() const
{
    return GeneratedItemIds.Num();
}

bool UInventoryKitLootComponent::TakeAllItems(FItemLocation TargetLocation)
{
    // 获取物品系统
    UInventoryKitItemSystem* ItemSystem = GetWorld()->GetSubsystem<UInventoryKitItemSystem>();
    if (!ItemSystem)
    {
        return false;
    }
    
    // 复制一份物品ID列表，因为移动过程中会修改原始列表
    TArray<int32> ItemsToMove = GeneratedItemIds;
    bool bAllSuccess = true;
    
    for (int32 ItemId : ItemsToMove)
    {
        // 移动物品到目标容器
        FItemLocation CurrentLocation = GetContainerLocation();
        if (!ItemSystem->MoveItem(ItemId, CurrentLocation, TargetLocation))
        {
            bAllSuccess = false;
        }
    }
    
    return bAllSuccess;
}

bool UInventoryKitLootComponent::TakeItem(int32 ItemId, FItemLocation TargetLocation)
{
    // 检查物品是否存在
    if (!GeneratedItemIds.Contains(ItemId))
    {
        return false;
    }
    
    // 获取物品系统
    UInventoryKitItemSystem* ItemSystem = GetWorld()->GetSubsystem<UInventoryKitItemSystem>();
    if (!ItemSystem)
    {
        return false;
    }
    
    // 移动物品到目标容器
    FItemLocation CurrentLocation = GetContainerLocation();
    return ItemSystem->MoveItem(ItemId, CurrentLocation, TargetLocation);
} 