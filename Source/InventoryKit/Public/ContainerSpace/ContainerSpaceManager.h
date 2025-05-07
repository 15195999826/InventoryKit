// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/InventoryKitTypes.h"
#include "ContainerSpaceManager.generated.h"

DEFINE_LOG_CATEGORY_STATIC(LogInventoryKitSpaceManager, Log, All);
/**
 * 容器空间管理器基类
 * 负责不同类型容器的槽位管理逻辑
 */
UCLASS(Abstract)
class INVENTORYKIT_API UContainerSpaceManager : public UObject
{
    GENERATED_BODY()
    
public:
    /**
     * 检查是否可以添加物品到指定槽位
     * 
     * @param SlotIndex 槽位索引
     * @return 是否可以添加
     */
    virtual bool CanAddItemToSlot(int32 SlotIndex) const PURE_VIRTUAL(UContainerSpaceManager::CanAddItemToSlot, return false;);
    
    /**
     * 获取推荐的槽位索引
     * 用于当不指定槽位时自动选择合适的槽位
     * 
     * @return 推荐的槽位索引，如果没有可用槽位则返回-1
     */
    virtual int32 GetRecommendedSlotIndex() const PURE_VIRTUAL(UContainerSpaceManager::GetRecommendedSlotIndex, return -1;);
    
    /**
     * 检查槽位是否可用
     * 
     * @param SlotIndex 槽位索引
     * @return 槽位是否可用
     */
    virtual bool IsSlotAvailable(int32 SlotIndex) const PURE_VIRTUAL(UContainerSpaceManager::IsSlotAvailable, return false;);
    
    /**
     * 初始化空间管理器
     * 
     * @param Config 容器配置
     */
    virtual void Initialize(const FContainerSpaceConfig& Config) PURE_VIRTUAL(UContainerSpaceManager::Initialize, );
    
    /**
     * 获取容器容量
     * 
     * @return 容器容量
     */
    virtual int32 GetCapacity() const PURE_VIRTUAL(UContainerSpaceManager::GetCapacity, return 0;);
    
    /**
     * 检查槽位索引是否有效
     * 
     * @param SlotIndex 槽位索引
     * @return 是否有效
     */
    virtual bool IsValidSlotIndex(int32 SlotIndex) const PURE_VIRTUAL(UContainerSpaceManager::IsValidSlotIndex, return false;);

    /**
     * 根据槽位标签获取槽位索引
     * @param SlotTag 
     * @return 
     */
     UFUNCTION(BlueprintCallable, BlueprintPure, Category="InventoryKit|ContainerSpace")
    virtual int32 GetSlotIndexByTag(const FGameplayTag& SlotTag) const PURE_VIRTUAL(UContainerSpaceManager::GetSlotIndexByTag, return INDEX_NONE;);

    /**
     * 根据坐标获取槽位索引
     * @param X 
     * @param Y 
     * @return 
     */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="InventoryKit|ContainerSpace")
    virtual int32 GetSlotIndexByXY(int32 X, int32 Y) const PURE_VIRTUAL(UContainerSpaceManager::GetSlotIndexByXY, return INDEX_NONE;);

    virtual void UpdateSlotState(int32 SlotIndex, uint8 Flag) PURE_VIRTUAL(UContainerSpaceManager::UpdateSlotState, );
}; 