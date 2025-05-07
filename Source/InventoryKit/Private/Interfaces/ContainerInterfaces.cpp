#include "Interfaces/ContainerInterfaces.h"
#include "ContainerSpace/ContainerSpaceManager.h"
#include "ContainerSpace/FixedSlotSpaceManager.h"
#include "ContainerSpace/GridSpaceManager.h"
#include "ContainerSpace/UnorderedSpaceManager.h"

UContainerSpaceManager* IInventoryKitContainerInterface::CreateSpaceManager(UObject* InOuter,
                                                                            const FContainerSpaceConfig& InConfig)
{
	UContainerSpaceManager* Ret = nullptr;
	switch (InConfig.SpaceType) {
		case EContainerSpaceType::Unordered:
			{
				Ret = NewObject<UUnorderedSpaceManager>(InOuter);
			}
			break;
		case EContainerSpaceType::Fixed:
			{
				Ret = NewObject<UFixedSlotSpaceManager>(InOuter);
			}
			break;
		case EContainerSpaceType::Grid:
			{
				Ret = NewObject<UGridSpaceManager>(InOuter);
			}
			break;
	}

	check(Ret);
	Ret->Initialize(InConfig);

	return Ret;
}
