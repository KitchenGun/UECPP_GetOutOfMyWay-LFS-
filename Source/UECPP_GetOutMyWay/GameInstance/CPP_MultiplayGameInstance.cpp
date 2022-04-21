#include "CPP_MultiplayGameInstance.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "Common/UObject/Manager/ObjectPool/CPP_ObjectPoolManager.h"
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
		GetWorld()->ServerTravel("/Game/Level/TestLevel?listen");
	}
}

void UCPP_MultiplayGameInstance::OnFindSessionComplete(bool Succeeded)
{
	UE_LOG(LogTemp,Display,L"OnFindSessionComplete, Succeeded %d",Succeeded);
	if(Succeeded)
	{
		TArray<FOnlineSessionSearchResult> SearchResults = SessionSearch->SearchResults;
		UE_LOG(LogTemp,Display,L"FindSessionCount, count %d",SearchResults.Num());
		if(SearchResults.Num())
		{
			UE_LOG(LogTemp,Display,L"joining Server");
			SessionInterface->JoinSession(0,"My Session",SearchResults[0]);
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

void UCPP_MultiplayGameInstance::CreateServer()
{
	UE_LOG(LogTemp,Display,L"OnCreate Server");
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
	SessionSettings.NumPublicConnections = 8;

	SessionInterface->CreateSession(0,FName("My Session"),SessionSettings);
}   

void UCPP_MultiplayGameInstance::JoinServer()
{
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
	UE_LOG(LogTemp,Display,L"%s",*managerClass->GetName());
}
