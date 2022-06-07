#include "CPP_UserWidget_ServerButton.h"

#include "Components/Button.h"
#include "GameInstance/CPP_MultiplayGameInstance.h"

void UCPP_UserWidget_ServerButton::NativeConstruct()
{
	Super::NativeConstruct();
	//GI setting
	GI=Cast<UCPP_MultiplayGameInstance>(GetGameInstance());
	if(GI==nullptr)
		return;
	//¹ÙÀÎµù
	ServerButton->OnClicked.AddDynamic(this,&UCPP_UserWidget_ServerButton::OnButtonClick);
}

void UCPP_UserWidget_ServerButton::OnButtonClick()
{
	GI->JoinServer(Info);
}
