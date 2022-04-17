#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CPP_UserWidgetTank.generated.h"

UCLASS()
class UECPP_GETOUTMYWAY_API UCPP_UserWidgetTank : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;


	UPROPERTY(meta=(BindWidget))
	class UCanvasPanel* MainCanvas;
};
