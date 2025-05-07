// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/ContainerInterfaces.h"
#include "Core/InventoryKitTypes.h"
#include "InventoryKitBaseContainerComponent.generated.h"

// 前向声明
class UContainerSpaceManager;

// 容器变更事件委托
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnContainerChanged);

/**
 * 基础容器组件
 * 实现物品容器接口
 * 通过配置ContainerConfig, 可以创建不同类型的容器
 * 假设项目需要实现背包、装备、掉落等组件， 可以通过继承当前类， 增加额外逻辑
 */
UCLASS(ClassGroup=(InventoryKit), meta=(BlueprintSpawnableComponent))
class INVENTORYKIT_API UInventoryKitBaseContainerComponent : public UActorComponent, 
                                                  public IInventoryKitContainerInterface
{
    GENERATED_BODY()

public:    
    // 构造函数
    UInventoryKitBaseContainerComponent();

protected:
    // 组件初始化
    virtual void BeginPlay() override;
    
    // 背包唯一标识
    UPROPERTY(EditAnywhere, Category = "InventoryKit")
    int32 ID;
    
    // 物品ID缓存
    UPROPERTY()
    TArray<int32> ItemIds;
    
    // 容器配置
    UPROPERTY(EditAnywhere, Category = "InventoryKit|Configuration")
    FContainerSpaceConfig SpaceConfig;
    
    // 容器空间管理器
    UPROPERTY()
    TObjectPtr<UContainerSpaceManager> SpaceManager;

public:
    //~ Begin IInventoryKitContainerInterface
    virtual void InitContainer() override;
    virtual const int32 GetContainerID() const override;
    virtual bool CanAddItem(const FItemBaseInstance& InItem, int32 DstSlotIndex) const override;
    virtual bool CanMoveItem(const FItemBaseInstance& InItem, int32 DstSlotIndex) const override;
    virtual void OnItemAdded(const FItemBaseInstance& InItem) override;
    virtual void OnItemRemoved(const FItemBaseInstance& InItem) override;
    virtual const TArray<int32>& GetAllItems() const override;
    virtual UContainerSpaceManager* GetSpaceManager() override;
    //~ End IInventoryKitContainerInterface
    
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
    FOnContainerChanged OnContainerChanged;
};