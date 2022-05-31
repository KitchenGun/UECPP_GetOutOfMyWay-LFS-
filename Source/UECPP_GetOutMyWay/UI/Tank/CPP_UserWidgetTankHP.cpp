#include "UI/Tank/CPP_UserWidgetTankHP.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UCPP_UserWidgetTankHP::NativeConstruct()
{
	Super::NativeConstruct();
}

void UCPP_UserWidgetTankHP::SetHP(float value)
{
	HPVisualBar->SetPercent(value/100);
	HP->SetText(FText::FromString(FString::FormatAsNumber(static_cast<int>(value))));
}

void UCPP_UserWidgetTankHP::SetAMMO(int value)
{
	Ammo->SetText(FText::FromString(FString::FormatAsNumber(value)));
}
