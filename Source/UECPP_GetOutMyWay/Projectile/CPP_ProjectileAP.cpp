#include "Projectile/CPP_ProjectileAP.h"

#include "Components/BoxComponent.h"
#include "GameInstance/CPP_MultiplayGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Tank/CPP_Tank_Character.h"

void ACPP_ProjectileAP::BounceCal(float hitAngle, EHitDir hitDir)
{
	//AP탄으로 도탄여부 계산  데미지가 1이면 도탄
	switch (ProjectileHitDir)
	{
	case EHitDir::Front:
		if(60<hitAngle&&hitAngle<120)//60도 미만의 입사각 == 도탄
		{
			Damage=20;
			HitEffectSet(EHitEffect::TankHit);
			break;
		}
		else
		{
			Damage = 1;
			HitEffectSet(EHitEffect::TankRicochet);
		}
		break;
	case EHitDir::Side:
		if(45<hitAngle&&hitAngle<135)//45도 미만의 입사각 == 도탄
		{
			Damage=20;
			HitEffectSet(EHitEffect::TankHit);
			break;
		}
		else
		{
			Damage = 1;
			HitEffectSet(EHitEffect::TankRicochet);
		}
		break;
	case EHitDir::Back:
		if(30<hitAngle&&hitAngle<150)//30도 미만의 입사각 == 도탄
		{
			Damage=20;
			HitEffectSet(EHitEffect::TankHit);
			break;
		}
		else
		{
			Damage = 1;
			HitEffectSet(EHitEffect::TankRicochet);
		}
		break;
	case EHitDir::UpSide:
		if(60<hitAngle&&hitAngle<120)//60도 미만의 입사각 == 도탄
		{
			Damage=20;
			HitEffectSet(EHitEffect::TankHit);
			break;
		}
		else
		{
			Damage = 1;
			HitEffectSet(EHitEffect::TankRicochet);
		}
		break;
	case  EHitDir::DownSide:
		if(45<hitAngle&&hitAngle<135)//45도 미만의 입사각 == 도탄
		{
			Damage=20;
			HitEffectSet(EHitEffect::TankHit);
			break;
		}
		else
		{
			Damage = 1;
			HitEffectSet(EHitEffect::TankRicochet);
		}
		break;
	default:
		Damage=20;
		break;
	}
}

void ACPP_ProjectileAP::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(Cast<ACPP_Tank_Character>(OtherActor)&&Cast<UBoxComponent>(OtherComp)&&!IsOverlap)
	{
		//포탄의 입사각 계산
		float HitAngle = GetHitAngle(OtherComp,SweepResult);
		//도탄 판정
		BounceCal(HitAngle,ProjectileHitDir);
		//데미지 주기
		if(!IsOverlap)
		{
			UGameplayStatics::ApplyPointDamage(OtherActor,Damage,SweepResult.Location,SweepResult,nullptr
			   ,this,nullptr);
			Disable();
			//중복을 방지하기 위한 overlap 확인 변수
			IsOverlap = true;
		}
		
	}
	else
	{
		HitEffectSet(EHitEffect::Ground);
	}
	//삭제
	Super::OnBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	
}
