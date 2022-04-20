#include "CPP_MultiplayGameInstance.h"
#include "GameInstance/CPP_MultiplayGameInstance.h"
#include "OnlineSubsystem.h"
#include "Common/UObject/Manager/ObjectPool/CPP_ObjectPoolManager.h"

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
	
}

void UCPP_MultiplayGameInstance::CreateServer()
{
	//FOnlineSessionSettings SessionSettings;

	SessionInterface->CreateSession(0,FName("My Session"),)
}

void UCPP_MultiplayGameInstance::JoinServer()
{
	
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
