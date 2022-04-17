#include "Tank/Component/CPP_TankUIComponent.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Engine/StreamableManager.h"
#include "Kismet/GameplayStatics.h"
#include "Tank/CPP_Tank_Pawn.h"
#include "UI/Tank/CPP_UserWidgetTank.h"
#include "UI/Tank/Sight/CPP_UserWidgetTankSight.h"

UCPP_TankUIComponent::UCPP_TankUIComponent()
{
	if(GetOwner()==nullptr)
		return;
	if(GetOwner()->GetName().Equals("CPP_M1A1_Pawn"))
	{
		TankUIType = ETankType::M1A1;
	}
}


void UCPP_TankUIComponent::BeginPlay()
{
	Super::BeginPlay();
	if(PlayerCtrl == nullptr)
		PlayerCtrl =Cast<ACPP_Tank_Pawn>(GetOwner())->GetPlayerController();
	
	TankWidget = CreateWidget<UCPP_UserWidgetTank>(PlayerCtrl,TankUIClass);
	SetSightUI();
	TankWidget->AddToViewport();
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


