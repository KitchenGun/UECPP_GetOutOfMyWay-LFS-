#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CPP_UserWidgetTankHP.generated.h"

UCLASS()
class UECPP_GETOUTMYWAY_API UCPP_UserWidgetTankHP : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void SetHP(float value);
	UFUNCTION()
	void SetAMMO(int value);
protected:
	UPROPERTY(meta=(BindWidget))
	class UCanvasPanel* Canvas;
	
	UPROPERTY(meta=(BindWidget))
	class UImage* HPBackGround;
	
	UPROPERTY(meta=(BindWidget))
	class UVerticalBox* Vertical;
	//hp
	UPROPERTY(meta=(BindWidget))
	class UHorizontalBox* HPHorizontalBox;

	UPROPERTY(meta=(BindWidget))
	class UImage* RedCross;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* HP;

	UPROPERTY(meta =(BindWidget))
	class UProgressBar* HPVisualBar;
	//ammo
	UPROPERTY(meta=(BindWidget))
	class UHorizontalBox* AMMOHorizontalBox;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Ammo;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Slash;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* MaxAmmoCount;
};
