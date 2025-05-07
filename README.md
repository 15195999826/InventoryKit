# InventoryKit 插件

InventoryKit是一个为Unreal Engine设计的物品、背包和掉落系统框架，提供了完整的物品管理解决方案。该框架采用模块化设计，易于扩展和集成到各类游戏中，专注于提供逻辑层面的功能而不包含UI实现。

## 主要特性

- **物品系统** - 整个框架的核心，负责定义和管理游戏中的各类物品及其位置信息
- **背包组件** - 负责管理玩家随身携带的物品
- **装备组件** - 负责管理玩家装备的物品及其效果
- **掉落组件** - 负责从游戏世界中获取物品的机制
- **混合模式架构** - 平衡数据一致性和性能需求的设计

## 安装说明

1. 将InventoryKit文件夹复制到项目的Plugins目录下
2. 重新生成项目解决方案
3. 在项目的Build.cs文件中添加对"InventoryKit"模块的依赖
4. 重新编译项目

## 快速入门

### 1. 创建项目物品系统

```cpp
// 在游戏模块中继承物品系统基类
UCLASS()
class MYGAME_API UMyGameItemSystem : public UInventoryKitItemSystem
{
    GENERATED_BODY()
    
public:
    // 实现必要的抽象方法
    virtual bool GetItemLocation(int32 ItemId, FContainerLocation& OutLocation) const override;
    virtual bool MoveItem(int32 ItemId, FContainerLocation SourceLocation, FContainerLocation TargetLocation) override;
    virtual int32 CreateItem(FName ConfigId, FContainerLocation Location) override;
    virtual TArray<int32> GetItemsInContainer(FContainerLocation Location) const override;
    virtual FItemBaseInstance GetItemData(int32 ItemId) const override;
    
    // 添加游戏特定功能
    // ...
};
```

### 2. 定义游戏物品结构

```cpp
// 继承基础物品结构体
USTRUCT(BlueprintType)
struct FMyGameItemInstance : public FItemBaseInstance
{
    GENERATED_BODY()
    
    // 添加游戏特定属性
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 StackCount;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Quality;
    
    // 其他自定义属性
};
```

### 3. 添加背包组件

```cpp
// 在角色类中添加
UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
UInventoryKitBagComponent* BagComponent;

// 在构造函数中初始化
BagComponent = CreateDefaultSubobject<UInventoryKitBagComponent>(TEXT("BagComponent"));
BagComponent->SetCapacity(20);
```

### 4. 添加装备组件

```cpp
// 在角色类中添加
UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
UInventoryKitEquipmentComponent* EquipmentComponent;

// 在构造函数中初始化
EquipmentComponent = CreateDefaultSubobject<UInventoryKitEquipmentComponent>(TEXT("EquipmentComponent"));
```

### 5. 使用掉落组件

```cpp
// 在可掠夺的Actor中添加
UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
UInventoryKitLootComponent* LootComponent;

// 在构造函数中初始化
LootComponent = CreateDefaultSubobject<UInventoryKitLootComponent>(TEXT("LootComponent"));

// 添加掉落物品
LootComponent->AddLootItem(TEXT("Weapon_Sword"), 0.7f, 1, 1);
LootComponent->AddLootItem(TEXT("Gold"), 1.0f, 10, 50);
```

## 组件间移动物品

```cpp
// 获取物品系统
UMyGameItemSystem* ItemSystem = GetWorld()->GetSubsystem<UMyGameItemSystem>();

// 从背包移动到装备栏
FContainerLocation SourceLocation = BagComponent->GetContainerLocation();
FContainerLocation TargetLocation = EquipmentComponent->GetContainerLocation();
TargetLocation.SlotIndex = 0;  // 装备到特定槽位

ItemSystem->MoveItem(ItemId, SourceLocation, TargetLocation);
```

## 注意事项

- 物品系统作为World Subsystem，确保在使用前正确注册
- 物品位置变更时使用事件系统进行通知，确保UI及时更新
- 使用容器接口可以轻松扩展自定义容器类型

## 文档

完整的文档可以在 `/Plugins/InventoryKit/Source/InventoryKit/project-doc/` 目录下找到。

## 版本历史

- **1.0.0** - 初始版本，包含核心功能

## 许可证

本插件使用MIT许可证。