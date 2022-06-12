#include "Tank/Component/CPP_TankUIComponent.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Engine/Canvas.h"
#include "Engine/StreamableManager.h"
#include "Kismet/GameplayStatics.h"
#include "Tank/CPP_Tank_M1A1.h"
#include  "Tank/CPP_Tank_PC.h"
#include "UI/Tank/CPP_UserWidgetTank.h"
#include "UI/Tank/CPP_UserWidgetTankHP.h"
#include "UI/Tank/Sight/CPP_UserWidgetTankSight.h"

UCPP_TankUIComponent::UCPP_TankUIComponent()
{
	TankUIClass=ConstructorHelpers::FClassFinder<UCPP_UserWidgetTank>(L"WidgetBlueprint'/Game/BP/UI/Tank/WB_WidgetTank.WB_WidgetTank_C'").Class;
	TankSightUIClass=ConstructorHelpers::FClassFinder<UCPP_UserWidgetTankSight>(L"WidgetBlueprint'/Game/BP/UI/Tank/WB_WidgetTankSight.WB_WidgetTankSight_C'").Class;
	TankHPUIClass=ConstructorHelpers::FClassFinder<UCPP_UserWidgetTankHP>(L"WidgetBlueprint'/Game/BP/UI/Tank/WB_WidgetTankHP.WB_WidgetTankHP_C'").Class;
}

void UCPP_TankUIComponent::FPViewEffectToggle()
{
	if(IsValid(TankSightWidget))
		TankSightWidget->OnFPViewEffectToggle();
}

void UCPP_TankUIComponent::ZoomToggle()
{
	if(IsValid(TankSightWidget))
		TankSightWidget->OnOpticalChange();
}

void UCPP_TankUIComponent::SetRangeText(int Range)
{
	if(IsValid(TankSightWidget))
		TankSightWidget->SetRangeText(Range);
}

void UCPP_TankUIComponent::UpdateGunSightPos(FVector2D value)
{
	if(IsValid(TankSightWidget))
		TankSightWidget->UpdateGunSightPos(value);
}

void UCPP_TankUIComponent::UpdateTankHP(float HP)
{
	if(IsValid(TankHPWidget))
		TankHPWidget->SetHP(HP);
}

void UCPP_TankUIComponent::SetAmmo(int Ammo)
{
	if(IsValid(TankHPWidget))
		TankHPWidget->SetAMMO(Ammo);
}


void UCPP_TankUIComponent::BeginPlay()
{
	Super::BeginPlay();
	Owner=(Cast<ACPP_Tank_M1A1>(GetOwner()))? Cast<ACPP_Tank_M1A1>(GetOwner()) : Owner=nullptr;
	if(GetOwner()==nullptr)
		return;
	PC = Owner->GetPC();
	
	if(GetOwner()->GetName().Equals("CPP_Tank_M1A1"))
	{
		TankUIType = ETankType::M1A1;
	}
	
	if(IsValid(Owner))
	{
		Owner->FpViewToggleFunc.BindUFunction(this,"FPViewEffectToggle");
		Owner->FGunSightPosFunc.BindUFunction(this,"UpdateGunSightPos");
		Owner->FZoomToggleFunc.BindUFunction(this,"ZoomToggle");
		Owner->FSetRangeTextFunc.BindUFunction(this,"SetRangeText");
		Owner->FSetHPFunc.BindUFunction(this,"UpdateTankHP");
		Owner->GetGunSystem()->FSetAmmoFunc.BindUFunction(this,"SetAmmo");
	}
	

}

void UCPP_TankUIComponent::SetBasicUI()
{
	PC = Owner->GetPC();
	TankWidget = CreateWidget<UCPP_UserWidgetTank>(PC,TankUIClass);
	SetSightUI();
	SetHPUI();
	TankWidget->AddToViewport();
}

void UCPP_TankUIComponent::SetSightUI()
{
	
	TankSightWidget = CreateWidget<UCPP_UserWidgetTankSight>(PC,TankSightUIClass);
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
	
	TankHPWidget = CreateWidget<UCPP_UserWidgetTankHP>(PC,TankHPUIClass);
	TankHPWidget->AddToViewport();
	TankWidget->MainCanvas->AddChild(TankHPWidget);
	UCanvasPanelSlot* TankHPWidgetSlot = Cast<UCanvasPanelSlot>(TankWidget->MainCanvas->GetSlots()[1]);
	TankHPWidgetSlot->SetSize(FVector2D(420,220));
	TankHPWidgetSlot->SetPosition(FVector2D(-420,-220));
	TankHPWidgetSlot->SetAnchors(FAnchors(1,1,1,1));
	
}


