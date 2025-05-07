// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ContainerSpace/ContainerSpaceManager.h"
#include "UnorderedSpaceManager.generated.h"

/**
 * 无序容器空间管理器
 * 用于管理不关心物品具体位置的容器
 */
UCLASS()
class INVENTORYKIT_API UUnorderedSpaceManager : public UContainerSpaceManager
{
    GENERATED_BODY()
    
private:
    // 容量限制
    int32 Capacity;
    
    // 当前使用的物品数量（由容器维护）
    int32 ItemCount;
    
public:
    // 构造函数
    UUnorderedSpaceManager();
    
    //~ Begin UContainerSpaceManager Interface
    virtual bool CanAddItemToSlot(int32 SlotIndex) const override;
    virtual int32 GetRecommendedSlotIndex() const override;
    virtual bool IsSlotAvailable(int32 SlotIndex) const override;
    virtual void Initialize(const FContainerSpaceConfig& Config) override;
    virtual int32 GetCapacity() const override;
    virtual bool IsValidSlotIndex(int32 SlotIndex) const override;
    //~ End UContainerSpaceManager Interface
    
    /**
     * 设置容量
     * 
     * @param NewCapacity 新的容量
     */
    void SetCapacity(int32 NewCapacity);
    
    /**
     * 设置当前物品数量
     * 
     * @param NewItemCount 新的物品数量
     */
    void SetItemCount(int32 NewItemCount);
}; 