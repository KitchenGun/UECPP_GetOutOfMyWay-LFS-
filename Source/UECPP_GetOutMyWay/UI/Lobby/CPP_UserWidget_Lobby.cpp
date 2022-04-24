#include "UI/Lobby/CPP_UserWidget_Lobby.h"
#include "CPP_UserWidget_ServerButton.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/ScrollBox.h"
#include "Kismet/GameplayStatics.h"

UCPP_UserWidget_Lobby::UCPP_UserWidget_Lobby(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	ServerButtonUIClass = ConstructorHelpers::FClassFinder<UCPP_UserWidget_ServerButton>
		(L"WidgetBlueprint'/Game/BP/UI/Lobby/WB_ServerButton.WB_ServerButton_C'").Class;
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
	Button_Cancel->OnClicked.AddDynamic(this,&UCPP_UserWidget_Lobby::CancelButtonClicked);
	Button_Refresh->OnClicked.AddDynamic(this,&UCPP_UserWidget_Lobby::ServerListRefreshClicked);
	
	//input UI 전용으로 변경
	UGameplayStatics::GetPlayerController(GetWorld(),0);
	GetOwningPlayer()->SetInputMode(FInputModeUIOnly());

	//켰을때 UI 설정
	ServerPanel->SetVisibility(ESlateVisibility::Hidden);

	//바인딩
	GI->FServerListDel.AddDynamic(this,&UCPP_UserWidget_Lobby::AddServerList);
}

void UCPP_UserWidget_Lobby::ServerBrowserClicked()
{
	MainPanel->SetVisibility(ESlateVisibility::Hidden);
	ServerPanel->SetVisibility(ESlateVisibility::Visible);
	//서버 탐색
	ServerListRefreshClicked();
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

void UCPP_UserWidget_Lobby::ServerCreateClicked()
{
	if(!IsValid(GI))
		return;
	GI->CreateServer();
}

void UCPP_UserWidget_Lobby::ServerListRefreshClicked()
{
	if(!IsValid(GI))
		return;
	//기존의 서버 버튼삭제 해야함
	for(auto button:ServerList->GetAllChildren())
	{
		button->RemoveFromParent();
	}
	
	GI->FindServer();
}

void UCPP_UserWidget_Lobby::AddServerList(FServerInfo Info)
{
	class UCPP_UserWidget_ServerButton* ServerButton;
	ServerButton = CreateWidget<class UCPP_UserWidget_ServerButton>
	(UGameplayStatics::GetPlayerController(GetWorld(),0),ServerButtonUIClass);
	ServerButton->SetServerInfo(Info);
	ServerButton->AddToViewport();
	ServerList->AddChild(ServerButton);
}
