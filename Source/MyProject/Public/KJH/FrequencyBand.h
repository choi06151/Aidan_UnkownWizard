#pragma once

#include "CoreMinimal.h"
#include "FrequencyBand.generated.h"

USTRUCT(BlueprintType)
struct FFrequencyBand
{
    GENERATED_BODY()

    // 주파수 대역의 값을 저장하는 배열
    UPROPERTY(BlueprintReadWrite, Category = "Frequency Band")
    TArray<float> Values;
};