#pragma once

#include "CoreMinimal.h"
#include "Baton_Pooled.h"
#include "Components/ActorComponent.h"
#include "Spawn_Baton.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT_API USpawn_Baton : public UActorComponent
{
	GENERATED_BODY()

public:	
	USpawn_Baton();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Pool에서 Baton Spawn
	ABaton_Pooled* SpawnPooledBaton(FVector SpawnLocation, FRotator SpawnRotation, float BatonSpeed);

	// 풀에 사용할 Baton subClass
	UPROPERTY(EditAnywhere, Category = "Baton_Pooled")
	TSubclassOf<class ABaton_Pooled> PooledBatonSubclass;

	// pool의 크기
	UPROPERTY(EditAnywhere, Category = "Baton_Pooled")
	int PoolSize = 5;

	// Baton의 수명
	UPROPERTY(EditAnywhere, Category = "Baton_Pooled")
	float PooledBatonLifeSpan = 15.0f;

	class ABoss* boss;

protected:
	// Baton의 pool array
	TArray<ABaton_Pooled*> BatonPool;

	// Spawn된 Baton index array
	TArray<int> SpawnedPoolBatonIndexes;

	// Baton이 비활성화일 때 호출되는 함수
	void OnPooledBatonDespawn(ABaton_Pooled* Baton);
};
