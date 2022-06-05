#include "Tank/Component/CPP_TankUIComponent.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Engine/Canvas.h"
#include "Engine/StreamableManager.h"
#include "Kismet/GameplayStatics.h"
#include "Tank/CPP_Tank_M1A1.h"
#include "UI/Tank/CPP_UserWidgetTank.h"
#include "UI/Tank/CPP_UserWidgetTankHP.h"
#include "UI/Tank/Sight/CPP_UserWidgetTankSight.h"

UCPP_TankUIComponent::UCPP_TankUIComponent()
{
	
}

void UCPP_TankUIComponent::FPViewEffectToggle()
{
	TankSightWidget->OnFPViewEffectToggle();
}

void UCPP_TankUIComponent::ZoomToggle()
{
	TankSightWidget->OnOpticalChange();
}

void UCPP_TankUIComponent::SetRangeText(int Range)
{
	TankSightWidget->SetRangeText(Range);
}

void UCPP_TankUIComponent::UpdateGunSightPos(FVector2D value)
{
	//TankSightWidget->UpdateGunSightPos(value);
}

void UCPP_TankUIComponent::UpdateTankHP(float HP)
{
	TankHPWidget->SetHP(HP);
}

void UCPP_TankUIComponent::SetAmmo(int Ammo)
{
	TankHPWidget->SetAMMO(Ammo);
}


void UCPP_TankUIComponent::BeginPlay()
{
	if(GetOwner()==nullptr)
		return;

	(Cast<ACPP_Tank_M1A1>(GetOwner()))? Owner = Cast<ACPP_Tank_M1A1>(GetOwner()) : Owner=nullptr;
		
	if(GetOwner()->GetName().Equals("CPP_Tank_M1A1"))
	{
		TankUIType = ETankType::M1A1;
	}
	
	Super::BeginPlay();
	if(PlayerCtrl == nullptr)
		PlayerCtrl =GEngine->GetFirstLocalPlayerController(GetWorld());
	if(IsValid(Owner))
	{
		Owner->FpViewToggleFunc.BindUFunction(this,"FPViewEffectToggle");
		Owner->FGunSightPosFunc.BindUFunction(this,"UpdateGunSightPos");
		Owner->FZoomToggleFunc.BindUFunction(this,"ZoomToggle");
		Owner->FSetRangeTextFunc.BindUFunction(this,"SetRangeText");
		Owner->FSetHPFunc.BindUFunction(this,"UpdateTankHP");
		Owner->GetGunSystem()->FSetAmmoFunc.BindUFunction(this,"SetAmmo");
	}
	if(Owner->IsLocallyControlled())
	{
		TankWidget = CreateWidget<UCPP_UserWidgetTank>(PlayerCtrl,TankUIClass);
		SetSightUI();
		SetHPUI();
		TankWidget->AddToViewport();
	}
}

void UCPP_TankUIComponent::SetSightUI()
{
	TankSightWidget = CreateWidget<UCPP_UserWidgetTankSight>(PlayerCtrl,TankSightUIClass);
	TankSightWidget->AddToViewport();
	TankWidget->MainCanvas->AddChild(TankSightWidget);
	UCanvasPanelSlot* TankSightWidgetSlot = Cast<UCanvasPanelSlot>(TankWidget->MainCanvas->GetSlots()[0]);
	if(TankUIType==ETankType::M1A1)
	{
		TankSightWidgetSlot->SetAnchors(FAnchors{0,0,1,1});
		TankSightWidgetSlot->SetOffsets(FMargin{0,0,0,0});
	}
	TankSightWidget->SetTankType(TankUIType);
}

void UCPP_TankUIComponent::SetHPUI()
{
	TankHPWidget = CreateWidget<UCPP_UserWidgetTankHP>(PlayerCtrl,TankHPUIClass);
	TankHPWidget->AddToViewport();
	TankWidget->MainCanvas->AddChild(TankHPWidget);
	UCanvasPanelSlot* TankHPWidgetSlot = Cast<UCanvasPanelSlot>(TankWidget->MainCanvas->GetSlots()[1]);
	TankHPWidgetSlot->SetSize(FVector2D(420,220));
	TankHPWidgetSlot->SetPosition(FVector2D(-420,-220));
	TankHPWidgetSlot->SetAnchors(FAnchors(1,1,1,1));
}


