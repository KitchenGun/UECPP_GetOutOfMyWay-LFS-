#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CPP_UserWidgetTankSight.generated.h"

UCLASS()
class UECPP_GETOUTMYWAY_API UCPP_UserWidgetTankSight : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	
};
