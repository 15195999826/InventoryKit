// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/ContainerInterfaces.h"
#include "Core/InventoryKitTypes.h"
#include "InventoryKitBagComponent.generated.h"

// 背包变更事件委托
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBagChanged);

/**
 * 背包组件
 * 用于管理角色携带的物品，实现物品容器接口
 */
UCLASS(ClassGroup=(InventoryKit), meta=(BlueprintSpawnableComponent))
class INVENTORYKIT_API UInventoryKitBagComponent : public UActorComponent, 
                                                  public IInventoryKitContainerInterface
{
    GENERATED_BODY()

public:    
    // 构造函数
    UInventoryKitBagComponent();

protected:
    // 组件初始化
    virtual void BeginPlay() override;
    
    // 背包唯一标识
    UPROPERTY(EditAnywhere, Category = "InventoryKit")
    int32 ID;
    
    // 物品ID缓存
    UPROPERTY()
    TArray<int32> ItemIds;
    
    // 容量限制
    UPROPERTY(EditAnywhere, Category = "InventoryKit")
    int32 Capacity;
    
    // 重量限制
    UPROPERTY(EditAnywhere, Category = "InventoryKit")
    float WeightLimit;
    
    // 当前重量
    UPROPERTY(VisibleAnywhere, Category = "InventoryKit")
    float CurrentWeight;

public:
    //~ Begin IInventoryKitContainerInterface
    virtual const int32 GetContainerID() const override;
    virtual bool CanAddItem(int32 ItemId) const override;
    virtual void OnItemAdded(int32 ItemId) override;
    virtual void OnItemRemoved(int32 ItemId) override;
    virtual const TArray<int32>& GetAllItems() const override;
    //~ End IInventoryKitContainerInterface
    
    /**
     * 设置背包容量
     * @param NewCapacity 新的容量
     */
    UFUNCTION(BlueprintCallable, Category = "InventoryKit")
    void SetCapacity(int32 NewCapacity);
    
    /**
     * 获取背包容量
     * @return 背包容量
     */
    UFUNCTION(BlueprintCallable, Category = "InventoryKit")
    int32 GetCapacity() const;
    
    /**
     * 获取当前物品数量
     * @return 当前物品数量
     */
    UFUNCTION(BlueprintCallable, Category = "InventoryKit")
    int32 GetItemCount() const;
    
    /**
     * 获取当前重量
     * @return 当前重量
     */
    UFUNCTION(BlueprintCallable, Category = "InventoryKit")
    float GetCurrentWeight() const;
    
    /**
     * 设置重量限制
     * @param NewWeightLimit 新的重量限制
     */
    UFUNCTION(BlueprintCallable, Category = "InventoryKit")
    void SetWeightLimit(float NewWeightLimit);
    
    /**
     * 获取重量限制
     * @return 重量限制
     */
    UFUNCTION(BlueprintCallable, Category = "InventoryKit")
    float GetWeightLimit() const;
    
    /**
     * 检查背包是否包含指定物品
     * 
     * @param ItemId 要检查的物品ID
     * @return 是否包含此物品
     */
    UFUNCTION(BlueprintCallable, Category = "InventoryKit")
    bool ContainsItem(int32 ItemId) const;
    
    // 背包内容变更事件
    UPROPERTY(BlueprintAssignable, Category = "InventoryKit")
    FOnBagChanged OnBagChanged;
}; 