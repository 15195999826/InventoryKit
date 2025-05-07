# 容器槽位系统

## 概述

在InventoryKit框架中，容器槽位系统是管理物品在容器内部具体存储位置的关键机制。通过槽位系统，我们可以实现更精细的物品位置管理、更灵活的物品交互方式，以及更直观的用户界面展示。

## 槽位索引 (SlotIndex)

### 定义

槽位索引(SlotIndex)是一个整数值，用于标识物品在容器内部的具体位置。在InventoryKit中，SlotIndex有以下特性：

- **默认值**：-1，表示物品没有指定具体槽位，或容器不支持槽位系统
- **有效值**：0或正整数，表示物品位于容器的特定槽位
- **唯一性**：在同一容器内，通常每个槽位只能容纳一个物品（除非支持堆叠）

### 在物品位置中的应用

在`FItemLocation`结构体中，我们使用`SlotIndex`来精确定位物品：

```cpp
USTRUCT(BlueprintType)
struct INVENTORYKIT_API FItemLocation
{
    GENERATED_BODY()
    
    // 容器ID
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 ContainerID;
    
    // 槽位索引
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 SlotIndex;
    
    // 构造函数
    FItemLocation()
        : ContainerID(0), SlotIndex(-1)
    {
    }
};
```

## 容器空间类型

InventoryKit支持多种容器空间类型，每种类型对应不同的槽位管理方式：

### 1. 无序容器空间

无序容器空间不关心物品的具体位置，只关心物品是否在容器中。这类容器的特点是：

- **SlotIndex恒为-1**：物品没有具体槽位
- **典型应用**：简单背包、掉落容器、无格子UI的物品列表
- **实现方式**：容器内部使用`TArray<int32>`存储物品ID

```cpp
// 无序容器示例（UInventoryKitVoidContainer）
virtual void OnItemAdded(int32 ItemId) override
{
    ItemIds.Add(ItemId);
    OnContainerChanged.Broadcast();
}
```

### 2. 固定槽位容器空间

固定槽位容器为每个物品分配固定的槽位索引，通常用于装备系统。特点是：

- **预定义槽位**：容器预先定义了固定数量的槽位，每个槽位有特定用途
- **SlotIndex代表槽位类型**：索引通常对应特定槽位类型（头部、身体等）
- **典型应用**：装备系统、快捷栏、固定布局的特殊容器
- **实现方式**：通常使用`TArray<FBaseEquipmentSlot>`管理槽位

```cpp
// 固定槽位容器示例（装备组件）
bool UInventoryKitEquipmentComponent::EquipItem(int32 ItemId, EEquipmentSlotType SlotType, int32 SlotIndex)
{
    // 验证槽位有效性
    if (SlotIndex < 0 || SlotIndex >= EquipmentSlots.Num())
    {
        return false;
    }
    
    // 更新槽位数据
    EquipmentSlots[SlotIndex].EquippedItemId = ItemId;
    EquipmentSlots[SlotIndex].SlotType = ConvertSlotTypeToTag(SlotType);
    
    // 更新物品缓存
    if (!EquippedItemIds.Contains(ItemId))
    {
        EquippedItemIds.Add(ItemId);
    }
    
    // 触发事件
    OnEquipmentChanged.Broadcast(EquipmentSlots[SlotIndex].SlotType, SlotIndex);
    return true;
}
```

### 3. 网格容器空间

网格容器允许物品放置在二维网格的任意位置，通常用于背包系统。特点是：

- **二维坐标映射**：二维网格坐标(x,y)映射为一维SlotIndex
- **物品可占据多个槽位**：大型物品可占据网格中的多个相邻槽位
- **典型应用**：网格式背包、拖放UI界面、物品合成系统
- **实现方式**：通过将网格功能集成到背包组件中实现

```cpp
// 坐标映射示例（集成在背包组件中）
int32 UInventoryKitBagComponent::CoordinateToIndex(int32 X, int32 Y) const
{
    // 确保坐标在网格范围内
    if (X < 0 || X >= GridWidth || Y < 0 || Y >= GridHeight)
    {
        return -1;
    }
    return Y * GridWidth + X;
}

// 逆映射
void UInventoryKitBagComponent::IndexToCoordinate(int32 Index, int32& OutX, int32& OutY) const
{
    if (Index < 0 || Index >= GridWidth * GridHeight)
    {
        OutX = -1;
        OutY = -1;
        return;
    }
    
    OutX = Index % GridWidth;
    OutY = Index / GridWidth;
}
```

## 容器的槽位容量

容器的槽位容量决定了容器可以容纳多少物品以及如何放置这些物品：

### 1. 背包的网格容量

`UInventoryKitBagComponent`通过GridWidth和GridHeight属性定义网格布局和总容量：

```cpp
// 背包网格定义
UPROPERTY(EditAnywhere, Category = "InventoryKit")
int32 GridWidth;

UPROPERTY(EditAnywhere, Category = "InventoryKit")
int32 GridHeight;
```

这定义了背包的二维网格布局。总容量为`GridWidth * GridHeight`，当只需要一维列表时，可以将GridHeight设为1。

### 2. 装备槽容量

装备组件通过预定义的装备槽数组定义容量：

```cpp
// 装备组件容量
UPROPERTY(EditAnywhere, Category = "InventoryKit")
TArray<FBaseEquipmentSlot> EquipmentSlots;
```

每个装备槽对应一个特定类型的装备位置，通常通过游戏标签系统定义。

## SlotIndex在不同容器中的使用

### 1. 背包组件 (UInventoryKitBagComponent)

在背包中，SlotIndex表示物品在网格中的位置：

- 通过网格坐标计算得到，使用公式：`SlotIndex = Y * GridWidth + X`
- 背包组件提供坐标和索引之间的相互转换
- 支持基于槽位的物品拖放操作
- 当只需一维列表时，可将GridHeight设为1，此时SlotIndex等同于列表索引

```cpp
// 获取指定槽位的物品ID
int32 UInventoryKitBagComponent::GetItemAtSlot(int32 SlotIndex) const
{
    if (SlotIndex >= 0 && SlotIndex < GridWidth * GridHeight)
    {
        // 查找该槽位中的物品
        for (int32 ItemId : ItemIds)
        {
            FItemLocation Location;
            auto ItemSystem = GetWorld()->GetSubsystem<UInventoryKitItemSystem>();
            if (ItemSystem && ItemSystem->GetItemLocation(ItemId, Location) && 
                Location.SlotIndex == SlotIndex)
            {
                return ItemId;
            }
        }
    }
    return -1;
}
```

### 2. 装备组件 (UInventoryKitEquipmentComponent)

在装备组件中，SlotIndex直接对应装备槽数组的索引：

```cpp
// 获取装备在指定槽位的物品ID
int32 UInventoryKitEquipmentComponent::GetEquippedItemId(EEquipmentSlotType SlotType, int32 SlotIndex) const
{
    if (SlotIndex >= 0 && SlotIndex < EquipmentSlots.Num())
    {
        return EquipmentSlots[SlotIndex].EquippedItemId;
    }
    return -1;
}
```

### 3. 仓库和其他容器

其他类型的容器可以根据需要选择合适的槽位管理方式：

- 仓库可以使用与背包相同的网格式布局
- 快速栏可以使用固定槽位，每个槽位对应一个快捷键
- 特殊容器（如任务物品容器）可以使用自定义槽位逻辑

## 物品移动与槽位交互

所有物品移动操作统一通过`UInventoryKitItemSystem::MoveItem`函数处理，确保物品位置的一致性和有效性：

```cpp
bool UInventoryKitItemSystem::MoveItem(int32 ItemId, const FItemLocation& SourceLocation, const FItemLocation& TargetLocation)
{
    // 验证源位置
    FItemLocation CurrentLocation;
    if (!GetItemLocation(ItemId, CurrentLocation) || CurrentLocation != SourceLocation)
    {
        return false;
    }
    
    // 获取源容器和目标容器
    IInventoryKitContainerInterface* SourceContainer = FindContainer(SourceLocation.ContainerID);
    IInventoryKitContainerInterface* TargetContainer = FindContainer(TargetLocation.ContainerID);
    
    if (!SourceContainer || !TargetContainer)
    {
        return false;
    }
    
    // 检查目标容器是否可以接收物品
    if (!TargetContainer->CanAddItem(ItemId))
    {
        return false;
    }
    
    // 通知源容器移除物品
    SourceContainer->OnItemRemoved(ItemId);
    
    // 通知目标容器添加物品
    TargetContainer->OnItemAdded(ItemId);
    
    // 更新物品位置（包括槽位信息）
    ItemLocations[ItemId] = TargetLocation;
    
    // 广播位置变更事件
    OnItemLocationChanged.Broadcast(ItemId, SourceLocation, TargetLocation);
    
    return true;
}
```

### 重要原则

物品位置移动应遵循以下原则：

1. **统一入口**：所有物品移动操作都通过物品系统的MoveItem函数处理
2. **位置验证**：移动前验证物品的当前位置与源位置是否匹配
3. **容器通知**：移动过程中通知源容器和目标容器更新缓存
4. **完整位置信息**：移动时传递完整的位置信息，包括容器ID和槽位索引
5. **事件广播**：移动完成后广播位置变更事件，供UI等组件响应

这种设计确保了物品位置信息的一致性和准确性，避免了分散在各个组件中的物品移动逻辑可能导致的数据不一致问题。

## 实现背包网格系统的建议

对于将网格功能集成到背包组件的实现，建议以下步骤：

1. **扩展背包组件添加网格相关属性**：
   ```cpp
   // 在UInventoryKitBagComponent中添加
   UPROPERTY(EditAnywhere, Category = "InventoryKit")
   int32 GridWidth;
   
   UPROPERTY(EditAnywhere, Category = "InventoryKit")
   int32 GridHeight;
   
   UPROPERTY(VisibleAnywhere, Category = "InventoryKit")
   TArray<uint8> SlotFlags; // 槽位状态标志
   ```

2. **提供网格坐标转换函数**：
   ```cpp
   // 坐标与索引转换
   UFUNCTION(BlueprintCallable, Category = "InventoryKit")
   int32 CoordinateToIndex(int32 X, int32 Y) const;
   
   UFUNCTION(BlueprintCallable, Category = "InventoryKit")
   void IndexToCoordinate(int32 Index, int32& OutX, int32& OutY) const;
   ```

3. **实现槽位查询和操作函数**：
   ```cpp
   // 槽位查询
   UFUNCTION(BlueprintCallable, Category = "InventoryKit")
   bool IsSlotAvailable(int32 SlotIndex) const;
   
   UFUNCTION(BlueprintCallable, Category = "InventoryKit")
   int32 FindAvailableSlot() const;
   
   UFUNCTION(BlueprintCallable, Category = "InventoryKit")
   int32 GetItemAtSlot(int32 SlotIndex) const;
   ```

4. **网格初始化**：
   ```cpp
   // 初始化网格
   void UInventoryKitBagComponent::SetGridSize(int32 Width, int32 Height)
   {
       GridWidth = FMath::Max(1, Width);
       GridHeight = FMath::Max(1, Height);
       SlotFlags.SetNumZeroed(GridWidth * GridHeight);
   }
   ```

## 未来扩展方向

1. **物品大小系统**：支持不同尺寸的物品占据多个网格槽位
2. **自动排序机制**：基于物品属性自动整理槽位
3. **槽位限制系统**：特定槽位只能放置特定类型的物品
4. **槽位分组**：将相关槽位分组，便于UI展示和交互
5. **槽位持久化**：保存和恢复玩家自定义的槽位布局

## 结论

槽位系统是InventoryKit框架中连接物品数据与用户界面的关键环节。通过将网格功能集成到背包组件中，我们可以提供更灵活的物品管理体验，同时保持API的简洁性。统一的物品移动逻辑确保了数据一致性，使系统更加可靠和易于维护。不同类型的游戏可以通过调整网格尺寸和配置来满足特定的游戏玩法需求。 