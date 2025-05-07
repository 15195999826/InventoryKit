// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/InventoryKitBagComponent.h"
#include "Core/InventoryKitItemSystem.h"
#include "Engine/World.h"

UInventoryKitBagComponent::UInventoryKitBagComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    
    // 默认值
    ID = -1;
    GridWidth = 5;
    GridHeight = 4;
    Capacity = GridWidth * GridHeight;
    WeightLimit = 100.0f;
    CurrentWeight = 0.0f;
}

void UInventoryKitBagComponent::BeginPlay()
{
    Super::BeginPlay();

    // 初始化网格
    InitializeGrid();

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

void UInventoryKitBagComponent::InitializeGrid()
{
    // 确保网格尺寸有效
    GridWidth = FMath::Max(1, GridWidth);
    GridHeight = FMath::Max(1, GridHeight);
    
    // 更新容量
    Capacity = GridWidth * GridHeight;
    
    // 初始化槽位状态数组
    SlotFlags.SetNumZeroed(Capacity);
}

void UInventoryKitBagComponent::SetGridSize(int32 Width, int32 Height)
{
    // 确保尺寸有效
    Width = FMath::Max(1, Width);
    Height = FMath::Max(1, Height);
    
    // 如果尺寸没变，直接返回
    if (Width == GridWidth && Height == GridHeight)
    {
        return;
    }
    
    // 设置新尺寸
    GridWidth = Width;
    GridHeight = Height;
    
    // 更新容量
    Capacity = GridWidth * GridHeight;
    
    // 重新初始化槽位状态
    SlotFlags.SetNumZeroed(Capacity);
    
    // 触发背包变更事件
    OnBagChanged.Broadcast();
}

void UInventoryKitBagComponent::GetGridSize(int32& OutWidth, int32& OutHeight) const
{
    OutWidth = GridWidth;
    OutHeight = GridHeight;
}

int32 UInventoryKitBagComponent::CoordinateToIndex(int32 X, int32 Y) const
{
    // 检查坐标是否在网格范围内
    if (X < 0 || X >= GridWidth || Y < 0 || Y >= GridHeight)
    {
        return -1;
    }
    
    // 计算一维索引
    return Y * GridWidth + X;
}

void UInventoryKitBagComponent::IndexToCoordinate(int32 Index, int32& OutX, int32& OutY) const
{
    // 检查索引是否有效
    if (Index < 0 || Index >= GridWidth * GridHeight)
    {
        OutX = -1;
        OutY = -1;
        return;
    }
    
    // 计算坐标
    OutX = Index % GridWidth;
    OutY = Index / GridWidth;
}

bool UInventoryKitBagComponent::IsSlotAvailable(int32 SlotIndex) const
{
    // 检查索引是否有效
    if (SlotIndex < 0 || SlotIndex >= SlotFlags.Num())
    {
        return false;
    }
    
    // 检查槽位是否被锁定
    if (SlotFlags[SlotIndex] & 1) // 第一位表示锁定状态
    {
        return false;
    }
    
    // 检查槽位是否已有物品
    return GetItemAtSlot(SlotIndex) == -1;
}

int32 UInventoryKitBagComponent::FindAvailableSlot() const
{
    // 遍历所有槽位寻找可用的
    for (int32 i = 0; i < SlotFlags.Num(); ++i)
    {
        if (IsSlotAvailable(i))
        {
            return i;
        }
    }
    
    // 没有找到可用槽位
    return -1;
}

int32 UInventoryKitBagComponent::GetItemAtSlot(int32 SlotIndex) const
{
    // 检查索引是否有效
    if (SlotIndex < 0 || SlotIndex >= GridWidth * GridHeight)
    {
        return -1;
    }
    
    // 获取物品系统
    UInventoryKitItemSystem* ItemSystem = GetWorld()->GetSubsystem<UInventoryKitItemSystem>();
    if (!ItemSystem)
    {
        return -1;
    }
    
    // 查找该槽位中的物品
    for (int32 ItemId : ItemIds)
    {
        FItemLocation Location;
        if (ItemSystem->GetItemLocation(ItemId, Location) && 
            Location.ContainerID == ID && 
            Location.SlotIndex == SlotIndex)
        {
            return ItemId;
        }
    }
    
    // 槽位中没有物品
    return -1;
}

int32 UInventoryKitBagComponent::GetItemSlotIndex(int32 ItemId) const
{
    // 检查物品是否在背包中
    if (!ItemIds.Contains(ItemId))
    {
        return -1;
    }
    
    // 获取物品系统
    UInventoryKitItemSystem* ItemSystem = GetWorld()->GetSubsystem<UInventoryKitItemSystem>();
    if (!ItemSystem)
    {
        return -1;
    }
    
    // 获取物品位置
    FItemLocation Location;
    if (ItemSystem->GetItemLocation(ItemId, Location) && Location.ContainerID == ID)
    {
        return Location.SlotIndex;
    }
    
    return -1;
}

void UInventoryKitBagComponent::SetSlotLocked(int32 SlotIndex, bool bLocked)
{
    // 检查索引是否有效
    if (SlotIndex < 0 || SlotIndex >= SlotFlags.Num())
    {
        return;
    }
    
    // 设置锁定状态
    if (bLocked)
    {
        SlotFlags[SlotIndex] |= 1; // 设置第一位为1
    }
    else
    {
        SlotFlags[SlotIndex] &= ~1; // 设置第一位为0
    }
    
    // 触发背包变更事件
    OnBagChanged.Broadcast();
}

void UInventoryKitBagComponent::SetCapacity(int32 NewCapacity)
{
    // 为保持向后兼容性，我们使用一维网格来实现旧的容量设置
    SetGridSize(NewCapacity, 1);
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