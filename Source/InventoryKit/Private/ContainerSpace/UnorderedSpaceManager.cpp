// Copyright Epic Games, Inc. All Rights Reserved.

#include "ContainerSpace/UnorderedSpaceManager.h"

// 构造函数
UUnorderedSpaceManager::UUnorderedSpaceManager()
    : Capacity(-1) // 默认无限容量
    , ItemCount(0)
{
    // 初始化成员变量
}

bool UUnorderedSpaceManager::CanAddItemToSlot(int32 SlotIndex) const
{
    // 无序容器不关心具体槽位，只要容量允许就可以添加
    // 检查是否有容量限制，以及当前物品数量是否小于容量
    return (Capacity < 0 || ItemCount < Capacity);
}

int32 UUnorderedSpaceManager::GetRecommendedSlotIndex() const
{
    // 无序容器不关心具体槽位，返回0表示可以添加
    // 如果容量已满，则返回-1
    if (Capacity >= 0 && ItemCount >= Capacity)
    {
        return INDEX_NONE;
    }
    
    return 0; // 无序容器使用0作为通用槽位索引
}

bool UUnorderedSpaceManager::IsSlotAvailable(int32 SlotIndex) const
{
    // 无序容器只关心总容量，不关心具体槽位
    // 只要还有容量，任何槽位都可用
    // 如果没有容量限制或当前物品数量小于容量，则返回true
    return (Capacity < 0 || ItemCount < Capacity);
}

void UUnorderedSpaceManager::Initialize(const FContainerSpaceConfig& Config)
{
    // 设置容量
    Capacity = Config.Capacity;
    
    // 重置物品计数
    ItemCount = 0;
}

int32 UUnorderedSpaceManager::GetCapacity() const
{
    return Capacity;
}

bool UUnorderedSpaceManager::IsValidSlotIndex(int32 SlotIndex) const
{
    // 无序容器中，只要是非负索引都认为是有效的
    // 通常使用0作为通用槽位索引
    return SlotIndex >= 0;
}

void UUnorderedSpaceManager::SetCapacity(int32 NewCapacity)
{
    Capacity = NewCapacity;
}

void UUnorderedSpaceManager::SetItemCount(int32 NewItemCount)
{
    ItemCount = FMath::Max(0, NewItemCount);
}

int32 UUnorderedSpaceManager::GetSlotIndexByTag(const FGameplayTag& SlotTag) const
{
    // 无序容器不支持槽位标签
    UE_LOG(LogInventoryKitSpaceManager, Error, TEXT("UnorderedSpaceManager does not support slot tags."));
    return INDEX_NONE;
}

int32 UUnorderedSpaceManager::GetSlotIndexByXY(int32 X, int32 Y) const
{
    // 无序容器不支持通过坐标查找槽位
    UE_LOG(LogInventoryKitSpaceManager, Error, TEXT("UnorderedSpaceManager does not support XY coordinates for slot index."));
    return INDEX_NONE;
}

void UUnorderedSpaceManager::UpdateSlotState(int32 SlotIndex, uint8 Flag)
{
    
} 