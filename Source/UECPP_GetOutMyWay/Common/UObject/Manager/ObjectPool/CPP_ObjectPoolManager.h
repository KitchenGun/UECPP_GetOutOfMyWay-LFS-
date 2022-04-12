#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Common/Interface/CPP_Objectpooling.h"
#include "Common/UObject/Manager/CPP_UManagerClass.h"
#include "CPP_ObjectPoolManager.generated.h"

UCLASS()
class UECPP_GETOUTMYWAY_API UCPP_ObjectPoolManager : public UCPP_UManagerClass
{
	GENERATED_BODY()
private :
	// Ǯ���� ������Ʈ���� ������ �迭
	TArray<ICPP_Objectpooling*> PoolObjects;
public :
	// ���ο� ������Ʈ�� ����մϴ�. 
	template<typename T>
	T* RegisterRecyclableObject(T* newRecyclableObject)
	{
		PoolObjects.Add(static_cast<ICPP_Objectpooling*>(newRecyclableObject));
		return newRecyclableObject;
	}

	// ����� ��ü�� ����ϴ�.
	ICPP_Objectpooling* GetRecycledObject(int32 objId);

	//�ʱ�ȭ
	virtual void BeginPlay() override;
	virtual void InitManagerClass() override;
	virtual void ShutdownManagerClass() override;
};
