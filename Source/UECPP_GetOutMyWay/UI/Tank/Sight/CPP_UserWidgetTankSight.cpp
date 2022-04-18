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
	UCanvasPanelSlot* TPSightSlot = Cast<UCanvasPanelSlot>(TPSightReticle->Slot);
	
	SightSlot->SetAnchors(FAnchors{0,0,1,1});
	GunSightSlot->SetAnchors(FAnchors{0,0,1,1});
	TPSightSlot->SetAnchors(FAnchors{0,0,1,1});
	SightSlot->SetOffsets(FMargin{0,0,0,0});
	GunSightSlot->SetOffsets(FMargin{0,75,0,0});
	TPSightSlot->SetOffsets(FMargin{0,0,0,0});
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

void UCPP_UserWidgetTankSight::UpdateGunSightPos(FVector2D value)
{
		int32 X,Y;
		GetOwningPlayer()->GetViewportSize(X,Y);
		FVector2D pos = FVector2D{X*0.5f,Y*0.5f};
		value-=pos;
	
		if(IsFPView)
		{
			//GunSightReticle->RenderTransform.Translation = value;
			UCanvasPanelSlot* GunSightSlot = Cast<UCanvasPanelSlot>(GunSightReticle->Slot);
			value.Y += 75;
			GunSightSlot->SetPosition(value);
		}
		else
		{
			//TPSightReticle->RenderTransform.Translation = value;
			UCanvasPanelSlot* TPSightSlot = Cast<UCanvasPanelSlot>(TPSightReticle->Slot);
			TPSightSlot->SetPosition(value);
		}
}
