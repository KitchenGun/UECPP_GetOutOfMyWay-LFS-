
#include "Common/UObject/Manager/ObjectPool/CPP_ObjectPoolManager.h"
#include "Projectile/CPP_Projectile.h"
#include "Particle/CPP_ParticleActor.h"

ICPP_Objectpooling* UCPP_ObjectPoolManager::GetRecycledObject(int32 objId)
{
	ICPP_Objectpooling* tempObj = nullptr;
	if(PoolObjects.Max()!=0)
	{
		for(auto obj : PoolObjects)
		{
			if(obj!=nullptr)
			{
				if(obj->GetID()==objId)
				{
					if(obj->GetCanRecycle(objId))
					{
						if(objId==0)
							tempObj = Cast<ACPP_Projectile>(obj);
						if(objId==1)
							tempObj = Cast<ACPP_ParticleActor>(obj);
						break;
					}
				}
			}
		}
	}
	return tempObj;
}

void UCPP_ObjectPoolManager::BeginPlay()
{
	Super::BeginPlay();
	//저장되어있던 객체 초기화
	PoolObjects.Empty();
}

void UCPP_ObjectPoolManager::InitManagerClass()
{
	Super::InitManagerClass();
}

void UCPP_ObjectPoolManager::ShutdownManagerClass()
{
	
	Super::ShutdownManagerClass();
}
