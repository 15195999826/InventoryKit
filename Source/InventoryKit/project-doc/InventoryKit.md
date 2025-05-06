# InventoryKit 框架

## 架构概述

InventoryKit是一个为游戏设计的物品、背包和掉落系统框架，提供了完整的物品管理解决方案。该框架采用模块化设计，易于扩展和集成到各类游戏中，专注于提供逻辑层面的功能而不包含UI实现。系统主要由以下系统和组件构成：

1. **物品系统** - 整个框架的核心，负责定义和管理游戏中的各类物品及其位置信息（抽象系统）
2. **背包组件** - 负责管理玩家随身携带的物品（组件）
3. **仓库系统** - 负责管理长期存储的物品（系统）
4. **掉落组件** - 负责从游戏世界中获取物品的机制（组件）
5. **商店组件** - 负责物品的购买、出售和交易功能（组件）
6. **装备组件** - 负责管理玩家装备的物品及其效果（组件）

整个物品管理流程遵循以下逻辑：
1. 项目继承物品系统基类创建具体实现，所有物品由物品系统统一管理
2. 各组件（背包组件、装备组件等）仅持有物品ID的缓存，实际物品实例由物品系统管理
3. 物品的位置信息统一由物品系统管理并作为权威数据源，确保数据一致性
4. 当物品需要移动时，通过物品系统进行操作，并通知相关组件更新缓存
5. 项目通过继承基础物品结构体来定义具体物品属性和行为

## 物品ID与位置管理机制

InventoryKit采用混合模式管理物品，平衡数据一致性和性能需求：

1. **物品系统作为权威数据源**：
   - 物品系统维护所有物品的位置信息，作为唯一真实数据源
   - 提供物品创建、查询、移动、销毁的统一API
   - 确保物品数据一致性和完整性

2. **组件缓存物品ID**：
   - 各组件（背包、装备等）本地缓存自己管理的物品ID列表
   - 缓存仅用于快速访问，非权威数据
   - 组件初始化时从物品系统同步物品列表

3. **事件通知机制**：
   - 物品系统提供事件通知，当物品位置变更时通知相关组件
   - 组件监听事件并更新本地缓存，保持数据同步

4. **操作验证流程**：
   - 组件操作物品前先通过物品系统验证物品位置是否正确
   - 物品移动由物品系统执行，完成后通知源组件和目标组件更新缓存

物品系统采用以下机制管理物品位置：

1. **物品ID系统**：
   - 每个物品实例由唯一的整数ID标识
   - 组件间传递物品只使用物品ID，不直接传递物品实例

2. **容器ID系统**：
   - 每个容器组件拥有唯一ID
   - 结合容器类型和ID形成全局唯一位置标识

3. **位置结构体**：
   - 使用`FContainerLocation`结构体表示物品位置
   - 包含容器类型、容器ID和可选的槽位索引
   - 可通过比较运算符直接进行位置比较

```cpp
// 物品位置结构体示例
USTRUCT(BlueprintType)
struct INVENTORYKIT_API FContainerLocation
{
    GENERATED_BODY()
    
    // 容器类型（背包、装备、仓库等）
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EContainerType ContainerType;
    
    // 容器ID
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 ContainerId;
    
    // 槽位索引（可选）
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 SlotIndex;
    
    // 构造函数
    FContainerLocation()
        : ContainerType(EContainerType::None)
        , ContainerId(-1)
        , SlotIndex(-1)
    {
    }
    
    // 使用参数构造
    FContainerLocation(EContainerType InType, int32 InId, int32 InSlot = -1)
        : ContainerType(InType)
        , ContainerId(InId)
        , SlotIndex(InSlot)
    {
    }
    
    // 比较操作符
    bool operator==(const FContainerLocation& Other) const
    {
        return ContainerType == Other.ContainerType && 
               ContainerId == Other.ContainerId && 
               SlotIndex == Other.SlotIndex;
    }
    
    bool operator!=(const FContainerLocation& Other) const
    {
        return !(*this == Other);
    }
};

// 物品移动示例
bool UMyGameItemSystem::MoveItem(int32 ItemId, FContainerLocation SourceLocation, FContainerLocation TargetLocation)
{
    // 验证物品当前位置
    FContainerLocation CurrentLocation;
    if (!GetItemLocation(ItemId, CurrentLocation) || CurrentLocation != SourceLocation)
    {
        return false;
    }
    
    // 验证目标容器是否可以接收物品
    IInventoryKitContainerInterface* TargetContainer = GetContainerByLocation(TargetLocation);
    if (!TargetContainer || !TargetContainer->CanAddItem(ItemId))
    {
        return false;
    }
    
    // 通知源容器移除物品
    IInventoryKitContainerInterface* SourceContainer = GetContainerByLocation(SourceLocation);
    if (SourceContainer)
    {
        SourceContainer->OnItemRemoved(ItemId);
    }
    
    // 通知目标容器添加物品
    TargetContainer->OnItemAdded(ItemId);
    
    // 更新物品位置
    ItemLocations[ItemId] = TargetLocation;
    
    // 广播位置变更事件
    OnItemLocationChanged.Broadcast(ItemId, SourceLocation, TargetLocation);
    
    return true;
}
```

## 核心组件

### 1. 物品系统 (UInventoryKitItemSystem)

物品系统是整个框架的核心，采用抽象基类设计，供项目继承实现：
- 定义物品基础结构体，允许项目自定义具体物品属性
- 管理所有物品实例及其位置信息（作为权威数据源）
- 提供物品创建、移动和销毁的统一接口
- 追踪物品所属的组件（背包、装备、仓库等）
- 暴露数据接口供UI层调用
- 提供物品位置变更事件，供各组件订阅

关键特性：
- 物品ID系统：每个物品实例拥有唯一整数ID
- 组件ID系统：每个容器组件拥有唯一ID，与类型结合表示物品位置
- 物品位置管理：统一管理物品所在的容器位置
- 物品基础结构体：提供框架，供项目扩展定义具体物品属性
- 物品配置集成：支持使用UE数据表管理物品基础数据
- 物品实例缓存：优化物品访问性能
- 容器接口：定义统一的容器接口，便于物品系统管理

```cpp
// 物品基础结构体
USTRUCT(BlueprintType)
struct INVENTORYKIT_API FItemBaseInstance
{
    GENERATED_BODY()
    
    // 物品唯一ID
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 ItemId;
    
    // 物品配置ID（对应数据表中的行名）
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName ConfigId;
    
    // 项目可以通过继承此结构体添加自定义属性
    // ...
};

// 物品系统抽象基类
UCLASS(Abstract)
class INVENTORYKIT_API UInventoryKitItemSystem : public UWorldSubsystem
{
    GENERATED_BODY()
    
protected:
    // 物品位置映射表
    TMap<int32, FContainerLocation> ItemLocations;
    
    // 物品实例映射表
    TMap<int32, FItemBaseInstance> Items;
    
public:
    // 获取物品位置
    UFUNCTION(BlueprintCallable, Category = "InventoryKit")
    virtual bool GetItemLocation(int32 ItemId, FContainerLocation& OutLocation) const PURE_VIRTUAL(UInventoryKitItemSystem::GetItemLocation, return false;);
    
    // 移动物品
    UFUNCTION(BlueprintCallable, Category = "InventoryKit")
    virtual bool MoveItem(int32 ItemId, FContainerLocation SourceLocation, FContainerLocation TargetLocation) PURE_VIRTUAL(UInventoryKitItemSystem::MoveItem, return false;);
    
    // 创建物品
    UFUNCTION(BlueprintCallable, Category = "InventoryKit")
    virtual int32 CreateItem(FName ConfigId, FContainerLocation Location) PURE_VIRTUAL(UInventoryKitItemSystem::CreateItem, return -1;);
    
    // 查询指定容器中的所有物品
    UFUNCTION(BlueprintCallable, Category = "InventoryKit")
    virtual TArray<int32> GetItemsInContainer(FContainerLocation Location) const PURE_VIRTUAL(UInventoryKitItemSystem::GetItemsInContainer, return TArray<int32>(););
    
    // 获取物品数据
    UFUNCTION(BlueprintCallable, Category = "InventoryKit")
    virtual FItemBaseInstance GetItemData(int32 ItemId) const PURE_VIRTUAL(UInventoryKitItemSystem::GetItemData, return FItemBaseInstance(););
    
    // 物品位置变更事件
    UPROPERTY(BlueprintAssignable, Category = "InventoryKit")
    FOnItemLocationChanged OnItemLocationChanged;
};

// 物品容器接口
UINTERFACE(MinimalAPI, Blueprintable)
class UInventoryKitContainerInterface : public UInterface
{
    GENERATED_BODY()
};

class IInventoryKitContainerInterface
{
    GENERATED_BODY()
public:
    // 获取容器位置
    virtual FContainerLocation GetContainerLocation() const = 0;
    
    // 检查是否可以添加物品
    virtual bool CanAddItem(int32 ItemId) const = 0;
    
    // 物品添加通知（供物品系统调用）
    virtual void OnItemAdded(int32 ItemId) = 0;
    
    // 物品移除通知（供物品系统调用）
    virtual void OnItemRemoved(int32 ItemId) = 0;
    
    // 获取当前容器中的所有物品
    virtual TArray<int32> GetAllItems() const = 0;
};
```

### 2. 背包组件 (InventoryKitBagComponent)

背包组件负责管理玩家随身携带的物品，主要职责包括：
- 背包空间管理
- 物品存取操作（仅操作物品ID）
- 重量和空间限制
- 物品分类和排序
- 暴露数据和事件接口供UI层订阅
- 维护物品ID的本地缓存

实现方式：
```cpp
UCLASS()
class INVENTORYKIT_API UInventoryKitBagComponent : public UActorComponent, 
                                                 public IInventoryKitContainerInterface
{
    GENERATED_BODY()
private:
    // 背包唯一ID
    UPROPERTY(EditAnywhere, Category = "InventoryKit")
    int32 BagId;
    
    // 物品ID缓存
    UPROPERTY()
    TArray<int32> ItemIds;
    
    // 容量限制
    UPROPERTY(EditAnywhere, Category = "InventoryKit")
    int32 Capacity;
    
public:
    // 获取容器位置
    virtual FContainerLocation GetContainerLocation() const override
    {
        FContainerLocation Location;
        Location.ContainerType = EContainerType::Bag;
        Location.ContainerId = BagId;
        return Location;
    }
    
    // 实现容器接口
    virtual bool CanAddItem(int32 ItemId) const override;
    virtual void OnItemAdded(int32 ItemId) override;
    virtual void OnItemRemoved(int32 ItemId) override;
    virtual TArray<int32> GetAllItems() const override { return ItemIds; }
    
    // 背包特有功能
    UFUNCTION(BlueprintCallable, Category = "InventoryKit")
    bool AddItem(int32 ItemId);
    
    UFUNCTION(BlueprintCallable, Category = "InventoryKit")
    bool RemoveItem(int32 ItemId);
    
    // 事件通知
    UPROPERTY(BlueprintAssignable, Category = "InventoryKit")
    FOnBagChanged OnBagChanged;
};
```

### 3. 仓库系统 (InventoryKitStorageSystem)

仓库系统用于管理长期存储的物品，主要职责包括：
- 大容量物品存储
- 多仓库管理
- 物品传输和共享
- 持久化存储支持
- 安全和权限管理
- 提供数据查询接口供UI层使用

实现与背包类似，但作为系统级服务，使用容器接口管理多个仓库实例。

### 4. 掉落组件 (InventoryKitLootComponent)

掉落组件用于从游戏世界获取物品，以组件形式附加到可掉落物品的对象上，主要职责包括：
- 可掉落物体定义
- 战利品表生成
- 掉落进度和时间控制
- 物品稀有度和掉落机制
- 环境交互支持
- 提供掉落事件接口供UI层响应

### 5. 商店组件 (InventoryKitShopComponent)

商店组件以ActorComponent形式附加到商店NPC或交易点上，负责物品的交易功能，主要职责包括：
- 商店物品库存管理
- 价格计算和调整
- 购买和出售逻辑
- 特殊商品和折扣机制
- 提供交易数据接口供UI层使用

关键特性：
- 每个商店独立的物品库存
- 商店特有的价格策略
- 商品刷新机制
- 货币和交易记录
- 多种支付方式支持

### 6. 装备组件 (InventoryKitEquipmentComponent)

装备组件以ActorComponent形式附加到角色上，负责物品的使用和装备功能，主要职责包括：
- 装备槽位管理
- 装备效果应用
- 装备条件验证
- 装备状态监控
- 提供装备数据和事件接口供UI层使用

关键特性：
- 多槽位支持（头部、身体、手部等）
- 装备套装效果
- 装备限制条件
- 装备属性加成
- 与物品系统无缝集成

### 7. 全局服务 (InventoryKitServices)

为了支持组件间的协作和全局功能，框架提供以下全局服务：

- **InventoryKitCurrencyService** - 处理游戏内货币和交易记录
- **InventoryKitLootService** - 管理战利品表和掉落规则

## 物品定义机制

InventoryKit提供基础物品结构体而非具体类型，让项目自行扩展：

1. **物品基础结构体**：插件提供`FItemBaseInstance`结构体，项目可以继承扩展定义具体物品属性
2. **物品配置集成**：默认支持UE数据表管理物品基础数据，物品实例通过`FName`类型的配置ID引用数据表数据
3. **物品系统基类**：提供`UInventoryKitItemSystem`抽象基类，项目需要继承并实现具体功能
4. **物品位置表示**：使用容器类型和容器ID组合表示物品位置，确保全局唯一定位

```cpp
// 项目物品结构体示例
USTRUCT(BlueprintType)
struct FMyGameItemInstance : public FItemBaseInstance
{
    GENERATED_BODY()
    
    // 物品当前堆叠数量
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 StackCount;
    
    // 物品当前耐久度
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Durability;
    
    // 自定义额外属性
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<FName, float> CustomAttributes;
};

// 项目物品系统实现示例
UCLASS()
class MYGAME_API UMyGameItemSystem : public UInventoryKitItemSystem
{
    GENERATED_BODY()
    
private:
    // 物品实例映射表（使用游戏特定的物品结构）
    TMap<int32, FMyGameItemInstance> GameItems;
    
    // 物品位置映射表
    TMap<int32, FContainerLocation> ItemLocations;
    
    // 物品ID生成器
    int32 NextItemId;
    
public:
    // 初始化
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    
    // 实现抽象方法
    virtual bool GetItemLocation(int32 ItemId, FContainerLocation& OutLocation) const override;
    virtual bool MoveItem(int32 ItemId, FContainerLocation SourceLocation, FContainerLocation TargetLocation) override;
    virtual int32 CreateItem(FName ConfigId, FContainerLocation Location) override;
    virtual TArray<int32> GetItemsInContainer(FContainerLocation Location) const override;
    virtual FItemBaseInstance GetItemData(int32 ItemId) const override;
    
    // 扩展方法（项目特定）
    FMyGameItemInstance* GetGameItemData(int32 ItemId);
    bool UpdateItemDurability(int32 ItemId, float NewDurability);
    bool AddItemStack(int32 ItemId, int32 Count);
};
```

## 插件目录结构

```
InventoryKit/
├── Source/
│   ├── Core/                           # 核心系统
│   │   ├── InventoryKitItemSystem.h         # 物品系统抽象基类
│   │   ├── InventoryKitTypes.h              # 物品基础结构体和类型定义 
│   │   ├── InventoryKitBagComponent.h       # 背包组件
│   │   ├── InventoryKitStorageSystem.h      # 仓库系统
│   │   ├── InventoryKitLootComponent.h      # 掉落组件
│   │   ├── InventoryKitShopComponent.h      # 商店组件
│   │   ├── InventoryKitEquipmentComponent.h # 装备组件
│   │   └── InventoryKitSettings.h           # 系统设置
│   │
│   ├── Services/                       # 全局服务
│   │   ├── InventoryKitCurrencyService.h    # 货币服务
│   │   └── InventoryKitLootService.h        # 战利品服务
│   │
│   ├── Interfaces/                     # 接口定义
│   │   ├── BagInterfaces.h             # 背包接口
│   │   ├── StorageInterfaces.h         # 仓库接口
│   │   ├── LootInterfaces.h            # 掉落接口
│   │   ├── ShopInterfaces.h            # 商店接口
│   │   ├── EquipmentInterfaces.h       # 装备接口
│   │   └── ContainerInterfaces.h       # 容器接口
│   │
│   └── Utilities/                      # 工具类
```

## 使用方法

1. **创建项目物品系统**:
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

2. **定义游戏物品结构**:
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

3. **添加背包组件并订阅事件**:
   ```cpp
   UInventoryKitBagComponent* BagComp = 
       Character->AddComponent<UInventoryKitBagComponent>(TEXT("BagComponent"));
   BagComp->SetBagId(1001);  // 设置背包唯一ID
   BagComp->SetCapacity(20);  // 设置背包容量
   
   // 订阅物品系统事件
   auto ItemSystem = GetWorld()->GetSubsystem<UMyGameItemSystem>();
   ItemSystem->OnItemLocationChanged.AddDynamic(BagComp, &UInventoryKitBagComponent::HandleItemLocationChanged);
   
   // 创建物品并添加到背包
   FContainerLocation BagLocation = BagComp->GetContainerLocation();
   int32 ItemId = ItemSystem->CreateItem(TEXT("Weapon_Sword"), BagLocation);
   ```

4. **组件之间移动物品**:
   ```cpp
   // 从背包移动到装备栏
   auto ItemSystem = GetWorld()->GetSubsystem<UMyGameItemSystem>();
   
   FContainerLocation SourceLocation = BagComp->GetContainerLocation();
   FContainerLocation TargetLocation = EquipComp->GetContainerLocation();
   TargetLocation.SlotIndex = 0;  // 装备到特定槽位
   
   ItemSystem->MoveItem(ItemId, SourceLocation, TargetLocation);
   
   // 物品系统将自动通知背包组件和装备组件更新缓存
   ```

5. **查询容器中的物品**:
   ```cpp
   // 方法1：通过物品系统查询（权威数据，较慢）
   auto ItemSystem = GetWorld()->GetSubsystem<UMyGameItemSystem>();
   FContainerLocation BagLocation = BagComp->GetContainerLocation();
   TArray<int32> BagItems = ItemSystem->GetItemsInContainer(BagLocation);
   
   // 方法2：直接从组件缓存获取（非权威数据，快速访问）
   TArray<int32> CachedItems = BagComp->GetAllItems();
   ```

6. **组件实现容器接口示例**:
   ```cpp
   // 背包组件实现示例
   void UInventoryKitBagComponent::OnItemAdded(int32 ItemId)
   {
       // 添加到本地缓存
       ItemIds.Add(ItemId);
       
       // 触发UI更新事件
       OnBagChanged.Broadcast();
   }
   
   void UInventoryKitBagComponent::OnItemRemoved(int32 ItemId)
   {
       // 从本地缓存移除
       ItemIds.Remove(ItemId);
       
       // 触发UI更新事件
       OnBagChanged.Broadcast();
   }
   
   bool UInventoryKitBagComponent::CanAddItem(int32 ItemId) const
   {
       // 检查背包容量限制
       return ItemIds.Num() < Capacity;
   }
   ```

## 混合模式的优势

采用物品系统作为权威数据源与组件本地缓存相结合的混合模式具有以下优势：

1. **数据一致性**：
   - 物品位置信息只在物品系统中维护为权威数据
   - 避免了多处维护导致的数据不一致问题
   - 物品移动操作统一由物品系统执行，确保正确性

2. **性能优化**：
   - 组件保持物品ID的本地缓存，支持快速访问
   - 减少频繁查询物品系统的开销
   - 常见操作（如UI显示）可直接使用缓存数据

3. **清晰的架构**：
   - 物品系统作为抽象基类，让项目决定具体实现
   - 组件通过统一接口与物品系统交互
   - 事件通知机制使得数据流动清晰可控

4. **扩展性**：
   - 新增组件类型只需实现容器接口
   - 物品系统无需针对新组件做特殊处理
   - 统一的移动和位置管理机制适用于所有组件
   - 项目可以完全决定物品属性和行为的具体实现

## 物品ID与位置管理

为避免数据重复和不一致，物品系统采用以下机制管理物品位置：

1. **物品ID系统**：
   - 每个物品实例由唯一的`FItemHandle`标识
   - 组件间传递物品只使用物品句柄，不直接传递物品实例

2. **位置管理**：
   - 物品系统内部维护`TMap<FItemHandle, FItemLocation>`记录物品位置
   - `FItemLocation`包含位置类型（背包/装备/仓库等）和目标组件引用

3. **位置更新**：
   - 当物品移动时，组件通过物品系统的API更新位置
   - 组件仅记录物品ID缓存，物品实际位置由物品系统统一管理
   - 物品系统通知相关组件更新缓存数据

```cpp
// 物品位置结构示例
struct FItemLocation
{
    EItemLocationType LocationType;  // 位置类型（背包/装备/仓库等）
    UObject* Container;              // 容器组件
    int32 SlotIndex;                 // 槽位索引（可选）
};

// 物品移动示例
bool UInventoryKitItemSystem::MoveItem(const FItemHandle& ItemHandle, UObject* SourceContainer, UObject* TargetContainer)
{
    // 验证物品当前位置
    FItemLocation CurrentLocation;
    if (!GetItemLocation(ItemHandle, CurrentLocation) || CurrentLocation.Container != SourceContainer)
    {
        return false;
    }
    
    // 验证目标容器是否可以接收物品
    IInventoryKitContainerInterface* TargetInterface = 
        Cast<IInventoryKitContainerInterface>(TargetContainer);
    if (!TargetInterface || !TargetInterface->CanAddItem(ItemHandle))
    {
        return false;
    }
    
    // 通知源容器移除物品
    IInventoryKitContainerInterface* SourceInterface = 
        Cast<IInventoryKitContainerInterface>(SourceContainer);
    if (SourceInterface)
    {
        SourceInterface->OnItemRemoved(ItemHandle);
    }
    
    // 通知目标容器添加物品
    TargetInterface->OnItemAdded(ItemHandle);
    
    // 更新物品位置
    FItemLocation NewLocation;
    NewLocation.Container = TargetContainer;
    NewLocation.LocationType = GetLocationTypeForContainer(TargetContainer);
    ItemLocations[ItemHandle] = NewLocation;
    
    // 广播位置变更事件
    OnItemLocationChanged.Broadcast(ItemHandle, SourceContainer, TargetContainer);
    
    return true;
}
```

## 当前功能

### 1. 物品管理
- [x] 物品基础结构体系统
- [x] 物品ID管理机制
- [x] 物品位置表示（容器ID + 类型）
- [x] 物品配置集成
- [x] 物品持久化

### 2. 背包系统
- [x] 物品存取操作
- [x] 重量和空间限制
- [x] 物品排序和筛选
- [x] 背包事件通知

### 3. 仓库系统
- [x] 多仓库支持
- [x] 物品传输功能
- [x] 仓库持久化
- [x] 仓库数据接口

### 4. 掉落系统
- [x] 可掉落对象定义
- [x] 基础战利品表支持
- [x] 掉落进度控制
- [x] 基础掉落机制

### 5. 商店系统
- [x] 独立商店实例
- [x] 购买和出售功能
- [x] 基础价格系统
- [x] 商店库存管理
- [x] 商店数据接口

### 6. 装备系统
- [x] 基础装备槽位
- [x] 装备效果应用
- [x] 装备条件检查
- [x] 装备状态事件

### 7. 全局服务
- [x] 货币和交易管理
- [x] 战利品表配置

## 未来规划

### 1. 高级物品功能
- [ ] 物品套装系统
- [ ] 物品耐久度
- [ ] 物品升级和改造
- [ ] 稀有度和品质系统

### 2. 背包扩展
- [ ] 背包格子布局数据结构
- [ ] 背包扩展机制
- [ ] 背包标签和分类系统
- [ ] 快速物品栏逻辑

### 3. 仓库增强
- [ ] 共享仓库权限系统
- [ ] 仓库自动分类
- [ ] 物品交易系统
- [ ] 远程仓库访问

### 4. 掉落系统增强
- [ ] 动态战利品生成
- [ ] 上下文感知掉落
- [ ] 高级掉落机制（拆解、解锁）
- [ ] 环境互动掉落

### 5. 商店系统扩展
- [ ] 商店NPC行为集成
- [ ] 动态价格波动
- [ ] 声望和关系影响价格
- [ ] 限时特卖和稀有商品
- [ ] 黑市和特殊交易

### 6. 装备系统增强
- [ ] 复杂槽位系统（主武器/副武器）
- [ ] 套装属性加成
- [ ] 装备强化和附魔
- [ ] 装备特殊效果和技能

## 注意事项

1. **性能考虑**:
   - 物品数量对性能的影响
   - 物品ID查询和位置更新的性能优化
   - 合理使用物品实例化
   - 优化物品存储结构
   - 在多商店场景下的内存管理
   - 缓存更新频率与性能平衡

2. **扩展建议**:
   - 合理设计物品结构体，避免过度复杂
   - 自定义物品结构时注意向后兼容
   - 合理设计物品分类和属性
   - 实现物品系统基类时注意线程安全
   - 实现容器接口时正确处理缓存更新

3. **网络同步**:
   - 物品操作的网络复制
   - 物品ID同步策略
   - 多人游戏中的库存同步
   - 服务器权威物品管理
   - 商店交易的安全验证
   - 缓存数据的网络同步机制

4. **UI集成建议**:
   - 使用插件提供的事件和接口开发自定义UI
   - 避免UI逻辑与核心系统耦合
   - 通过数据绑定模式实现UI与数据的同步
   - 使用委托和事件系统响应物品状态变化
   - UI可直接使用组件缓存数据提高性能

5. **设计提示**:
   - 根据游戏类型调整背包限制
   - 平衡掉落系统的奖励机制
   - 设计平衡的商店经济系统
   - 避免装备系统出现过强组合
   - 为不同商店设计特色和差异化