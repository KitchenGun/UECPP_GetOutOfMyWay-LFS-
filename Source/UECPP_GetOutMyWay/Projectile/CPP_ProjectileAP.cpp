#include "Projectile/CPP_ProjectileAP.h"

#include "Common/UObject/Manager/ObjectPool/CPP_ObjectPoolManager.h"
#include "Components/BoxComponent.h"
#include "GameInstance/CPP_MultiplayGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Tank/CPP_Tank_Pawn.h"

void ACPP_ProjectileAP::BounceCal(float hitAngle, EHitDir hitDir)
{
	//debug
	const UEnum* DirEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EHitDir"), true);
	FString EnumToString = DirEnum->GetNameStringByValue((int64)ProjectileHitDir);
	UE_LOG(LogTemp,Display,L"%s",*EnumToString);
	UE_LOG(LogTemp,Display,L"%s",*HitPos.ToString());
	UE_LOG(LogTemp,Display,L"%.2f",hitAngle);
	//APź���� ��ź���� ���  �������� 1�̸� ��ź
	switch (ProjectileHitDir)
	{
	case EHitDir::Front:
		if(60<hitAngle&&hitAngle<120)//60�� �̸��� �Ի簢 == ��ź
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
		if(45<hitAngle&&hitAngle<135)//45�� �̸��� �Ի簢 == ��ź
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
		if(30<hitAngle&&hitAngle<150)//30�� �̸��� �Ի簢 == ��ź
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
		if(60<hitAngle&&hitAngle<120)//60�� �̸��� �Ի簢 == ��ź
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
		if(45<hitAngle&&hitAngle<135)//45�� �̸��� �Ի簢 == ��ź
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
	if(Cast<ACPP_Tank_Pawn>(OtherActor)&&Cast<UBoxComponent>(OtherComp)&&!IsOverlap)
	{
		//�ߺ��� �����ϱ� ���� overlap Ȯ�� ����
		IsOverlap = true;
		//��ź�� �Ի簢 ���
		float HitAngle = GetHitAngle(OtherComp,SweepResult);
		//��ź ����
		BounceCal(HitAngle,ProjectileHitDir);
		//������ �ֱ�
		UGameplayStatics::ApplyPointDamage(OtherActor,Damage,SweepResult.Location,SweepResult,PlayerCtrl,this,nullptr);
	}
	else
	{
		HitEffectSet(EHitEffect::Ground);
	}
	//����	
	Super::OnBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}
