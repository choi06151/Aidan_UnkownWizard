#pragma once

#include "CoreMinimal.h"
#include "Bullet_Pooled.h"
#include "Components/ActorComponent.h"
#include "Spawn_Bullet.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT_API USpawn_Bullet : public UActorComponent
{
	GENERATED_BODY()

public:	
	USpawn_Bullet();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Pool에서 Bullet Spawn
	ABullet_Pooled* SpawnPooledBullet(FVector SpawnLocation, FRotator SpawnRotation);

	// 풀에 사용할 bullet subClass
	UPROPERTY(EditAnywhere, Category = "Bullet_Pooled")
	TSubclassOf<class ABullet_Pooled> PooledBulletSubclass;

	// pool의 크기
	UPROPERTY(EditAnywhere, Category = "Bullet_Pooled")
	int PoolSize = 20;

	// bullet의 수명
	UPROPERTY(EditAnywhere, Category = "Bullet_Pooled")
	float PooledBulletLifeSpan = 5.0f;

protected:
	// Bullet의 pool array
	TArray<ABullet_Pooled*> BulletPool;

	// Spawn된 Bullet index array
	TArray<int> SpawnedPoolIndexes;

	// Bullet이 비활성화일 때 호출되는 함수
	void OnPooledBulletDespawn(ABullet_Pooled* Bullet);
};
