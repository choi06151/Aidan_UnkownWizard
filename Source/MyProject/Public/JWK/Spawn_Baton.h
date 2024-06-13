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

	// Pool���� Baton Spawn
	ABaton_Pooled* SpawnPooledBaton(FVector SpawnLocation, FRotator SpawnRotation, float BatonSpeed);

	// Ǯ�� ����� Baton subClass
	UPROPERTY(EditAnywhere, Category = "Baton_Pooled")
	TSubclassOf<class ABaton_Pooled> PooledBatonSubclass;

	// pool�� ũ��
	UPROPERTY(EditAnywhere, Category = "Baton_Pooled")
	int PoolSize = 5;

	// Baton�� ����
	UPROPERTY(EditAnywhere, Category = "Baton_Pooled")
	float PooledBatonLifeSpan = 15.0f;

	class ABoss* boss;

protected:
	// Baton�� pool array
	TArray<ABaton_Pooled*> BatonPool;

	// Spawn�� Baton index array
	TArray<int> SpawnedPoolBatonIndexes;

	// Baton�� ��Ȱ��ȭ�� �� ȣ��Ǵ� �Լ�
	void OnPooledBatonDespawn(ABaton_Pooled* Baton);
};
