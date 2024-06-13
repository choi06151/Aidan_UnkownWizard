#include "JWK/Spawn_Baton.h"

USpawn_Baton::USpawn_Baton()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USpawn_Baton::BeginPlay()
{
	Super::BeginPlay();

	// Baton의 subClass null 체크
	if (PooledBatonSubclass != nullptr)
	{
		UWorld* const World = GetWorld();

		if (World != nullptr)
			for (int i = 0; i < PoolSize; i++)
			{
				// Baton 객체 생성 및 초기화
				ABaton_Pooled* PoolableBaton = World->SpawnActor<ABaton_Pooled>
				(
					PooledBatonSubclass,
					FVector().ZeroVector,
					FRotator().ZeroRotator
				);

				if (PoolableBaton != nullptr)
				{
					PoolableBaton->SetActive(false);
					PoolableBaton->SetPoolIndex(i);
					PoolableBaton->OnPooledBatonDespawn.AddDynamic(this, &USpawn_Baton::OnPooledBatonDespawn);
					BatonPool.Add(PoolableBaton);
				}
			}
	}
}

void USpawn_Baton::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

// pool에서 Baton을 Spawn하는 함수
ABaton_Pooled* USpawn_Baton::SpawnPooledBaton(FVector SpawnLocation, FRotator SpawnRotation, float BatonSpeed)
{
	for (ABaton_Pooled* PoolableBaton : BatonPool)
	{
		if (PoolableBaton != nullptr && !PoolableBaton->IsActive())
		{
			// 비활성화된 Baton을 찾아 활성화
			PoolableBaton->TeleportTo(SpawnLocation, SpawnRotation);
			PoolableBaton->SetLifeSpan(PooledBatonLifeSpan);
			PoolableBaton->SetBatonSpeed(BatonSpeed); // 이동 속도 설정
			PoolableBaton->SetActive(true);
			SpawnedPoolBatonIndexes.Add(PoolableBaton->GetPoolIndex());

			return PoolableBaton;
		}
	}

	if (SpawnedPoolBatonIndexes.Num() > 0)
	{
		// 모든 Bullet이 활성화된 경우, 가장 먼저 활성화된 Bullet을 재사용
		int PooledBatonIndex = SpawnedPoolBatonIndexes[0];
		SpawnedPoolBatonIndexes.Remove(PooledBatonIndex);
		ABaton_Pooled* PoolableBaton = BatonPool[PooledBatonIndex];

		if (PoolableBaton != nullptr)
		{
			PoolableBaton->meshComp->SetVisibility(true); //총알의 Visibility 설정을 리셋
			PoolableBaton->SetActive(false);
			PoolableBaton->TeleportTo(SpawnLocation, SpawnRotation);
			PoolableBaton->SetLifeSpan(PooledBatonLifeSpan);
			PoolableBaton->SetBatonSpeed(BatonSpeed); // 이동 속도 설정
			PoolableBaton->SetActive(true);
			SpawnedPoolBatonIndexes.Add(PoolableBaton->GetPoolIndex());

			return PoolableBaton;
		}
	}

	return nullptr; // 사용 가능한 Baton 없는 경우 null 반환
}

// Baton이 비활성화될 때 호출되는 함수
void USpawn_Baton::OnPooledBatonDespawn(ABaton_Pooled* Baton)
{
	SpawnedPoolBatonIndexes.Remove(Baton->GetPoolIndex()); // 비활성화된 Baton의 인덱스 제거
}
