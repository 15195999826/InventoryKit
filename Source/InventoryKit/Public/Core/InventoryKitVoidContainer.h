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

public:
	//~ Begin IInventoryKitContainerInterface
	virtual const int32 GetContainerID() const override;
	virtual bool CanAddItem(int32 ItemId) const override;
	virtual void OnItemAdded(int32 ItemId) override;
	virtual void OnItemRemoved(int32 ItemId) override;
	virtual const TArray<int32>& GetAllItems() const override;
	//~ End IInventoryKitContainerInterface

	void SetContainerID(int32 NewContainerID)
	{
		ID = NewContainerID;
	}
};
