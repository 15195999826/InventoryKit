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
        ID = ItemSystem->RegisterContainer(this);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("ItemSystem not found!"));
    }
}

void UInventoryKitBaseContainerComponent::InitContainer()
{
    SpaceManager = CreateSpaceManager(this, SpaceConfig);
}

const int32 UInventoryKitBaseContainerComponent::GetContainerID() const
{
    return ID;
}

bool UInventoryKitBaseContainerComponent::CanAddItem(int32 ItemId) const
{
    // 检查容量限制
    if (ItemIds.Num() >= SpaceManager->GetCapacity())
    {
        return false;
    }
    
    // TODO: 检查重量限制
    
    return true;
}

void UInventoryKitBaseContainerComponent::OnItemAdded(int32 ItemId)
{
    // 如果物品已经在背包中，不重复添加
    if (!ItemIds.Contains(ItemId))
    {
        ItemIds.Add(ItemId);
        
        // TODO: 更新当前重量
        
        // 触发背包变更事件
        OnContainerChanged.Broadcast();
    }
}

void UInventoryKitBaseContainerComponent::OnItemRemoved(int32 ItemId)
{
    if (ItemIds.Contains(ItemId))
    {
        ItemIds.Remove(ItemId);
        SpaceManager->
        // TODO: 更新当前重量
        
        // 触发背包变更事件
        OnContainerChanged.Broadcast();
    }
}

const TArray<int32>& UInventoryKitBaseContainerComponent::GetAllItems() const
{
    return ItemIds;
}

UContainerSpaceManager* UInventoryKitBaseContainerComponent::GetSpaceManager()
{
    return SpaceManager;
}

bool UInventoryKitBaseContainerComponent::ContainsItem(int32 ItemId) const
{
    return ItemIds.Contains(ItemId);
} 