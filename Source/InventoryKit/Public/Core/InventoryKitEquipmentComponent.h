// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/ContainerInterfaces.h"
#include "Core/InventoryKitTypes.h"
#include "InventoryKitEquipmentComponent.generated.h"

// 装备变更事件委托
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEquipmentChanged, FGameplayTag, SlotType, int32, SlotIndex);

/**
 * 装备组件
 * 用于管理角色装备的物品，实现物品容器接口
 */
UCLASS(ClassGroup=(InventoryKit), meta=(BlueprintSpawnableComponent))
class INVENTORYKIT_API UInventoryKitEquipmentComponent : public UActorComponent,
                                                       public IInventoryKitContainerInterface
{
    GENERATED_BODY()

public:    
    // 构造函数
    UInventoryKitEquipmentComponent();

protected:
    // 组件初始化
    virtual void BeginPlay() override;
    
    // 装备组件唯一ID
    UPROPERTY(EditAnywhere, Category = "InventoryKit")
    FContainerIdentifier Identifier;
    
    // 装备槽数组
    UPROPERTY(EditAnywhere, Category = "InventoryKit")
    TArray<FBaseEquipmentSlot> EquipmentSlots;
    
    // 物品ID缓存
    UPROPERTY()
    TArray<int32> EquippedItemIds;

public:
    //~ Begin IInventoryKitContainerInterface
    virtual const FContainerIdentifier& GetContainerID() const override;
    virtual bool CanAddItem(int32 ItemId) const override;
    virtual void OnItemAdded(int32 ItemId) override;
    virtual void OnItemRemoved(int32 ItemId) override;
    virtual const TArray<int32>& GetAllItems() const override;
    //~ End IInventoryKitContainerInterface
    
    /**
     * 初始化装备槽
     * 创建默认的装备槽配置
     */
    UFUNCTION(BlueprintCallable, Category = "InventoryKit")
    void InitializeDefaultSlots();
    
    /**
     * 添加装备槽
     * @param SlotType 槽位类型
     * @param CustomName 自定义槽位名称（仅在SlotType为Custom时使用）
     * @return 创建的槽位索引
     */
    UFUNCTION(BlueprintCallable, Category = "InventoryKit")
    int32 AddEquipmentSlot(EEquipmentSlotType SlotType, FName CustomName = NAME_None);
    
    /**
     * 获取装备槽数量
     * @return 装备槽数量
     */
    UFUNCTION(BlueprintCallable, Category = "InventoryKit")
    int32 GetEquipmentSlotCount() const;
    
    /**
     * 获取特定类型的装备槽
     * @param SlotType 要查找的槽位类型
     * @param OutSlot 输出参数，找到的槽位
     * @return 是否找到
     */
    UFUNCTION(BlueprintCallable, Category = "InventoryKit")
    bool GetEquipmentSlotByType(EEquipmentSlotType SlotType, FBaseEquipmentSlot& OutSlot) const;
    
    /**
     * 装备物品到指定槽位
     * @param ItemId 要装备的物品ID
     * @param SlotType 槽位类型
     * @param SlotIndex 槽位索引
     * @return 是否装备成功
     */
    UFUNCTION(BlueprintCallable, Category = "InventoryKit")
    bool EquipItem(int32 ItemId, EEquipmentSlotType SlotType, int32 SlotIndex = 0);
    
    /**
     * 卸下指定槽位的物品
     * @param SlotType 槽位类型
     * @param SlotIndex 槽位索引
     * @return 是否卸下成功
     */
    UFUNCTION(BlueprintCallable, Category = "InventoryKit")
    bool UnequipItem(EEquipmentSlotType SlotType, int32 SlotIndex = 0);
    
    /**
     * 获取装备在指定槽位的物品ID
     * @param SlotType 槽位类型
     * @param SlotIndex 槽位索引
     * @return 物品ID，未装备则返回-1
     */
    UFUNCTION(BlueprintCallable, Category = "InventoryKit")
    int32 GetEquippedItemId(EEquipmentSlotType SlotType, int32 SlotIndex = 0) const;
    
    /**
     * 检查指定物品是否已装备
     * @param ItemId 要检查的物品ID
     * @return 是否已装备
     */
    UFUNCTION(BlueprintCallable, Category = "InventoryKit")
    bool IsItemEquipped(int32 ItemId) const;
    
    // 装备变更事件
    UPROPERTY(BlueprintAssignable, Category = "InventoryKit")
    FOnEquipmentChanged OnEquipmentChanged;
}; 