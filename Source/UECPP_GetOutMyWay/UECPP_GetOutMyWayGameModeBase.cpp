#include "UECPP_GetOutMyWayGameModeBase.h"
#include "Tank/CPP_Tank_PC.h"
#include "Blueprint/UserWidget.h"
#include "GameInstance/CPP_MultiplayGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Tank/CPP_Tank_Character.h"
#include "UI/Lobby/CPP_UserWidget_TeamSelect.h"

AUECPP_GetOutMyWayGameModeBase::AUECPP_GetOutMyWayGameModeBase()
{
	Player = ConstructorHelpers::FClassFinder<APawn>(L"Blueprint'/Game/BP/Tank/BP_M1A1_Character.BP_M1A1_Character_C'").Class;
	PlayerControllerClass = ConstructorHelpers::FClassFinder<ACPP_Tank_PC>(L"Blueprint'/Game/BP/BP_Tank_PC.BP_Tank_PC_C'").Class;
}

void AUECPP_GetOutMyWayGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	GI = Cast<UCPP_MultiplayGameInstance>(GetGameInstance());
}

//_Implementation
void AUECPP_GetOutMyWayGameModeBase::Spawn_Implementation(const FString& Team, ACPP_Tank_PC* PC)
{
	class ACPP_Tank_Character* myTank;
	FTransform SpawnTrans;
	
	DefaultPawnClass = Player;
	if(Team.Equals("Red"))
	{
		//������
		SpawnTrans = GI->GetRedTeamSpawnPoints()[0]->GetTransform();
		if(SpawnTrans.IsValid())
		{
			myTank = GetWorld()->SpawnActor<ACPP_Tank_Character>(DefaultPawnClass,SpawnTrans.GetLocation(),SpawnTrans.Rotator());
			GEngine->AddOnScreenDebugMessage(-1,5.0f,FColor::White,L"ownerchange");
			PC->SetOwner(myTank);
			PC->Possess(myTank);
		}
		else
		{
			while (SpawnTrans.IsValid())
			{
				GI->FindSpawnPoint();
				SpawnTrans = GI->GetRedTeamSpawnPoints()[0]->GetTransform();
			}
			myTank = GetWorld()->SpawnActor<ACPP_Tank_Character>(DefaultPawnClass,SpawnTrans.GetLocation(),SpawnTrans.Rotator());
			GEngine->AddOnScreenDebugMessage(-1,5.0f,FColor::White,L"ownerchange");
			PC->SetOwner(myTank);
			PC->Possess(myTank);
		}
	}
	else
	{//�Ķ���
		SpawnTrans = GI->GetBlueTeamSpawnPoints()[0]->GetTransform();
		if(SpawnTrans.IsValid())
		{
			myTank = GetWorld()->SpawnActor<ACPP_Tank_Character>(DefaultPawnClass,SpawnTrans.GetLocation(),SpawnTrans.Rotator());
			GEngine->AddOnScreenDebugMessage(-1,5.0f,FColor::White,L"ownerchange");
			PC->SetOwner(myTank);
			PC->Possess(myTank);
		}
		else
		{
			while (SpawnTrans.IsValid())
			{
				GI->FindSpawnPoint();
				SpawnTrans = GI->GetBlueTeamSpawnPoints()[0]->GetTransform();
			}
			myTank = GetWorld()->SpawnActor<ACPP_Tank_Character>(DefaultPawnClass,SpawnTrans.GetLocation(),SpawnTrans.Rotator());
			GEngine->AddOnScreenDebugMessage(-1,5.0f,FColor::White,L"ownerchange");
			PC->SetOwner(myTank);
			PC->Possess(myTank);
		}
	}
}

