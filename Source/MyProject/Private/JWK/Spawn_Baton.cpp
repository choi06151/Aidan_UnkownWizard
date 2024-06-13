#include "JWK/Spawn_Baton.h"

USpawn_Baton::USpawn_Baton()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USpawn_Baton::BeginPlay()
{
	Super::BeginPlay();

	// Baton�� subClass null üũ
	if (PooledBatonSubclass != nullptr)
	{
		UWorld* const World = GetWorld();

		if (World != nullptr)
			for (int i = 0; i < PoolSize; i++)
			{
				// Baton ��ü ���� �� �ʱ�ȭ
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

// pool���� Baton�� Spawn�ϴ� �Լ�
ABaton_Pooled* USpawn_Baton::SpawnPooledBaton(FVector SpawnLocation, FRotator SpawnRotation, float BatonSpeed)
{
	for (ABaton_Pooled* PoolableBaton : BatonPool)
	{
		if (PoolableBaton != nullptr && !PoolableBaton->IsActive())
		{
			// ��Ȱ��ȭ�� Baton�� ã�� Ȱ��ȭ
			PoolableBaton->TeleportTo(SpawnLocation, SpawnRotation);
			PoolableBaton->SetLifeSpan(PooledBatonLifeSpan);
			PoolableBaton->SetBatonSpeed(BatonSpeed); // �̵� �ӵ� ����
			PoolableBaton->SetActive(true);
			SpawnedPoolBatonIndexes.Add(PoolableBaton->GetPoolIndex());

			return PoolableBaton;
		}
	}

	if (SpawnedPoolBatonIndexes.Num() > 0)
	{
		// ��� Bullet�� Ȱ��ȭ�� ���, ���� ���� Ȱ��ȭ�� Bullet�� ����
		int PooledBatonIndex = SpawnedPoolBatonIndexes[0];
		SpawnedPoolBatonIndexes.Remove(PooledBatonIndex);
		ABaton_Pooled* PoolableBaton = BatonPool[PooledBatonIndex];

		if (PoolableBaton != nullptr)
		{
			PoolableBaton->meshComp->SetVisibility(true); //�Ѿ��� Visibility ������ ����
			PoolableBaton->SetActive(false);
			PoolableBaton->TeleportTo(SpawnLocation, SpawnRotation);
			PoolableBaton->SetLifeSpan(PooledBatonLifeSpan);
			PoolableBaton->SetBatonSpeed(BatonSpeed); // �̵� �ӵ� ����
			PoolableBaton->SetActive(true);
			SpawnedPoolBatonIndexes.Add(PoolableBaton->GetPoolIndex());

			return PoolableBaton;
		}
	}

	return nullptr; // ��� ������ Baton ���� ��� null ��ȯ
}

// Baton�� ��Ȱ��ȭ�� �� ȣ��Ǵ� �Լ�
void USpawn_Baton::OnPooledBatonDespawn(ABaton_Pooled* Baton)
{
	SpawnedPoolBatonIndexes.Remove(Baton->GetPoolIndex()); // ��Ȱ��ȭ�� Baton�� �ε��� ����
}
