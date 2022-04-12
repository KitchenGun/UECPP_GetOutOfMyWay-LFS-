#include "UI/Lobby/CPP_UserWidget_Lobby.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

UCPP_UserWidget_Lobby::UCPP_UserWidget_Lobby(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	
}

void UCPP_UserWidget_Lobby::NativeConstruct()
{
	Super::NativeConstruct();

	Button_ServerBrowser->OnClicked.AddDynamic(this,&UCPP_UserWidget_Lobby::ServerBrowserClicked);
	Button_ShootingRange->OnClicked.AddDynamic(this,&UCPP_UserWidget_Lobby::ShootingRangeButtonClicked);
	Button_Exit->OnClicked.AddDynamic(this,&UCPP_UserWidget_Lobby::ExitButtonClicked);

	//���߿� ��Ƽ���� player index ���� �ʿ��ҵ�?//UI �������� ����
	UGameplayStatics::GetPlayerController(GetWorld(),0);
	GetOwningPlayer()->SetInputMode(FInputModeUIOnly());
}

void UCPP_UserWidget_Lobby::ServerBrowserClicked()
{
	
}

void UCPP_UserWidget_Lobby::ShootingRangeButtonClicked()
{
	UGameplayStatics::OpenLevel(this,"TestLevel");
}

void UCPP_UserWidget_Lobby::ExitButtonClicked()
{
	UKismetSystemLibrary::QuitGame(
		GetWorld(),
		UGameplayStatics::GetPlayerController(GetWorld(),0),
		EQuitPreference::Quit,
		false);
}
