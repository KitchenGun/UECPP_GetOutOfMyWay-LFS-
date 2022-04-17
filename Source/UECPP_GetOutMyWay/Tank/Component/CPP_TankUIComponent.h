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
	UFUNCTION()
	void FPViewEffectToggle();

protected:
	virtual void BeginPlay() override;
	void SetSightUI();

	
private:
	UPROPERTY(EditAnywhere,Category="UIClass")
	TSubclassOf<class UCPP_UserWidgetTank> TankUIClass;
	UPROPERTY(EditAnywhere,Category="UIClass")
	TSubclassOf<class UCPP_UserWidgetTankSight> TankSightUIClass;

	
	class UCPP_UserWidgetTank* TankWidget;
	class UCPP_UserWidgetTankSight* TankSightWidget;
	ETankType TankUIType;
	
	class ACPP_Tank_Pawn* Owner;
	APlayerController* PlayerCtrl;
};
