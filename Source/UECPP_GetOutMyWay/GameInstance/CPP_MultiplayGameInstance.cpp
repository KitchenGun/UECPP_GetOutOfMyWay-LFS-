#include "CPP_MultiplayGameInstance.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "Common/UObject/Manager/ObjectPool/CPP_ObjectPoolManager.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"

UCPP_MultiplayGameInstance::UCPP_MultiplayGameInstance()
{
}

void UCPP_MultiplayGameInstance::BeginPlay()
{
}

void UCPP_MultiplayGameInstance::Init()
{//�÷��� �����Ҷ� 
	Super::Init();
	//server
	if(IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get())
	{
		SessionInterface = Subsystem->GetSessionInterface();
		if(SessionInterface.IsValid())
		{
			//Bind Delegates
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this,&UCPP_MultiplayGameInstance::OnCreateSessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this,&UCPP_MultiplayGameInstance::OnFindSessionComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this,&UCPP_MultiplayGameInstance::OnJoinSessionComplete);
		}
	}
	
	//objectpoolmanager ����
	RegisterManagerClass(UCPP_ObjectPoolManager::StaticClass());
	
	//beginplay�����
	for(auto & IT : ManagerClasses)
	{
		IT.Value->BeginPlay();
	}
}

void UCPP_MultiplayGameInstance::Shutdown()
{
	//�÷��� ���⶧
	for (auto pair : ManagerClasses)
	{
		if(!IsValid(pair.Value))
			return;
		//IsValid ������ ����Ű�� ������Ʈ ����?
		//IsValidLowlevel,Fast ���� ������ �����ִ���?
		if (!pair.Value->IsValidLowLevel()) return;
		
		pair.Value->ShutdownManagerClass();

		// ��ü �Ҹ�
		pair.Value->ConditionalBeginDestroy();//������ �÷��Ͱ� ����ɶ� �� ������ �����Ҷ� ����
		//GetWorld()->ForceGarbageCollection(); //������ �÷��Ͱ� ��� �۵��ϵ��� �ϴ� �Լ�
	}
	ManagerClasses.Empty();
	
	Super::Shutdown();
}

void UCPP_MultiplayGameInstance::OnCreateSessionComplete(FName ServerName, bool Succeeded)
{
	UE_LOG(LogTemp,Display,L"OnCreateSessionComplete, Succeeded %d",Succeeded);
	if(Succeeded)
	{
		GetWorld()->ServerTravel("/Game/Level/TestMap?listen");
	}
}

void UCPP_MultiplayGameInstance::OnFindSessionComplete(bool Succeeded)
{
	UE_LOG(LogTemp,Display,L"OnFindSessionComplete, Succeeded %d",Succeeded);
	if(Succeeded)
	{
		SearchResults = SessionSearch->SearchResults;
		int32 index =0;
		for(FOnlineSessionSearchResult Result : SearchResults)
		{
			if(!Result.IsValid())
				continue;
			FServerInfo Info;
			Info.ServerName = Result.Session.SessionSettings.Settings.FindRef("SESSION_NAME").Data.ToString();//���� �̸�
			Info.MaxPlayers = Result.Session.SessionSettings.NumPublicConnections; //�ִ��ο�
			Info.CurrentPlayers = Info.MaxPlayers - Result.Session.NumOpenPublicConnections;//�����ο�
			Info.Index = index;
			//delegate ȣ��
			if(FServerListDel.IsBound())
				FServerListDel.Broadcast(Info);
			index++;
		}
	}
}

void UCPP_MultiplayGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	UE_LOG(LogTemp,Warning,L"OnJoinSessionComplete , sessionName %s",*SessionName.ToString());
	if(APlayerController* PC=UGameplayStatics::GetPlayerController(GetWorld(),0))
	{
		FString JoinAddress = "";
		SessionInterface->GetResolvedConnectString(SessionName,JoinAddress);
		if(JoinAddress!="")
			PC->ClientTravel(JoinAddress,ETravelType::TRAVEL_Absolute);
	}
}

void UCPP_MultiplayGameInstance::FindSpawnPoint()
{
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),ACPP_TeamPlayerSpawn::StaticClass(),SpawnPoints);
	ACPP_TeamPlayerSpawn* tempPoint;
	for(AActor* temp : SpawnPoints)
	{
		tempPoint = Cast<ACPP_TeamPlayerSpawn>(temp);

		if(tempPoint->GetTeamInfo().Equals(TEXT("Red")))
		{
			RedSpawnPoints.AddUnique(tempPoint);
		}
		else
		{
			BlueSpawnPoints.AddUnique(tempPoint);
		}
	}
}

void UCPP_MultiplayGameInstance::CreateServer()
{
	FOnlineSessionSettings SessionSettings;
    SessionSettings.bAllowJoinInProgress = false;///������
	SessionSettings.bUseLobbiesIfAvailable = true;//������
	SessionSettings.bIsDedicated = false;
	if(IOnlineSubsystem::Get()->GetSubsystemName()!="NULL")
		SessionSettings.bIsLANMatch = false;
	else
		SessionSettings.bIsLANMatch = true;
	
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bUsesPresence = true;
	SessionSettings.NumPublicConnections = 6;
	SessionSettings.Set(FName("SESSION_NAME"),
		UGameplayStatics::GetPlayerController(GetWorld(),0)->PlayerState->GetPlayerName(),
		EOnlineDataAdvertisementType::ViaOnlineService);

	SessionInterface->CreateSession(0,
		FName(UGameplayStatics::GetPlayerController(GetWorld(),0)->PlayerState->GetPlayerName()),SessionSettings);
}   

void UCPP_MultiplayGameInstance::JoinServer(FServerInfo Info)
{
	SessionInterface->JoinSession(0,FName(Info.ServerName),SearchResults[Info.Index]);
}

void UCPP_MultiplayGameInstance::FindServer()
{
	//���� ��� �ʱ�ȭ
	SearchResults.Empty();
	
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	if(IOnlineSubsystem::Get()->GetSubsystemName()!="NULL")
		SessionSearch->bIsLanQuery = false;
	else
		SessionSearch->bIsLanQuery = true;//isLan
	SessionSearch->MaxSearchResults = 10000;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE,true,EOnlineComparisonOp::Equals);
	
	SessionInterface->FindSessions(0,SessionSearch.ToSharedRef());
}

void UCPP_MultiplayGameInstance::RegisterManagerClass(TSubclassOf<UCPP_UManagerClass> managerClass)
{
	UCPP_UManagerClass* managerClassInst = NewObject<UCPP_UManagerClass>(this,managerClass,NAME_None,EObjectFlags::RF_MarkAsRootSet);
	//������ �÷��Ͱ� ȣ����� �ʵ��� ����
	managerClassInst->InitManagerClass();
	//TMap�� ����
	ManagerClasses.Add(managerClass->GetName(), managerClassInst);
}
