#include "UI/Lobby/CPP_UserWidget_Lobby.h"

#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Kismet/GameplayStatics.h"

UCPP_UserWidget_Lobby::UCPP_UserWidget_Lobby(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	
}

void UCPP_UserWidget_Lobby::NativeConstruct()
{
	Super::NativeConstruct();
	//GI setting
	GI=Cast<UCPP_MultiplayGameInstance>(GetGameInstance());
	if(GI==nullptr)
		return;
	
	//main
	Button_ServerBrowser->OnClicked.AddDynamic(this,&UCPP_UserWidget_Lobby::ServerBrowserClicked);
	Button_ShootingRange->OnClicked.AddDynamic(this,&UCPP_UserWidget_Lobby::ShootingRangeButtonClicked);
	Button_Exit->OnClicked.AddDynamic(this,&UCPP_UserWidget_Lobby::ExitButtonClicked);
	//server
	Button_CreateServer->OnClicked.AddDynamic(this,&UCPP_UserWidget_Lobby::ServerCreateClicked);
	Button_JoinServer->OnClicked.AddDynamic(this,&UCPP_UserWidget_Lobby::ServerJoinClicked);
	Button_Cancel->OnClicked.AddDynamic(this,&UCPP_UserWidget_Lobby::CancelButtonClicked);
	
	//나중에 멀티가면 player index 수정 필요할듯?//UI 전용으로 변경
	UGameplayStatics::GetPlayerController(GetWorld(),0);
	GetOwningPlayer()->SetInputMode(FInputModeUIOnly());

	//켰을때 UI 설정
	ServerPanel->SetVisibility(ESlateVisibility::Hidden);
}

void UCPP_UserWidget_Lobby::ServerBrowserClicked()
{
	MainPanel->SetVisibility(ESlateVisibility::Hidden);
	ServerPanel->SetVisibility(ESlateVisibility::Visible);
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

void UCPP_UserWidget_Lobby::CancelButtonClicked()
{
	MainPanel->SetVisibility(ESlateVisibility::Visible);
	ServerPanel->SetVisibility(ESlateVisibility::Hidden);
}

void UCPP_UserWidget_Lobby::ServerJoinClicked()
{
	if(!IsValid(GI))
		return;
	GI->JoinServer();
}

void UCPP_UserWidget_Lobby::ServerCreateClicked()
{
	if(!IsValid(GI))
		return;
	GI->CreateServer();
}
