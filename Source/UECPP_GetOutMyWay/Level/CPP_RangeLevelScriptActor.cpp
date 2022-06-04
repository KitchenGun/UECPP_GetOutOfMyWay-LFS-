#include "Level/CPP_RangeLevelScriptActor.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Lobby/CPP_UserWidget_TeamSelect.h"

ACPP_RangeLevelScriptActor::ACPP_RangeLevelScriptActor()
{
	BP_SelectWidget = ConstructorHelpers::FClassFinder<UCPP_UserWidget_TeamSelect> (L"WidgetBlueprint'/Game/BP/UI/Lobby/WB_TeamSelect.WB_TeamSelect_C'").Class;
}

void ACPP_RangeLevelScriptActor::BeginPlay()
{
	Super::BeginPlay();
	
	//∆¿º±≈√	
	//CreateWidget<UCPP_UserWidget_TeamSelect>(GetWorld(),BP_SelectWidget)->AddToViewport();
	
	UGameplayStatics::GetPlayerController(GetWorld(),0)->SetInputMode(FInputModeGameOnly());
	UGameplayStatics::GetPlayerController(GetWorld(),0)->bShowMouseCursor = false;
}
