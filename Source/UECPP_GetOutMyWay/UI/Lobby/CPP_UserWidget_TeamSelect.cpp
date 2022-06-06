
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
	PC = Cast<ACPP_Tank_PC>(GetWorld()->GetFirstPlayerController());
	TeamInfo = TEXT("Red");
	PC->SelectRedTeam();
	//��������
	if(PC->IsLocalController())
	{
		Canvas->SetVisibility(ESlateVisibility::Hidden);
		//���� ��ǲ ����
		UGameplayStatics::GetPlayerController(GetWorld(),0)->bShowMouseCursor = false;
		UGameplayStatics::GetPlayerController(GetWorld(),0)->SetInputMode(FInputModeGameOnly());
	}
}

void UCPP_UserWidget_TeamSelect::OnClickBlue()
{
	PC = Cast<ACPP_Tank_PC>(GetWorld()->GetFirstPlayerController());
	TeamInfo = TEXT("Blue");
	PC->SelectBlueTeam();
	//��������
	if(PC==UGameplayStatics::GetPlayerController(GetWorld(),0)&&PC->IsLocalPlayerController())
	{
		Canvas->SetVisibility(ESlateVisibility::Hidden);
		//���� ��ǲ ����
		UGameplayStatics::GetPlayerController(GetWorld(),0)->bShowMouseCursor = false;
		UGameplayStatics::GetPlayerController(GetWorld(),0)->SetInputMode(FInputModeGameOnly());
	}
}


