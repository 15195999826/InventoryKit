// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ContainerSpace/ContainerSpaceManager.h"
#include "GridSpaceManager.generated.h"

/**
 * 网格容器空间管理器
 * 用于管理以网格形式布局的容器
 * Todo: 提供调整网格大小的功能
 */
UCLASS()
class INVENTORYKIT_API UGridSpaceManager : public UContainerSpaceManager
{
    GENERATED_BODY()
    
private:
    // 网格宽度
    int32 GridWidth;
    
    // 网格高度
    int32 GridHeight;

    /**
     * 槽位状态数组 -- 0表示可用，1表示被占用 Todo: 其他各种概念视情况添加
     * 只通过持有者的Add or Remove 函数进行更新
     */
    TArray<uint8> SlotFlags;
    
public:
    // 构造函数
    UGridSpaceManager();
    
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
     * 获取网格尺寸
     * 
     * @param OutWidth 输出参数，网格宽度
     * @param OutHeight 输出参数，网格高度
     */
    void GetGridSize(int32& OutWidth, int32& OutHeight) const;
    
    /**
     * 将二维坐标转换为一维槽位索引
     * 
     * @param X X坐标
     * @param Y Y坐标
     * @return 对应的槽位索引，如果坐标无效则返回-1
     */
    int32 CoordinateToIndex(int32 X, int32 Y) const;
    
    /**
     * 将一维槽位索引转换为二维坐标
     * 
     * @param Index 槽位索引
     * @param OutX 输出参数，X坐标
     * @param OutY 输出参数，Y坐标
     */
    void IndexToCoordinate(int32 Index, int32& OutX, int32& OutY) const;
};