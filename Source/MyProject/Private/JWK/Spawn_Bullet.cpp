#include "JWK/Spawn_Bullet.h"

USpawn_Bullet::USpawn_Bullet()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USpawn_Bullet::BeginPlay()
{
	Super::BeginPlay();

	if(PooledBulletSubclass != nullptr)
	{
		UWorld* const World = GetWorld();

		if (World != nullptr)
			for (int i = 0; i < PoolSize; i++)
			{
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

ABullet_Pooled* USpawn_Bullet::SpawnPooledBullet()
{
	for (ABullet_Pooled* PoolableBullet : BulletPool)
	{
		if (PoolableBullet != nullptr && !PoolableBullet->IsActive())
		{
			PoolableBullet->TeleportTo(FVector(0, 0, 0), FRotator(0, 0, 0));
			PoolableBullet->SetLifeSpan(PooledBulletLifeSpan);
			PoolableBullet->SetActive(true);
			SpawnedPoolIndexes.Add(PoolableBullet->GetPoolIndex());

			return PoolableBullet;
		}
	}

	if(SpawnedPoolIndexes.Num() > 0)
	{
		int PooledBulletIndex = SpawnedPoolIndexes[0];
		SpawnedPoolIndexes.Remove(PooledBulletIndex);
		ABullet_Pooled* PoolableBullet = BulletPool[PooledBulletIndex];
		
		if (PoolableBullet != nullptr)
		{
			PoolableBullet->SetActive(false);

			PoolableBullet->TeleportTo(FVector(0, 0, 0), FRotator(0, 0, 0));
			PoolableBullet->SetLifeSpan(PooledBulletLifeSpan);
			PoolableBullet->SetActive(true);
			SpawnedPoolIndexes.Add(PoolableBullet->GetPoolIndex());

			return PoolableBullet;
		}
	}

	return  nullptr;
}

void USpawn_Bullet::OnPooledBulletDespawn(ABullet_Pooled* Bullet)
{
	SpawnedPoolIndexes.Remove(Bullet->GetPoolIndex());
}

