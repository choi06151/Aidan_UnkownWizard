#include "JWK/Spawn_Bullet.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"

USpawn_Bullet::USpawn_Bullet()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void USpawn_Bullet::BeginPlay()
{
    Super::BeginPlay();

    // bullet의 subClass null 체크
    if (PooledBulletSubclass != nullptr)
    {
        UWorld* const World = GetWorld();

        if (World != nullptr)
        {
            for (int i = 0; i < PoolSize; i++)
            {
                // Bullet 객체 생성 및 초기화
                ABullet_Pooled* PoolableBullet = World->SpawnActor<ABullet_Pooled>
                (
                    PooledBulletSubclass,
                    FVector::ZeroVector,
                    FRotator::ZeroRotator
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
}

void USpawn_Bullet::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

// pool에서 Bullet을 Spawn하는 함수
ABullet_Pooled* USpawn_Bullet::SpawnPooledBullet(FVector SpawnLocation, FRotator SpawnRotation, float BulletSpeed, int FloatIntensity)
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
            PoolableBullet->SetFloatIntensity(FloatIntensity); // 둥실거림 강도 설정 0619

            return PoolableBullet;
        }
    }

    // 비활성화된 Bullet이 없는 경우, 새로운 Bullet을 생성하여 풀에 추가
    UWorld* const World = GetWorld();
    if (World != nullptr)
    {
        int32 NewIndex = BulletPool.Num();
        ABullet_Pooled* NewBullet = World->SpawnActor<ABullet_Pooled>(
            PooledBulletSubclass,
            SpawnLocation,
            SpawnRotation
        );

        if (NewBullet != nullptr)
        {
            NewBullet->SetActive(true);
            NewBullet->SetPoolIndex(NewIndex);
            NewBullet->SetLifeSpan(PooledBulletLifeSpan);
            NewBullet->SetBulletSpeed(BulletSpeed);
            NewBullet->OnPooledBulletDespawn.AddDynamic(this, &USpawn_Bullet::OnPooledBulletDespawn);
            NewBullet->SetFloatIntensity(FloatIntensity); // 둥실거림 강도 설정 0619
            BulletPool.Add(NewBullet);
            SpawnedPoolIndexes.Add(NewIndex);

            return NewBullet;
        }
    }

    return nullptr; // 사용 가능한 Bullet이 없는 경우 null 반환
}

// Bullet이 비활성화될 때 호출되는 함수
void USpawn_Bullet::OnPooledBulletDespawn(ABullet_Pooled* Bullet)
{
    if (Bullet != nullptr)
    {
        SpawnedPoolIndexes.Remove(Bullet->GetPoolIndex());  // 비활성화된 Bullet의 인덱스 제거
        Bullet->SetActive(false);
    }
}

void USpawn_Bullet::DeactivateAllBullets()
{
    for(ABullet_Pooled* Bullet : BulletPool)
    {
        if(nullptr != Bullet && Bullet->IsActive())
            Bullet->Deactivate();
    }
}

const TArray<ABullet_Pooled*>& USpawn_Bullet::GetBulletPool() const
{
    return BulletPool;
}
