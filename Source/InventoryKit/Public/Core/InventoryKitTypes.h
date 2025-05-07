// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InventoryKitTypes.generated.h"

/**
 * 容器空间类型
 * 定义容器如何管理物品槽位
 */
UENUM(BlueprintType)
enum class EContainerSpaceType : uint8
{
    Unordered UMETA(DisplayName = "无序容器"),   // 无序容器空间 - 不关心物品具体位置
    Fixed UMETA(DisplayName = "固定槽位容器"),    // 固定槽位容器空间 - 预定义槽位类型
    Grid UMETA(DisplayName = "网格容器")         // 网格容器空间 - 二维网格布局
};

/**
 * 容器配置结构体
 * 用于初始化和配置容器的槽位管理方式
 */
USTRUCT(BlueprintType)
struct INVENTORYKIT_API FContainerSpaceConfig
{
    GENERATED_BODY()
    
    // 容器空间类型
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryKit")
    EContainerSpaceType SpaceType;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryKit|Unordered")
    int32 Capacity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryKit|Grid", meta = (EditCondition = "SpaceType == EContainerSpaceType::Fixed", EditConditionHides))
    TArray<FGameplayTag> FixedSlotTypes;
    
    // 网格容器配置: 网格宽度
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryKit|Grid", meta = (EditCondition = "SpaceType == EContainerSpaceType::Grid", EditConditionHides, ClampMin = "1"))
    int32 GridWidth;
    
    // 网格容器配置: 网格高度
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryKit|Grid", meta = (EditCondition = "SpaceType == EContainerSpaceType::Grid", EditConditionHides, ClampMin = "1"))
    int32 GridHeight;

    // Todo: 负重等配置
    
    // 构造函数 - 默认为无序容器，不限制容量
    FContainerSpaceConfig()
        : SpaceType(EContainerSpaceType::Unordered)
        , Capacity(-1)
        , GridWidth(1)
        , GridHeight(1)
    {
    }

    FContainerSpaceConfig(EContainerSpaceType InSpaceType, int32 InCapacity, int32 InGridWidth, int32 InGridHeight)
        : SpaceType(InSpaceType)
        , Capacity(InCapacity)
        , GridWidth(InGridWidth)
        , GridHeight(InGridHeight)
    {
    }
};

/**
 * 物品位置结构体，用于唯一标识物品在系统中的位置
 */
USTRUCT(BlueprintType)
struct INVENTORYKIT_API FItemLocation
{
    GENERATED_BODY()
    
    // 容器ID
    UPROPERTY(BlueprintReadOnly)
    int32 ContainerID;
    
    // 槽位索引（可选）
    UPROPERTY(BlueprintReadOnly)
    int32 SlotIndex;
    
    // 构造函数
    FItemLocation()
        : ContainerID(0), SlotIndex(-1)
    {
    }

    // 比较操作符
    bool operator==(const FItemLocation& Other) const
    {
        return ContainerID == Other.ContainerID && 
               SlotIndex == Other.SlotIndex;
    }
    
    bool operator!=(const FItemLocation& Other) const
    {
        return !(*this == Other);
    }
};

/**
 * 物品基础结构体，项目如果需要额外实例数据， 可以通过创建一个新的结构体， 然后继承InventorySystem, 在其中增加一个<ID, CustomData>的Map来保存实例数据
 */
USTRUCT(BlueprintType)
struct INVENTORYKIT_API FItemBaseInstance
{
    GENERATED_BODY()
    
    // 物品唯一ID
    UPROPERTY(BlueprintReadOnly)
    int32 ItemId;
    
    // 物品配置ID（对应数据表中的行名）
    UPROPERTY(BlueprintReadOnly)
    FName ConfigRowName;

    UPROPERTY(BlueprintReadOnly)
    FItemLocation ItemLocation;
    
    // 基础构造函数
    FItemBaseInstance()
        : ItemId(-1)
    {
    }
};