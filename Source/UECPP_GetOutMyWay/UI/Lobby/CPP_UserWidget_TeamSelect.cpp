
#include "UI/Lobby/CPP_UserWidget_TeamSelect.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UCPP_UserWidget_TeamSelect::NativeConstruct()
{
	Super::NativeConstruct();
	Red->OnClicked.AddDynamic(this,&UCPP_UserWidget_TeamSelect::OnClickRed);
	Blue->OnClicked.AddDynamic(this,&UCPP_UserWidget_TeamSelect::OnClickBlue);
}

void UCPP_UserWidget_TeamSelect::OnClickRed()
{
	TeamInfo = "Red";
	this->RemoveFromViewport();
	//선택이후
	UGameplayStatics::GetPlayerController(GetWorld(),0)->SetInputMode(FInputModeGameOnly());
	UGameplayStatics::GetPlayerController(GetWorld(),0)->bShowMouseCursor = false;
}

void UCPP_UserWidget_TeamSelect::OnClickBlue()
{
	TeamInfo = "Blue";
	this->RemoveFromViewport();
	//선택이후
	UGameplayStatics::GetPlayerController(GetWorld(),0)->SetInputMode(FInputModeGameOnly());
	UGameplayStatics::GetPlayerController(GetWorld(),0)->bShowMouseCursor = false;
}
