// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/ContainerInterfaces.h"
#include "UObject/Object.h"
#include "InventoryKitVoidContainer.generated.h"

/**
 * 虚空容器
 */
UCLASS()
class INVENTORYKIT_API UInventoryKitVoidContainer : public UObject, public IInventoryKitContainerInterface
{
	GENERATED_BODY()


protected:
	int32 ID = 0; // 容器ID

	// 物品ID缓存
	UPROPERTY()
	TArray<int32> ItemIds;

	// 容器空间管理器
	UPROPERTY()
	TObjectPtr<UContainerSpaceManager> SpaceManager;
	
public:
	//~ Begin IInventoryKitContainerInterface
	virtual void InitContainer(int32 InContainerID) override;
	virtual const int32 GetContainerID() const override;
	virtual bool CanAddItem(const FItemBaseInstance& InItem, int32 DstSlotIndex) override;
	virtual bool CanMoveItem(const FItemBaseInstance& InItem, int32 DstSlotIndex) override;
	virtual void OnItemAdded(const FItemBaseInstance& InItem) override;
	virtual void OnItemMoved(const FItemLocation& OldLocation, const FItemBaseInstance& InItem) override;
	virtual void OnItemRemoved(const FItemBaseInstance& InItem) override;
	virtual const TArray<int32>& GetAllItems() const override;
	virtual UContainerSpaceManager* GetSpaceManager() override;
	//~ End IInventoryKitContainerInterface

	void SetContainerSpaceConfig(const FContainerSpaceConfig& InConfig)
	{
		ContainerSpaceConfig = InConfig;
	}
	
private:
	FContainerSpaceConfig ContainerSpaceConfig;
	
};
