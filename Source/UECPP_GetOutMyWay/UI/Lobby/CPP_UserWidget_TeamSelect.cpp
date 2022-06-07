
#include "UI/Lobby/CPP_UserWidget_TeamSelect.h"

#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Kismet/GameplayStatics.h"

void UCPP_UserWidget_TeamSelect::NativeConstruct()
{
	Super::NativeConstruct();
	Red->OnClicked.AddDynamic(this,&UCPP_UserWidget_TeamSelect::OnClickRed);
	Blue->OnClicked.AddDynamic(this,&UCPP_UserWidget_TeamSelect::OnClickBlue);
}

void UCPP_UserWidget_TeamSelect::OnClickRed()
{
	PC = Cast<ACPP_Tank_PC>(GetOwningLocalPlayer()->GetPlayerController(GetWorld()));
	if(PC->IsLocalController())
	{
		TeamInfo = TEXT("Red");
		PC->SelectRedTeam();
		//선택이후
		Canvas->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UCPP_UserWidget_TeamSelect::OnClickBlue()
{
	if (PC->IsLocalController())
	{
		TeamInfo = TEXT("Blue");
		PC->SelectBlueTeam();
		//선택이후
		Canvas->SetVisibility(ESlateVisibility::Hidden);
	}
}


