// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/ContainerInterfaces.h"
#include "Core/InventoryKitTypes.h"
#include "InventoryKitLootComponent.generated.h"

// 掉落项结构体
USTRUCT(BlueprintType)
struct FLootItem
{
    GENERATED_BODY()
    
    // 物品配置ID
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryKit")
    FName ItemConfigId;
    
    // 掉落几率 (0.0-1.0)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryKit", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float DropChance;
    
    // 最小数量
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryKit", meta = (ClampMin = "1"))
    int32 MinCount;
    
    // 最大数量
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryKit", meta = (ClampMin = "1"))
    int32 MaxCount;
    
    // 默认构造函数
    FLootItem()
        : DropChance(1.0f)
        , MinCount(1)
        , MaxCount(1)
    {
    }
    
    // 带参数构造函数
    FLootItem(FName InItemId, float InChance = 1.0f, int32 InMinCount = 1, int32 InMaxCount = 1)
        : ItemConfigId(InItemId)
        , DropChance(InChance)
        , MinCount(InMinCount)
        , MaxCount(InMaxCount)
    {
    }
};

// 掉落变更事件委托
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLootChanged);

/**
 * 掉落组件
 * 附加到可掉落物品的Actor上，实现物品容器接口
 */
UCLASS(ClassGroup=(InventoryKit), meta=(BlueprintSpawnableComponent))
class INVENTORYKIT_API UInventoryKitLootComponent : public UActorComponent,
                                                   public IInventoryKitContainerInterface
{
    GENERATED_BODY()

public:    
    // 构造函数
    UInventoryKitLootComponent();

protected:
    // 组件初始化
    virtual void BeginPlay() override;
    
    // 掉落容器唯一ID
    UPROPERTY(EditAnywhere, Category = "InventoryKit")
    int32 ID;
    
    // 预定义的掉落物品列表
    UPROPERTY(EditAnywhere, Category = "InventoryKit")
    TArray<FLootItem> PredefinedLootItems;
    
    // 生成的物品ID缓存
    UPROPERTY()
    TArray<int32> GeneratedItemIds;
    
    // 是否已经生成掉落物品
    UPROPERTY(VisibleAnywhere, Category = "InventoryKit")
    bool bLootGenerated;

public:
    //~ Begin IInventoryKitContainerInterface
    virtual const int32 GetContainerID() const override;
    virtual bool CanAddItem(int32 ItemId) const override;
    virtual void OnItemAdded(int32 ItemId) override;
    virtual void OnItemRemoved(int32 ItemId) override;
    virtual const TArray<int32>& GetAllItems() const override;
    //~ End IInventoryKitContainerInterface
    
    /**
     * 添加掉落项
     * @param ItemConfigId 物品配置ID
     * @param DropChance 掉落几率
     * @param MinCount 最小数量
     * @param MaxCount 最大数量
     */
    UFUNCTION(BlueprintCallable, Category = "InventoryKit")
    void AddLootItem(FName ItemConfigId, float DropChance = 1.0f, int32 MinCount = 1, int32 MaxCount = 1);
    
    /**
     * 清空掉落项
     */
    UFUNCTION(BlueprintCallable, Category = "InventoryKit")
    void ClearLootItems();
    
    /**
     * 生成掉落物品
     * 根据掉落几率和数量生成实际掉落的物品
     * 
     * @param bForceRegenerate 是否强制重新生成（即使已经生成过）
     * @return 是否成功生成
     */
    UFUNCTION(BlueprintCallable, Category = "InventoryKit")
    bool GenerateLoot(bool bForceRegenerate = false);
    
    /**
     * 获取掉落物品数量
     * @return 物品数量
     */
    UFUNCTION(BlueprintCallable, Category = "InventoryKit")
    int32 GetLootItemCount() const;
    
    /**
     * 取出所有物品
     * 将所有掉落物品移动到目标容器
     * 
     * @param TargetLocation 目标容器位置
     * @return 是否成功取出所有物品
     */
    UFUNCTION(BlueprintCallable, Category = "InventoryKit")
    bool TakeAllItems(FItemLocation TargetLocation);
    
    /**
     * 取出指定物品
     * 将指定物品移动到目标容器
     * 
     * @param ItemId 要取出的物品ID
     * @param TargetLocation 目标容器位置
     * @return 是否成功取出物品
     */
    UFUNCTION(BlueprintCallable, Category = "InventoryKit")
    bool TakeItem(int32 ItemId, FItemLocation TargetLocation);
    
    // 掉落物品变更事件
    UPROPERTY(BlueprintAssignable, Category = "InventoryKit")
    FOnLootChanged OnLootChanged;
}; 