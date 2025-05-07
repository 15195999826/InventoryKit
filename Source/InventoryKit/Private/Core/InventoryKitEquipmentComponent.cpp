// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/InventoryKitEquipmentComponent.h"
#include "Core/InventoryKitItemSystem.h"
#include "Engine/World.h"

UInventoryKitEquipmentComponent::UInventoryKitEquipmentComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryKitEquipmentComponent::BeginPlay()
{
    Super::BeginPlay();
    
    // 如果没有预定义的装备槽，创建默认槽位
    if (EquipmentSlots.Num() == 0)
    {
        InitializeDefaultSlots();
    }
    
    // 初始化时从物品系统同步物品列表
    UInventoryKitItemSystem* ItemSystem = GetWorld()->GetSubsystem<UInventoryKitItemSystem>();
    if (ItemSystem)
    {
        EquippedItemIds = ItemSystem->GetItemsInContainer(GetContainerID());
        
        // 更新槽位中的物品ID
        for (int32 ItemId : EquippedItemIds)
        {
            FItemLocation ItemLocation;
            if (ItemSystem->GetItemLocation(ItemId, ItemLocation))
            {
                for (FBaseEquipmentSlot& Slot : EquipmentSlots)
                {
                    if (ItemLocation.SlotIndex == Slot.SlotIndex)
                    {
                        Slot.EquippedItemId = ItemId;
                        break;
                    }
                }
            }
        }
    }
}

const FContainerIdentifier& UInventoryKitEquipmentComponent::GetContainerID() const
{
    return ID;
}

bool UInventoryKitEquipmentComponent::CanAddItem(int32 ItemId) const
{
    // 获取物品系统
    UInventoryKitItemSystem* ItemSystem = GetWorld()->GetSubsystem<UInventoryKitItemSystem>();
    if (!ItemSystem)
    {
        return false;
    }
    
    // TODO: 检查物品是否可以装备（需要项目实现）
    // 这里应该检查物品类型是否适合装备
    
    return true;
}

void UInventoryKitEquipmentComponent::OnItemAdded(int32 ItemId)
{
    // 这里只更新缓存，装备槽位的实际更新在EquipItem方法中完成
    if (!EquippedItemIds.Contains(ItemId))
    {
        EquippedItemIds.Add(ItemId);
    }
}

void UInventoryKitEquipmentComponent::OnItemRemoved(int32 ItemId)
{
    if (EquippedItemIds.Contains(ItemId))
    {
        EquippedItemIds.Remove(ItemId);
        
        // 同时更新槽位信息
        for (FBaseEquipmentSlot& Slot : EquipmentSlots)
        {
            if (Slot.EquippedItemId == ItemId)
            {
                Slot.EquippedItemId = -1;
                OnEquipmentChanged.Broadcast(Slot.SlotType, Slot.SlotIndex);
                break;
            }
        }
    }
}

const TArray<int32>& UInventoryKitEquipmentComponent::GetAllItems() const
{
    return EquippedItemIds;
}

void UInventoryKitEquipmentComponent::InitializeDefaultSlots()
{
    EquipmentSlots.Empty();
    
    // 创建标准装备槽
    AddEquipmentSlot(EEquipmentSlotType::Head);
    AddEquipmentSlot(EEquipmentSlotType::Body);
    AddEquipmentSlot(EEquipmentSlotType::Hands);
    AddEquipmentSlot(EEquipmentSlotType::Legs);
    AddEquipmentSlot(EEquipmentSlotType::Feet);
    AddEquipmentSlot(EEquipmentSlotType::MainHand);
    AddEquipmentSlot(EEquipmentSlotType::OffHand);
    AddEquipmentSlot(EEquipmentSlotType::Accessory1);
    AddEquipmentSlot(EEquipmentSlotType::Accessory2);
}

int32 UInventoryKitEquipmentComponent::AddEquipmentSlot(EEquipmentSlotType SlotType, FName CustomName)
{
    // 查找同类型槽位的最大索引
    int32 MaxIndex = -1;
    for (const FBaseEquipmentSlot& Slot : EquipmentSlots)
    {
        if (Slot.SlotType == SlotType && Slot.SlotIndex > MaxIndex)
        {
            MaxIndex = Slot.SlotIndex;
        }
    }
    
    // 创建新槽位
    FBaseEquipmentSlot NewSlot;
    NewSlot.SlotType = SlotType;
    NewSlot.SlotIndex = MaxIndex + 1;
    NewSlot.EquippedItemId = -1;
    NewSlot.bEnabled = true;
    
    if (SlotType == EEquipmentSlotType::Custom)
    {
        NewSlot.CustomSlotName = CustomName;
    }
    
    EquipmentSlots.Add(NewSlot);
    
    return NewSlot.SlotIndex;
}

int32 UInventoryKitEquipmentComponent::GetEquipmentSlotCount() const
{
    return EquipmentSlots.Num();
}

bool UInventoryKitEquipmentComponent::GetEquipmentSlotByType(EEquipmentSlotType SlotType, FBaseEquipmentSlot& OutSlot) const
{
    for (const FBaseEquipmentSlot& Slot : EquipmentSlots)
    {
        if (Slot.SlotType == SlotType)
        {
            OutSlot = Slot;
            return true;
        }
    }
    
    return false;
}

bool UInventoryKitEquipmentComponent::EquipItem(int32 ItemId, EEquipmentSlotType SlotType, int32 SlotIndex)
{
    // 获取物品系统
    UInventoryKitItemSystem* ItemSystem = GetWorld()->GetSubsystem<UInventoryKitItemSystem>();
    if (!ItemSystem)
    {
        return false;
    }
    
    // 查找目标槽位
    FBaseEquipmentSlot* TargetSlot = nullptr;
    for (FBaseEquipmentSlot& Slot : EquipmentSlots)
    {
        if (Slot.SlotType == SlotType && Slot.SlotIndex == SlotIndex)
        {
            TargetSlot = &Slot;
            break;
        }
    }
    
    if (!TargetSlot)
    {
        // 槽位不存在
        return false;
    }
    
    if (!TargetSlot->bEnabled)
    {
        // 槽位禁用
        return false;
    }
    
    // 获取物品当前位置
    FItemLocation CurrentLocation;
    if (!ItemSystem->GetItemLocation(ItemId, CurrentLocation))
    {
        // 物品不存在
        return false;
    }
    
    // 准备新位置
    FItemLocation TargetLocation = GetContainerLocation();
    TargetLocation.SlotIndex = TargetSlot->SlotIndex;
    
    // 如果槽位已有物品，先卸下
    if (TargetSlot->EquippedItemId != -1)
    {
        // 这里假设将卸下的物品移动到无处（相当于临时存储）
        // 实际项目中可能需要将物品移动到背包
        FItemLocation TempLocation(EContainerType::None, -1);
        ItemSystem->MoveItem(TargetSlot->EquippedItemId, TargetLocation, TempLocation);
    }
    
    // 移动物品到装备槽
    bool bSuccess = ItemSystem->MoveItem(ItemId, CurrentLocation, TargetLocation);
    if (bSuccess)
    {
        // 更新槽位信息
        TargetSlot->EquippedItemId = ItemId;
        
        // 触发装备变更事件
        OnEquipmentChanged.Broadcast(TargetSlot->SlotType, TargetSlot->SlotIndex);
    }
    
    return bSuccess;
}

bool UInventoryKitEquipmentComponent::UnequipItem(EEquipmentSlotType SlotType, int32 SlotIndex)
{
    // 查找目标槽位
    FBaseEquipmentSlot* TargetSlot = nullptr;
    for (FBaseEquipmentSlot& Slot : EquipmentSlots)
    {
        if (Slot.SlotType == SlotType && Slot.SlotIndex == SlotIndex)
        {
            TargetSlot = &Slot;
            break;
        }
    }
    
    if (!TargetSlot || TargetSlot->EquippedItemId == -1)
    {
        // 槽位不存在或没有装备物品
        return false;
    }
    
    int32 ItemId = TargetSlot->EquippedItemId;
    
    // 获取物品系统
    UInventoryKitItemSystem* ItemSystem = GetWorld()->GetSubsystem<UInventoryKitItemSystem>();
    if (!ItemSystem)
    {
        return false;
    }
    
    // 获取物品当前位置
    FItemLocation CurrentLocation = GetContainerLocation();
    CurrentLocation.SlotIndex = TargetSlot->SlotIndex;
    
    // 创建"无处"位置表示物品暂时不在任何容器中
    // 实际项目中应该移动到背包或其他合适位置
    FItemLocation NoneLocation(EContainerType::None, -1);
    
    // 移动物品
    bool bSuccess = ItemSystem->MoveItem(ItemId, CurrentLocation, NoneLocation);
    if (bSuccess)
    {
        // 更新槽位信息
        TargetSlot->EquippedItemId = -1;
        
        // 触发装备变更事件
        OnEquipmentChanged.Broadcast(TargetSlot->SlotType, TargetSlot->SlotIndex);
    }
    
    return bSuccess;
}

int32 UInventoryKitEquipmentComponent::GetEquippedItemId(EEquipmentSlotType SlotType, int32 SlotIndex) const
{
    for (const FBaseEquipmentSlot& Slot : EquipmentSlots)
    {
        if (Slot.SlotType == SlotType && Slot.SlotIndex == SlotIndex)
        {
            return Slot.EquippedItemId;
        }
    }
    
    return -1;
}

bool UInventoryKitEquipmentComponent::IsItemEquipped(int32 ItemId) const
{
    return EquippedItemIds.Contains(ItemId);
}