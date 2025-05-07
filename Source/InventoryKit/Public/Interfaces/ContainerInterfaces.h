// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/InventoryKitTypes.h"
#include "UObject/Interface.h"
#include "ContainerInterfaces.generated.h"
class UContainerSpaceManager;
/**
 * 物品容器接口
 * 所有可以存储物品的组件都应该实现此接口
 */
UINTERFACE(MinimalAPI, Blueprintable)
class UInventoryKitContainerInterface : public UInterface
{
    GENERATED_BODY()
};

/**
 * 物品容器接口的实际接口类
 * 定义了所有容器必须实现的方法
 */
class INVENTORYKIT_API IInventoryKitContainerInterface
{
    GENERATED_BODY()
public:
    virtual void InitContainer() = 0;

    /**
     * 获取容器唯一ID
     * @return 容器ID
     */
    virtual const int32 GetContainerID() const = 0;
    
    /**
     * 检查容器是否可以添加指定物品
     * 实现此方法可以检查容量、重量、类型限制等
     * 
     * @param ItemId 要添加的物品ID
     * @return 是否可以添加物品
     */
    virtual bool CanAddItem(int32 ItemId) const = 0;
    
    /**
     * 物品添加通知回调
     * 当物品系统将物品添加到此容器时会调用此方法
     * 
     * @param ItemId 添加的物品ID
     */
    virtual void OnItemAdded(int32 ItemId) = 0;
    
    /**
     * 物品移除通知回调
     * 当物品系统从此容器移除物品时会调用此方法
     * 
     * @param ItemId 移除的物品ID
     */
    virtual void OnItemRemoved(int32 ItemId) = 0;
    
    /**
     * 获取容器中所有物品ID
     * 用于快速访问容器内容
     * 
     * @return 物品ID数组
     */
    virtual const TArray<int32>& GetAllItems() const = 0;

    virtual UContainerSpaceManager* GetSpaceManager() = 0;

    static UContainerSpaceManager* CreateSpaceManager(UObject* InOuter, const FContainerSpaceConfig& InConfig);
};