#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Tank/CPP_Tank_PC.h"
#include "CPP_UserWidget_TeamSelect.generated.h"

UCLASS()
class UECPP_GETOUTMYWAY_API UCPP_UserWidget_TeamSelect : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void OnClickRed();
	UFUNCTION()
	void OnClickBlue();
private:
	UPROPERTY(meta=(BindWidget))
	class UCanvasPanel* Canvas;
	UPROPERTY(meta=(BindWidget))
	class UHorizontalBox* Horizontal;
	UPROPERTY(meta=(BindWidget))
	class UButton* Red;
	UPROPERTY(meta=(BindWidget))
	class UButton* Blue;

	ACPP_Tank_PC* PC;
	FString TeamInfo = "";
};
