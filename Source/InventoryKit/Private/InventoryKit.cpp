// Copyright Epic Games, Inc. All Rights Reserved.

#include "InventoryKit.h"
#include "Core/InventoryKitTypes.h"
#include "Core/InventoryKitItemSystem.h"
#include "Core/InventoryKitBagComponent.h"
#include "Core/InventoryKitEquipmentComponent.h"
#include "Core/InventoryKitLootComponent.h"
#include "Interfaces/ContainerInterfaces.h"

#define LOCTEXT_NAMESPACE "FInventoryKitModule"

void FInventoryKitModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	UE_LOG(LogTemp, Log, TEXT("InventoryKit Module Started"));
}

void FInventoryKitModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	UE_LOG(LogTemp, Log, TEXT("InventoryKit Module Shutdown"));
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FInventoryKitModule, InventoryKit)