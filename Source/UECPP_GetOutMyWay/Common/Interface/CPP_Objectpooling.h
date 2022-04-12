
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CPP_Objectpooling.generated.h"

UINTERFACE(MinimalAPI)
class UCPP_Objectpooling : public UInterface
{
	GENERATED_BODY()
};

class UECPP_GETOUTMYWAY_API ICPP_Objectpooling
{
	GENERATED_BODY()

public:
	//ID�� ������ ��ü ���п� �ʿ��� �Լ�
	virtual int32 GetID() const PURE_VIRTUAL(ICPP_Objectpooling::GetID, return 0;);
	virtual void SetID(int32 id) PURE_VIRTUAL(ICPP_Objectpooling::SetID, );
	//���� ���� ���� ��Ÿ���� �Լ�
	virtual bool GetCanRecycle(int32 id) const PURE_VIRTUAL(ICPP_Objectpooling::GetCanRecycle,return false;);
	virtual void SetCanRecycle(bool value) PURE_VIRTUAL(ICPP_Objectpooling::SetCanRecycle,);
	//��Ȱ��� ȣ��Ǵ� �Լ�
	virtual void OnRecycleStart() PURE_VIRTUAL(ICPP_Objectpooling::OnRecycleStart,);
	
	
};
