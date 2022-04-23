#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "GameInstance/CPP_MultiplayGameInstance.h"
#include "CPP_UserWidget_ServerButton.generated.h"

UCLASS()
class UECPP_GETOUTMYWAY_API UCPP_UserWidget_ServerButton : public UUserWidget
{
	GENERATED_BODY()
	virtual void NativeConstruct() override;
	void OnButtonClick();
public:
	FORCEINLINE void SetServerInfo(FServerInfo val)
	{
		Info = val;
		ServerName->SetText(FText::FromString(Info.ServerName));
		ServerCurPlayer->SetText(FText::AsNumber(Info.CurrentPlayers));
		ServerMaxPlayer->SetText(FText::AsNumber(Info.MaxPlayers));
	}
	
	UPROPERTY(meta=(BindWidget))
	class UButton* ServerButton;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* ServerName;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* ServerCurPlayer;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* ServerMaxPlayer;
private:
	//GameInstance
	class UCPP_MultiplayGameInstance* GI;
	FServerInfo Info;
};

