// Copyright Epic Games, Inc. All Rights Reserved.

#include "ContainerSpace/GridSpaceManager.h"

// 构造函数
UGridSpaceManager::UGridSpaceManager()
    : GridWidth(0)
    , GridHeight(0)
{
    // 初始化成员变量
}

bool UGridSpaceManager::CanAddItemToSlot(int32 SlotIndex) const
{
    // 检查槽位是否有效且可用
    return IsValidSlotIndex(SlotIndex) && IsSlotAvailable(SlotIndex);
}

int32 UGridSpaceManager::GetRecommendedSlotIndex() const
{
    // 寻找第一个可用的槽位
    for (int32 Index = 0; Index < SlotFlags.Num(); ++Index)
    {
        if (SlotFlags[Index] == 0) // 0表示可用
        {
            return Index;
        }
    }
    
    // 没有可用槽位
    return INDEX_NONE;
}

bool UGridSpaceManager::IsSlotAvailable(int32 SlotIndex) const
{
    // 检查槽位是否有效且未被占用
    if (!IsValidSlotIndex(SlotIndex))
    {
        return false;
    }
    
    // 检查槽位状态，0表示可用
    return SlotFlags[SlotIndex] == 0;
}

void UGridSpaceManager::Initialize(const FContainerSpaceConfig& Config)
{
    // 设置网格尺寸
    GridWidth = FMath::Max(1, Config.GridWidth);
    GridHeight = FMath::Max(1, Config.GridHeight);
    
    // 初始化槽位状态数组
    const int32 TotalSlots = GridWidth * GridHeight;
    SlotFlags.Init(0, TotalSlots); // 所有槽位初始化为可用状态(0)
}

int32 UGridSpaceManager::GetCapacity() const
{
    // 容量为网格总槽位数
    return GridWidth * GridHeight;
}

bool UGridSpaceManager::IsValidSlotIndex(int32 SlotIndex) const
{
    // 检查索引是否在有效范围内
    return SlotIndex >= 0 && SlotIndex < SlotFlags.Num();
}

void UGridSpaceManager::GetGridSize(int32& OutWidth, int32& OutHeight) const
{
    OutWidth = GridWidth;
    OutHeight = GridHeight;
}

int32 UGridSpaceManager::CoordinateToIndex(int32 X, int32 Y) const
{
    // 检查坐标是否在有效范围内
    if (X < 0 || X >= GridWidth || Y < 0 || Y >= GridHeight)
    {
        return INDEX_NONE;
    }
    
    // 转换为一维索引
    return Y * GridWidth + X;
}

void UGridSpaceManager::IndexToCoordinate(int32 Index, int32& OutX, int32& OutY) const
{
    // 检查索引是否有效
    if (!IsValidSlotIndex(Index))
    {
        OutX = -1;
        OutY = -1;
        return;
    }
    
    // 转换为二维坐标
    OutX = Index % GridWidth;
    OutY = Index / GridWidth;
}

int32 UGridSpaceManager::GetSlotIndexByXY(int32 X, int32 Y) const
{
    // 直接转换坐标为索引
    return CoordinateToIndex(X, Y);
}

void UGridSpaceManager::UpdateSlotState(int32 SlotIndex, uint8 Flag)
{
    // 检查索引是否有效
    if (IsValidSlotIndex(SlotIndex))
    {
        SlotFlags[SlotIndex] = Flag;
    }
}

int32 UGridSpaceManager::GetSlotIndexByTag(const FGameplayTag& SlotTag) const
{
    // 网格容器不支持通过标签查找槽位
    UE_LOG(LogInventoryKitSpaceManager, Error, TEXT("GetSlotIndexByTag is not supported in GridSpaceManager."));
    return INDEX_NONE;
}
