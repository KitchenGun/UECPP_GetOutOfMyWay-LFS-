#pragma once

#include "CoreMinimal.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "Common/UObject/Manager/CPP_UManagerClass.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "CPP_MultiplayGameInstance.generated.h"

USTRUCT(BlueprintType)
struct FServerInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
	FString ServerName;
	UPROPERTY(BlueprintReadOnly)
	int32 CurrentPlayers;
	UPROPERTY(BlueprintReadOnly)
	int32 MaxPlayers;
	UPROPERTY(BlueprintReadOnly)
	int32 Index;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FServerDel,FServerInfo,FServerInfoDel);

UCLASS()
class UECPP_GETOUTMYWAY_API UCPP_MultiplayGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UCPP_MultiplayGameInstance();

	void BeginPlay();
	//server
	UFUNCTION(BlueprintCallable)
	void CreateServer();
	UFUNCTION(BlueprintCallable)
	void JoinServer(FServerInfo Info);
	UFUNCTION(BlueprintCallable)
	void FindServer();
	//델리게이트
	UPROPERTY(BlueprintAssignable)
	FServerDel FServerListDel;
	//Get&Set
	template<typename ManagerClassType>
	FORCEINLINE ManagerClassType* GetManagerClass()
	{return Cast<ManagerClassType>(ManagerClasses[ManagerClassType::StaticClass()->GetName()]);}
protected:
	virtual void Init() override;
	virtual void Shutdown() override;
	//server
	virtual void OnCreateSessionComplete(FName ServerName, bool Succeeded);
	virtual void OnFindSessionComplete(bool Succeeded);
	virtual void OnJoinSessionComplete(FName SessionName,EOnJoinSessionCompleteResult::Type Result);
	//Manager 관리
	void RegisterManagerClass(TSubclassOf<UCPP_UManagerClass> managerClass);

	TMap<FString,UCPP_UManagerClass*> ManagerClasses;
	//server
	IOnlineSessionPtr SessionInterface;
	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	TArray<FOnlineSessionSearchResult> SearchResults;
	
};
