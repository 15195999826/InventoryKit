// Copyright Epic Games, Inc. All Rights Reserved.

#include "ContainerSpace/FixedSlotSpaceManager.h"

// 构造函数
UFixedSlotSpaceManager::UFixedSlotSpaceManager()
{
    // 初始化成员变量
}

bool UFixedSlotSpaceManager::CanAddItemToSlot(int32 SlotIndex) const
{
    // 检查槽位是否有效且可用
    return IsValidSlotIndex(SlotIndex) && IsSlotAvailable(SlotIndex);
}

int32 UFixedSlotSpaceManager::GetRecommendedSlotIndex() const
{
    UE_LOG(LogInventoryKitSpaceManager, Error, TEXT("GetRecommendedSlotIndex is not implemented for FixedSlotSpaceManager."));
    // 没有可用槽位
    return INDEX_NONE;
}

bool UFixedSlotSpaceManager::IsSlotAvailable(int32 SlotIndex) const
{
    // 检查槽位是否有效且未被占用
    if (!IsValidSlotIndex(SlotIndex))
    {
        return false;
    }
    
    // 检查槽位状态
    const uint8* FlagPtr = SlotFlags.Find(SlotIndex);
    if (FlagPtr)
    {
        // 如果标志为0，表示槽位可用
        return *FlagPtr == 0;
    }
    
    // 如果没有标志记录，默认槽位可用
    return true;
}

void UFixedSlotSpaceManager::Initialize(const FContainerSpaceConfig& Config)
{
    // 清除现有数据
    IndexToSlotTypeMap.Empty();
    SlotTypeToIndexMap.Empty();
    SlotFlags.Empty();
    
    // 添加固定槽位
    for (int32 i = 0; i < Config.FixedSlotTypes.Num(); ++i)
    {
        const FGameplayTag& SlotType = Config.FixedSlotTypes[i];
        IndexToSlotTypeMap.Add(i, SlotType);
        SlotTypeToIndexMap.Add(SlotType, i);
        
        // 初始化槽位状态为可用
        SlotFlags.Add(i, 0);
    }
}

int32 UFixedSlotSpaceManager::GetCapacity() const
{
    // 容量即槽位数量
    return SlotTypeToIndexMap.Num();
}

bool UFixedSlotSpaceManager::IsValidSlotIndex(int32 SlotIndex) const
{
    // 检查槽位索引是否存在于映射中
    return IndexToSlotTypeMap.Contains(SlotIndex);
}

int32 UFixedSlotSpaceManager::GetSlotCount() const
{
    return SlotTypeToIndexMap.Num();
}

int32 UFixedSlotSpaceManager::GetItemAtSlot(int32 SlotIndex) const
{
    const uint8* FlagPtr = SlotFlags.Find(SlotIndex);
    if (FlagPtr && *FlagPtr > 0)
    {
        // 标志大于0表示有物品，返回物品ID
        // 注意：这个实现假设标志值就是物品ID，可能需要根据实际需求修改
        return static_cast<int32>(*FlagPtr);
    }
    
    // 槽位为空
    return -1;
}

bool UFixedSlotSpaceManager::SetItemAtSlot(int32 SlotIndex, int32 ItemId)
{
    // 检查槽位是否有效
    if (!IsValidSlotIndex(SlotIndex))
    {
        return false;
    }
    
    // 检查槽位是否可用
    if (!IsSlotAvailable(SlotIndex) && ItemId > 0)
    {
        return false;
    }
    
    // 设置槽位状态
    if (ItemId > 0)
    {
        // 将物品ID作为标志值保存
        SlotFlags[SlotIndex] = static_cast<uint8>(ItemId);
    }
    else
    {
        // 清除槽位
        SlotFlags[SlotIndex] = 0;
    }
    
    return true;
}

bool UFixedSlotSpaceManager::ClearSlot(int32 SlotIndex)
{
    // 检查槽位是否有效
    if (!IsValidSlotIndex(SlotIndex))
    {
        return false;
    }
    
    // 清除槽位
    SlotFlags[SlotIndex] = 0;
    
    return true;
}

int32 UFixedSlotSpaceManager::GetSlotIndexByType(FGameplayTag SlotType) const
{
    if (SlotTypeToIndexMap.Contains(SlotType))
    {
        return SlotTypeToIndexMap[SlotType];
    }
    
    // 没有找到匹配的槽位
    return INDEX_NONE;
}

FGameplayTag UFixedSlotSpaceManager::GetSlotTypeByIndex(int32 SlotIndex) const
{
    if (IndexToSlotTypeMap.Contains(SlotIndex))
    {
        return IndexToSlotTypeMap[SlotIndex];
    }
    
    // 没有找到匹配的槽位
    UE_LOG(LogInventoryKitSpaceManager, Error, TEXT("GetSlotTypeByIndex: Slot index %d not found."), SlotIndex);
    return FGameplayTag();
}

int32 UFixedSlotSpaceManager::GetSlotIndexByTag(const FGameplayTag& SlotTag) const
{
    // 调用已有的方法查找槽位
    return GetSlotIndexByType(SlotTag);
}

int32 UFixedSlotSpaceManager::GetSlotIndexByXY(int32 X, int32 Y) const
{
    // 固定槽位管理器不支持通过坐标查找槽位
    UE_LOG(LogInventoryKitSpaceManager, Error, TEXT("GetSlotIndexByXY is not supported in FixedSlotSpaceManager."));
    return INDEX_NONE;
}

void UFixedSlotSpaceManager::UpdateSlotState(int32 SlotIndex, uint8 Flag)
{
    // 更新槽位状态
    if (SlotFlags.Contains(SlotIndex))
    {
        SlotFlags[SlotIndex] = Flag;
    }
    else
    {
        UE_LOG(LogInventoryKitSpaceManager, Error, TEXT("Slot index %d not found in FixedSlotSpaceManager."), SlotIndex);
    }
} 