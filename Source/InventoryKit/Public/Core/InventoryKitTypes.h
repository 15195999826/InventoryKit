// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InventoryKitTypes.generated.h"
/**
 * 物品位置结构体，用于唯一标识物品在系统中的位置
 */
USTRUCT(BlueprintType)
struct INVENTORYKIT_API FItemLocation
{
    GENERATED_BODY()
    
    // 容器类型（背包/装备/仓库等）
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 ContainerID;
    
    // 槽位索引（可选）
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
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
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 ItemId;
    
    // 物品配置ID（对应数据表中的行名）
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName ConfigRowName;
    
    // 基础构造函数
    FItemBaseInstance()
        : ItemId(-1)
    {
    }
};

// 装备槽结构体
USTRUCT(BlueprintType)
struct FBaseEquipmentSlot
{
    GENERATED_BODY()
    
    // 槽位类型
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryKit")
    FGameplayTag SlotType;
    
    // 装备在此槽位的物品ID
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryKit")
    int32 EquippedItemId;
    
    // 默认构造函数
    FBaseEquipmentSlot()
        : SlotType(FGameplayTag::EmptyTag)
        , EquippedItemId(-1)
    {
    }
};