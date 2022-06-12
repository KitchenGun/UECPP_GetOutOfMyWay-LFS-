#include "Tank/CPP_Tank_PC.h"

#include "CPP_Tank_Character.h"
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
	GM = Cast<AUECPP_GetOutMyWayGameModeBase>(GetWorld()->GetAuthGameMode());

	//∆¿º±≈√
	if(IsLocalPlayerController())
	{
		SelectWidget = CreateWidget<UCPP_UserWidget_TeamSelect>(this,BP_SelectWidget);
		SelectWidget->PC = this;
		SelectWidget->AddToViewport();
		//UI ¿Œ«≤ ¿¸øÎ
		SetInputMode(FInputModeUIOnly());
		bShowMouseCursor = true;
	}
}

void ACPP_Tank_PC::TeamSelect(FString TeamName)
{
	if(!IsLocalController())
		return;
	Team = TeamName;
	if(HasAuthority())
	{
		GM->Spawn(Team,this);
	}
	else
	{
		Server_TeamSelect();
	}
	//∞‘¿” ¿Œ«≤ ¿¸øÎ
	bShowMouseCursor = false;
	SetInputMode(FInputModeGameOnly());
}

void ACPP_Tank_PC::Server_TeamSelect_Implementation()
{
	GM->Spawn(Team,this);
}

void ACPP_Tank_PC::SelectRedTeam()
{
	Team = TEXT("Red");
	TeamSelect(Team);
}

void ACPP_Tank_PC::SelectBlueTeam()
{
	Team = TEXT("Blue");
	TeamSelect(Team);
}

void ACPP_Tank_PC::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void ACPP_Tank_PC::OnUnPossess()
{
	Super::OnUnPossess();
}

