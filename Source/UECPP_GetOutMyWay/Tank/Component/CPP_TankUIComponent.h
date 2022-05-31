#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CPP_TankUIComponent.generated.h"



UENUM(BlueprintType)
enum class ETankType : uint8
{
	M1A1=0 UMETA(DisplayName="M1A1"),
	T72 UMETA(DisplayName="T72"),
	Max
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UECPP_GETOUTMYWAY_API UCPP_TankUIComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCPP_TankUIComponent();
	//CrossHair UI
	UFUNCTION()
	void FPViewEffectToggle();
	UFUNCTION()
	void ZoomToggle();
	UFUNCTION()
	void SetRangeText(int Range);
	UFUNCTION()
	void UpdateGunSightPos(FVector2D value);
	//HP UI
	UFUNCTION()
	void UpdateTankHP(float HP);
	UFUNCTION()
	void SetAmmo(int ammo);
	
	
protected:
	virtual void BeginPlay() override;
	void SetSightUI();
	
	void SetHPUI();
	
private:
	//새로운 위젯 추가시 BP에 추가 잊지 않기
	UPROPERTY(EditAnywhere,Category="UIClass")
	TSubclassOf<class UCPP_UserWidgetTank> TankUIClass;
	UPROPERTY(EditAnywhere,Category="UIClass")
	TSubclassOf<class UCPP_UserWidgetTankSight> TankSightUIClass;
	UPROPERTY(EditAnywhere,Category="UIClass")
	TSubclassOf<class UCPP_UserWidgetTankHP> TankHPUIClass;
	
	class UCPP_UserWidgetTank* TankWidget;
	class UCPP_UserWidgetTankSight* TankSightWidget;
	class UCPP_UserWidgetTankHP* TankHPWidget;
	ETankType TankUIType;
	
	class ACPP_Tank_Character* Owner;
	APlayerController* PlayerCtrl;
};
