#include "Tank/CPP_Tank_PC.h"
#include "UECPP_GetOutMyWayGameModeBase.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Lobby/CPP_UserWidget_TeamSelect.h"


ACPP_Tank_PC::ACPP_Tank_PC()
{
	BP_SelectWidget = ConstructorHelpers::FClassFinder<UCPP_UserWidget_TeamSelect> (L"WidgetBlueprint'/Game/BP/UI/Lobby/WB_TeamSelect.WB_TeamSelect_C'").Class;
}

void ACPP_Tank_PC::BeginPlay()
{
	Super::BeginPlay();
	GM = Cast<AUECPP_GetOutMyWayGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	if(IsLocalController())
	{
		//∆¿º±≈√	
		CreateWidget<UCPP_UserWidget_TeamSelect>(GetWorld()->GetFirstPlayerController(),BP_SelectWidget)->AddToViewport();
		//UI ¿Œ«≤ ¿¸øÎ
		UGameplayStatics::GetPlayerController(GetWorld(),0)->SetInputMode(FInputModeUIOnly());
		UGameplayStatics::GetPlayerController(GetWorld(),0)->bShowMouseCursor = true;
	}
}

void ACPP_Tank_PC::TeamSelect()
{
	if(HasAuthority())
	{
		GM->Spawn(Team,this);
	}
	else
	{
		Server_TeamSelect();
	}
}

void ACPP_Tank_PC::Server_TeamSelect_Implementation()
{
	GM->Spawn(Team,this);
}

void ACPP_Tank_PC::SelectRedTeam()
{
	Team = TEXT("Red");
	TeamSelect();
}

void ACPP_Tank_PC::SelectBlueTeam()
{
	Team = TEXT("Blue");
	TeamSelect();
}
