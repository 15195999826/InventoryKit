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
 * 支持网格式布局，可配置为简单列表或二维网格
 * 通过将FItemLocation的SlotIndex是一维数据， 在背包中需要进行一次转化
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
    
    // 网格宽度
    UPROPERTY(EditAnywhere, Category = "InventoryKit|Grid", meta=(ClampMin="1"))
    int32 GridWidth;
    
    // 网格高度
    UPROPERTY(EditAnywhere, Category = "InventoryKit|Grid", meta=(ClampMin="1"))
    int32 GridHeight;
    
    // 槽位状态标志（0=正常, 1=锁定, 2=高亮等）
    UPROPERTY(VisibleAnywhere, Category = "InventoryKit|Grid")
    TArray<uint8> SlotFlags;
    
    // 容量限制（由网格尺寸决定，GridWidth * GridHeight）
    UPROPERTY(VisibleAnywhere, Category = "InventoryKit")
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
     * 初始化网格
     * 根据GridWidth和GridHeight初始化网格结构
     */
    UFUNCTION(BlueprintCallable, Category = "InventoryKit|Grid")
    void InitializeGrid();
    
    /**
     * 设置网格尺寸
     * 
     * @param Width 新的网格宽度
     * @param Height 新的网格高度
     */
    UFUNCTION(BlueprintCallable, Category = "InventoryKit|Grid")
    void SetGridSize(int32 Width, int32 Height);
    
    /**
     * 获取网格尺寸
     * 
     * @param OutWidth 输出参数，网格宽度
     * @param OutHeight 输出参数，网格高度
     */
    UFUNCTION(BlueprintCallable, Category = "InventoryKit|Grid")
    void GetGridSize(int32& OutWidth, int32& OutHeight) const;
    
    /**
     * 将二维坐标转换为一维槽位索引
     * 
     * @param X X坐标
     * @param Y Y坐标
     * @return 对应的槽位索引，如果坐标无效则返回-1
     */
    UFUNCTION(BlueprintCallable, Category = "InventoryKit|Grid")
    int32 CoordinateToIndex(int32 X, int32 Y) const;
    
    /**
     * 将一维槽位索引转换为二维坐标
     * 
     * @param Index 槽位索引
     * @param OutX 输出参数，X坐标
     * @param OutY 输出参数，Y坐标
     */
    UFUNCTION(BlueprintCallable, Category = "InventoryKit|Grid")
    void IndexToCoordinate(int32 Index, int32& OutX, int32& OutY) const;
    
    /**
     * 检查槽位是否可用（没有物品且未锁定）
     * 
     * @param SlotIndex 要检查的槽位索引
     * @return 槽位是否可用
     */
    UFUNCTION(BlueprintCallable, Category = "InventoryKit|Grid")
    bool IsSlotAvailable(int32 SlotIndex) const;
    
    /**
     * 查找可用的槽位
     * 
     * @return 找到的可用槽位索引，如果没有则返回-1
     */
    UFUNCTION(BlueprintCallable, Category = "InventoryKit|Grid")
    int32 FindAvailableSlot() const;
    
    /**
     * 获取槽位中的物品ID
     * 
     * @param SlotIndex 槽位索引
     * @return 物品ID，如果槽位为空则返回-1
     */
    UFUNCTION(BlueprintCallable, Category = "InventoryKit|Grid")
    int32 GetItemAtSlot(int32 SlotIndex) const;
    
    /**
     * 获取物品所在的槽位索引
     * 
     * @param ItemId 物品ID
     * @return 槽位索引，如果物品不在容器中则返回-1
     */
    UFUNCTION(BlueprintCallable, Category = "InventoryKit|Grid")
    int32 GetItemSlotIndex(int32 ItemId) const;
    
    /**
     * 设置槽位锁定状态
     * 
     * @param SlotIndex 槽位索引
     * @param bLocked 是否锁定
     */
    UFUNCTION(BlueprintCallable, Category = "InventoryKit|Grid")
    void SetSlotLocked(int32 SlotIndex, bool bLocked);
    
    /**
     * 设置背包容量
     * 此方法现在是SetGridSize的别名，保留为向后兼容
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