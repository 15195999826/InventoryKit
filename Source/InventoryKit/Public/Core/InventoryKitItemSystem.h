// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InventoryKitBaseContainerComponent.h"
#include "Subsystems/WorldSubsystem.h"
#include "Core/InventoryKitTypes.h"
#include "InventoryKitItemSystem.generated.h"

class UInventoryKitVoidContainer;
DEFINE_LOG_CATEGORY_STATIC(LogInventoryKitSystem, Log, All);
/**
 * 物品系统抽象基类
 * 作为物品管理的核心，负责物品创建、查询、移动和销毁
 * 项目需要继承此类并实现具体功能
 */
UCLASS(Abstract)
class INVENTORYKIT_API UInventoryKitItemSystem : public UWorldSubsystem
{
    GENERATED_BODY()
    
protected:
    // 物品实例映射表
    UPROPERTY()
    TMap<int32, FItemBaseInstance> ItemMap;
     
    /**
     * 容器列表， 初始化时， 创建一个虚空容器， 占用第一个ID
     */
    TMap<int32, IInventoryKitContainerInterface*> ContainerMap;
    
    // 虚空容器ID, 初始化系统时创建
    int32 VoidContainerID = -1;
    
    // 下一个可用的物品ID
    int32 NextItemID = 0;

    // 下一个可用的容器ID
    int32 NextContainerID = 0;
    
public:
    // 初始化
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    // 清理
    virtual void Deinitialize() override;

    /**
     * 获取物品位置
     * 基础实现：直接从映射表中获取
     */
    UFUNCTION(BlueprintCallable, Category = "InventoryKit")
    virtual bool GetItemLocation(int32 ItemId, FItemLocation& OutLocation) const;

    /**
     * 移动物品
     * 基础实现：更新位置映射并触发事件
     */
    UFUNCTION(BlueprintCallable, Category = "InventoryKit")
    virtual bool MoveItem(int32 ItemId, const FItemLocation& TargetLocation);
    
    /**
     * 查询指定容器中的所有物品
     * 基础实现：遍历位置映射表
     */
    UFUNCTION(BlueprintCallable, Category = "InventoryKit")
    virtual TArray<int32> GetItemsInContainer(int32 Identifier) const;

    /**
     * 获取物品数据
     * 基础实现：直接从映射表中获取
     */
    UFUNCTION(BlueprintCallable, Category = "InventoryKit")
    virtual FItemBaseInstance GetItemBaseInstance(int32 ItemId) const;
    
    /**
     * 注册容器
     * @param InContainer 
     */
    void RegisterContainer(IInventoryKitContainerInterface* InContainer);

    /**
     * 注销容器
     * @param InContainer 
     */
    void UnregisterContainer(IInventoryKitContainerInterface* InContainer);

    int32 GetVoidContainerID() const
    {
        return VoidContainerID;
    }

    const TMap<int32, IInventoryKitContainerInterface*>& GetContainerMap() const
    {
        return ContainerMap;
    }

protected:
    /**
     * 创建物品
     * 基础实现：生成新ID并创建物品实例
     */
    virtual int32 IntervalCreateItem(const FItemLocation& Location, bool bNotify = true);

private:
    // 防止GC
    UPROPERTY()
    TObjectPtr<UInventoryKitVoidContainer> VoidContainer;
}; 