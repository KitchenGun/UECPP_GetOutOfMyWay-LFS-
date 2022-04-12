#include "Projectile/CPP_ProjectileAP.h"

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Tank/CPP_Tank_Pawn.h"

void ACPP_ProjectileAP::BounceCal(float hitAngle, EHitDir hitDir)
{//APź���� ��ź���� ���  �������� 1�̸� ��ź 
	switch (ProjectileHitDir)
	{
	case EHitDir::Front:
		if(60<hitAngle&&hitAngle<120)//60�� �̸��� �Ի簢 == ��ź
			break;
		else
			Damage = 1;
		break;
	case EHitDir::Side:
		if(30<hitAngle&&hitAngle<150)//30�� �̸��� �Ի簢 == ��ź
			break;
		else
			Damage = 1;
		break;
	case EHitDir::Back:
		if(10<hitAngle&&hitAngle<170)//10�� �̸��� �Ի簢 == ��ź
			break;
		else
			Damage = 1;
		break;
	case EHitDir::UpSide:
		if(60<hitAngle&&hitAngle<120)//60�� �̸��� �Ի簢 == ��ź
			break;
		else
			Damage = 1;
		break;
	case  EHitDir::DownSide:
		if(45<hitAngle&&hitAngle<135)//45�� �̸��� �Ի簢 == ��ź
			break;
		else
			Damage = 1;
		break;
	default:
		break;
	}
}


void ACPP_ProjectileAP::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if(Cast<ACPP_Tank_Pawn>(OtherActor)&&Cast<UBoxComponent>(OtherComp))
	{
		//��ź�� �Ի簢 ���
		float HitAngle = GetHitAngle(HitComponent,OtherComp,Hit);
	
		const UEnum* DirEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EHitDir"), true);
		FString EnumToString = DirEnum->GetNameStringByValue((int64)ProjectileHitDir);
	
		//��ź ����
		BounceCal(HitAngle,ProjectileHitDir);
		//������ �ֱ�
		UGameplayStatics::ApplyPointDamage(OtherActor,Damage,Hit.Location,Hit,PlayerCtrl,this,nullptr);
		Disable();
	}
	//����	
	Super::OnHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);
		
}
