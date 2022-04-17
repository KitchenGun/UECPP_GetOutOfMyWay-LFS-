#include "UI/Tank/Sight/CPP_UserWidgetTankSight.h"

#include "Components/Image.h"
#include "Engine/AssetManager.h"

void UCPP_UserWidgetTankSight::NativeConstruct()
{
	Super::NativeConstruct();

	
}

void UCPP_UserWidgetTankSight::SetTankType(ETankType value)
{
	TankType = value;
	if(TankType == ETankType::M1A1)
	{
		UTexture2D* SightImg = Cast<UTexture2D>(UAssetManager::GetStreamableManager().LoadSynchronous(FSoftObjectPath(L"Texture2D'/Game/VigilanteContent/Vehicles/West_Tank_M1A1Abrams/Materials/UI/M1A1_SIGHT_BACKGROUND.M1A1_SIGHT_BACKGROUND'")));
		UTexture2D* GunSightImg = Cast<UTexture2D>(UAssetManager::GetStreamableManager().LoadSynchronous(FSoftObjectPath(L"Texture2D'/Game/VigilanteContent/Vehicles/West_Tank_M1A1Abrams/Materials/UI/M1A1_Sight.M1A1_Sight'")));

		SightReticle->SetBrushFromTexture(SightImg);
		GunSightReticle->SetBrushFromTexture(GunSightImg);
	}
}
