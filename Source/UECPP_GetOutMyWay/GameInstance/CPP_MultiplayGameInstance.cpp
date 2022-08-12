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
{//플레이 시작할때 
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
	
	//objectpoolmanager 저장
	RegisterManagerClass(UCPP_ObjectPoolManager::StaticClass());
	
	//beginplay실행용
	for(auto & IT : ManagerClasses)
	{
		IT.Value->BeginPlay();
	}
}

void UCPP_MultiplayGameInstance::Shutdown()
{
	//플레이 멈출때
	for (auto pair : ManagerClasses)
	{
		if(!IsValid(pair.Value))
			return;
		//IsValid 누군가 가리키는 오브젝트 인지?
		//IsValidLowlevel,Fast 현재 레벨에 남아있는지?
		if (!pair.Value->IsValidLowLevel()) return;
		
		pair.Value->ShutdownManagerClass();

		// 객체 소멸
		pair.Value->ConditionalBeginDestroy();//가비지 컬랙터가 실행될때 즉 게임을 종료할때 실행
		//GetWorld()->ForceGarbageCollection(); //가비지 컬랙터가 즉시 작동하도록 하는 함수
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
			Info.ServerName = Result.Session.SessionSettings.Settings.FindRef("SESSION_NAME").Data.ToString();//서버 이름
			Info.MaxPlayers = Result.Session.SessionSettings.NumPublicConnections; //최대인원
			Info.CurrentPlayers = Info.MaxPlayers - Result.Session.NumOpenPublicConnections;//현재인원
			Info.Index = index;
			//delegate 호출
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
    SessionSettings.bAllowJoinInProgress = false;///수정함
	SessionSettings.bUseLobbiesIfAvailable = true;//수정함
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
	//서버 결과 초기화
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
	//가비지 컬랙터가 호출되지 않도록 제작
	managerClassInst->InitManagerClass();
	//TMap에 저장
	ManagerClasses.Add(managerClass->GetName(), managerClassInst);
}
