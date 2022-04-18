#include "UI/Tank/Sight/CPP_UserWidgetTankSight.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Engine/AssetManager.h"

void UCPP_UserWidgetTankSight::NativeConstruct()
{
	Super::NativeConstruct();

	UCanvasPanelSlot* SightSlot = Cast<UCanvasPanelSlot>(SightReticle->Slot);
	UCanvasPanelSlot* GunSightSlot = Cast<UCanvasPanelSlot>(GunSightReticle->Slot);
	UCanvasPanelSlot* CommandSightSlot = Cast<UCanvasPanelSlot>(TPSightReticle->Slot);
	
	SightSlot->SetAnchors(FAnchors{0,0,1,1});
	GunSightSlot->SetAnchors(FAnchors{0,0,1,1});
	CommandSightSlot->SetAnchors(FAnchors{0,0,1,1});
	SightSlot->SetOffsets(FMargin{0,0,0,0});
	GunSightSlot->SetOffsets(FMargin{0,75,0,0});
	CommandSightSlot->SetOffsets(FMargin{0,0,0,0});
}

void UCPP_UserWidgetTankSight::SetTankType(ETankType value)
{
	TankType = value;
	if(TankType == ETankType::M1A1)
	{
		UTexture2D* SightImg = Cast<UTexture2D>(UAssetManager::GetStreamableManager().LoadSynchronous(FSoftObjectPath(L"Texture2D'/Game/VigilanteContent/Vehicles/West_Tank_M1A1Abrams/Materials/UI/M1A1_SIGHT_BACKGROUND.M1A1_SIGHT_BACKGROUND'")));
		UTexture2D* GunSightImg = Cast<UTexture2D>(UAssetManager::GetStreamableManager().LoadSynchronous(FSoftObjectPath(L"Texture2D'/Game/VigilanteContent/Vehicles/West_Tank_M1A1Abrams/Materials/UI/M1A1_Sight.M1A1_Sight'")));
		UTexture2D* CommanderSightImg = Cast<UTexture2D>(UAssetManager::GetStreamableManager().LoadSynchronous(FSoftObjectPath(L"Texture2D'/Game/VigilanteContent/Vehicles/West_Tank_M1A1Abrams/Materials/Textures/AimMarker.AimMarker'")));
			
		SightReticle->SetBrushFromTexture(SightImg);
		GunSightReticle->SetBrushFromTexture(GunSightImg);
		TPSightReticle->SetBrushFromTexture(CommanderSightImg);
		
		FPViewEffect->SetVisibility(ESlateVisibility::Hidden);
		GunSightReticle->SetVisibility(ESlateVisibility::Hidden);
		TPSightReticle->SetVisibility(ESlateVisibility::Visible);
	}
}

void UCPP_UserWidgetTankSight::OnFPViewEffectToggle()
{
	IsFPView =IsFPView?false:true;
	if(IsFPView)
	{//1ÀÎÄª
		TPSightReticle->SetVisibility(ESlateVisibility::Hidden);
		FPViewEffect->SetVisibility(ESlateVisibility::Visible);
		GunSightReticle->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{//3ÀÎÄª
		TPSightReticle->SetVisibility(ESlateVisibility::Visible);
		FPViewEffect->SetVisibility(ESlateVisibility::Hidden);
		GunSightReticle->SetVisibility(ESlateVisibility::Hidden);
	}
}
