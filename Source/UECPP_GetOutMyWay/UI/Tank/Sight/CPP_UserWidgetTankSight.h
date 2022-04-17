#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Tank/Component/CPP_TankUIComponent.h"
#include "Tank/Component/CPP_TankUIComponent.h"
#include "CPP_UserWidgetTankSight.generated.h"

UCLASS()
class UECPP_GETOUTMYWAY_API UCPP_UserWidgetTankSight : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	void SetTankType(ETankType value);


	UFUNCTION()
	void OnFPViewEffectToggle();
	
protected:
	UPROPERTY(meta=(BindWidget))
	class UCanvasPanel* SightCanvas;
	UPROPERTY(meta=(BindWidget))
	class UImage* SightReticle;
	UPROPERTY(meta=(BindWidget))
	class UImage* GunSightReticle;
	UPROPERTY(meta=(BindWidget))
	class UImage* FPViewEffect;
	
private:
	ETankType TankType;
};
