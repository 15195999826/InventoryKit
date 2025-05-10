// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ContainerSpace/ContainerSpaceManager.h"
#include "FixedSlotSpaceManager.generated.h"

/**
 * 固定槽位容器空间管理器
 * 用于管理具有特定槽位类型的容器
 *
 * 项目中映射到UI的思路: 初始化时, Slot UI上携带一个FGameplayTag, 然后当拖拽物品到这个Slot UI上时，通过SpaceManager查询到这个Slot的Index， 然后调用ItemSystem的MoveItem 
 */
UCLASS()
class INVENTORYKIT_API UFixedSlotSpaceManager : public UContainerSpaceManager
{
    GENERATED_BODY()
    
private:
    // Index to 槽位
    UPROPERTY()
    TMap<int32, FGameplayTag> IndexToSlotTypeMap;
    
    UPROPERTY()
    TMap<FGameplayTag, int32> SlotTypeToIndexMap;

    /**
     * 表示槽位状态
     * 只通过持有者的Add or Remove 函数进行更新
     */
    UPROPERTY()
    TMap<int32, uint8> SlotFlags;
    
public:
    // 构造函数
    UFixedSlotSpaceManager();
    
    //~ Begin UContainerSpaceManager Interface
    virtual bool CanAddItemToSlot(int32 SlotIndex) const override;
    virtual int32 GetRecommendedSlotIndex() const override;
    virtual bool IsSlotAvailable(int32 SlotIndex) const override;
    virtual void Initialize(const FContainerSpaceConfig& Config) override;
    virtual int32 GetCapacity() const override;
    virtual bool IsValidSlotIndex(int32 SlotIndex) const override;
    virtual int32 GetSlotIndexByTag(const FGameplayTag& SlotTag) const override;
    virtual int32 GetSlotIndexByXY(int32 X, int32 Y) const override;
    virtual void UpdateSlotState(int32 SlotIndex, uint8 Flag) override;
    //~ End UContainerSpaceManager Interface
    
    /**
     * 获取槽位数量
     * 
     * @return 槽位数量
     */
    int32 GetSlotCount() const;
    
    /**
     * 获取槽位中的物品ID
     * 
     * @param SlotIndex 槽位索引
     * @return 物品ID，如果槽位为空则返回-1
     */
    int32 GetItemAtSlot(int32 SlotIndex) const;
    
    /**
     * 设置槽位中的物品
     * 
     * @param SlotIndex 槽位索引
     * @param ItemId 物品ID
     * @return 是否设置成功
     */
    bool SetItemAtSlot(int32 SlotIndex, int32 ItemId);
    
    /**
     * 清除槽位中的物品
     * 
     * @param SlotIndex 槽位索引
     * @return 是否清除成功
     */
    bool ClearSlot(int32 SlotIndex);
    
    /**
     * 获取指定类型的槽位索引
     * 
     * @param SlotType 槽位类型
     * @return 槽位索引，如果未找到则返回-1
     */
    int32 GetSlotIndexByType(FGameplayTag SlotType) const;

    FGameplayTag GetSlotTypeByIndex(int32 SlotIndex) const;
}; 