
#include "UI/Lobby/CPP_UserWidget_TeamSelect.h"

#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Kismet/GameplayStatics.h"

void UCPP_UserWidget_TeamSelect::NativeConstruct()
{
	Super::NativeConstruct();
	Red->OnClicked.AddDynamic(this,&UCPP_UserWidget_TeamSelect::OnClickRed);
	Blue->OnClicked.AddDynamic(this,&UCPP_UserWidget_TeamSelect::OnClickBlue);
	PC = Cast<ACPP_Tank_PC>(GetWorld()->GetFirstPlayerController());
	
}

void UCPP_UserWidget_TeamSelect::OnClickRed()
{
	TeamInfo = TEXT("Red");
	PC->SelectRedTeam();
	//선택이후
	if(PC->IsLocalController())
		Canvas->SetVisibility(ESlateVisibility::Hidden);
	
}

void UCPP_UserWidget_TeamSelect::OnClickBlue()
{
	TeamInfo = TEXT("Blue");
	PC->SelectBlueTeam();
	//선택이후
	if(PC->IsLocalController())
		Canvas->SetVisibility(ESlateVisibility::Hidden);
}


