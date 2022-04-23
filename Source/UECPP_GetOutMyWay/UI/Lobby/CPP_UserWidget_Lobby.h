#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameInstance/CPP_MultiplayGameInstance.h"
#include "CPP_UserWidget_Lobby.generated.h"

UCLASS()
class UECPP_GETOUTMYWAY_API UCPP_UserWidget_Lobby : public UUserWidget
{
	GENERATED_BODY()
public:
	UCPP_UserWidget_Lobby(const FObjectInitializer &ObjectInitializer);
protected:
	virtual void NativeConstruct() override;

private:
	//main
	UFUNCTION()
	void ServerBrowserClicked();
	UFUNCTION()
	void ShootingRangeButtonClicked();
	UFUNCTION()
	void ExitButtonClicked();
	//server
	UFUNCTION()
	void CancelButtonClicked();
	UFUNCTION()
	void ServerCreateClicked();
	UFUNCTION()
	void ServerListRefreshClicked();

	UFUNCTION()
	void AddServerList(FServerInfo Info);
	
private:
	//Main
	UPROPERTY(meta=(BindWidget))
	class UCanvasPanel* MainPanel;
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_ShootingRange;//���߿� ��Ƽ ���� �ٲ����
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_Exit;
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_ServerBrowser;
	//Server
	UPROPERTY(meta=(BindWidget))
	class UCanvasPanel* ServerPanel;
	UPROPERTY(meta=(BindWidget))
	class UScrollBox* ServerList;
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_CreateServer;
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_Cancel;
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_Refresh;

	UPROPERTY(EditAnywhere,Category="UIClass")
	TSubclassOf<class UCPP_UserWidget_ServerButton> ServerButtonUIClass;
	
	//delegate
	FServerInfo ServerInfo;
	
	//GameInstance
	class UCPP_MultiplayGameInstance* GI;
	
};
