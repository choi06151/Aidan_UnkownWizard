#include "JWK/Spawn_Bullet.h"

USpawn_Bullet::USpawn_Bullet()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USpawn_Bullet::BeginPlay()
{
	Super::BeginPlay();

	// bullet의 subClass null 체크
	if(PooledBulletSubclass != nullptr)
	{
		UWorld* const World = GetWorld();

		if (World != nullptr)
			for (int i = 0; i < PoolSize; i++)
			{
				// Bullet 객체 생성 및 초기화
				ABullet_Pooled* PoolableBullet = World->SpawnActor<ABullet_Pooled>
				(
					PooledBulletSubclass,
					FVector().ZeroVector,
					FRotator().ZeroRotator
				);

				if (PoolableBullet != nullptr)
				{
					PoolableBullet->SetActive(false);
					PoolableBullet->SetPoolIndex(i);
					PoolableBullet->OnPooledBulletDespawn.AddDynamic(this, &USpawn_Bullet::OnPooledBulletDespawn);
					BulletPool.Add(PoolableBullet);
				}
			}
	}
}


void USpawn_Bullet::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

// pool에서 Bullet을 Spawn하는 함수
ABullet_Pooled* USpawn_Bullet::SpawnPooledBullet(FVector SpawnLocation, FRotator SpawnRotation, float BulletSpeed)
{
	for (ABullet_Pooled* PoolableBullet : BulletPool)
	{
		if (PoolableBullet != nullptr && !PoolableBullet->IsActive())
		{
			// 비활성화된 Bullet을 찾아 활성화
			PoolableBullet->TeleportTo(SpawnLocation, SpawnRotation);
			PoolableBullet->SetLifeSpan(PooledBulletLifeSpan);
			PoolableBullet->SetBulletSpeed(BulletSpeed); // 이동 속도 설정
			PoolableBullet->SetActive(true);
			SpawnedPoolIndexes.Add(PoolableBullet->GetPoolIndex());

			return PoolableBullet;
		}
	}

	if(SpawnedPoolIndexes.Num() > 0)
	{
		// 모든 Bullet이 활성화된 경우, 가장 먼저 활성화된 Bullet을 재사용
		int PooledBulletIndex = SpawnedPoolIndexes[0];
		SpawnedPoolIndexes.Remove(PooledBulletIndex);
		ABullet_Pooled* PoolableBullet = BulletPool[PooledBulletIndex];
		
		if (PoolableBullet != nullptr)
		{
			PoolableBullet->SetActive(false);
			PoolableBullet->TeleportTo(SpawnLocation, SpawnRotation);
			PoolableBullet->SetLifeSpan(PooledBulletLifeSpan);
			PoolableBullet->SetBulletSpeed(BulletSpeed); // 이동 속도 설정
			PoolableBullet->SetActive(true);
			SpawnedPoolIndexes.Add(PoolableBullet->GetPoolIndex());

			return PoolableBullet;
		}
	}
	return  nullptr; // 사용 가능한 Bullet이 없는 경우 nullptr 반환
}

// Bullet이 비활성화될 때 호출되는 함수
void USpawn_Bullet::OnPooledBulletDespawn(ABullet_Pooled* Bullet)
{
	SpawnedPoolIndexes.Remove(Bullet->GetPoolIndex());  // 비활성화된 Bullet의 인덱스 제거
}

