#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Tank/Component/CPP_TankUIComponent.h"
#include "CPP_UserWidgetTankSight.generated.h"

UCLASS()
class UECPP_GETOUTMYWAY_API UCPP_UserWidgetTankSight : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	void SetTankType(ETankType value);


	void OnFPViewEffectToggle();
	void OnOpticalChange();
	void SetRangeText(int Range);
	
	void UpdateGunSightPos(FVector2D value);
	
protected:
	UPROPERTY(meta=(BindWidget))
	class UCanvasPanel* SightCanvas;
	
	UPROPERTY(meta=(BindWidget))
	class UImage* SightReticle;
	
	UPROPERTY(meta=(BindWidget))
	class UImage* GunSightReticle;
	
	UPROPERTY(meta=(BindWidget))
	class UImage* FPViewEffect;
	
	UPROPERTY(meta=(BindWidget))
	class UImage* TPSightReticle;

	UPROPERTY(meta=(BindWidget))
	class UVerticalBox* DynamicTextBoxs;
	
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* OpticalText;
	
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* RangeText;
	
private:
	ETankType TankType;
	bool IsFPView = false;
	bool IsZoom = false;
};
