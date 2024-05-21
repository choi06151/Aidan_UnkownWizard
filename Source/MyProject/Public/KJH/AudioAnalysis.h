// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AudioAnalysis.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT_API UAudioAnalysis : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAudioAnalysis();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // 오디오 캡처를 초기화하는 함수
    void InitializeAudioCapture();

    // 오디오 캡처를 시작하는 함수
    void StartAudioCapture();

    // 오디오 캡처를 중지하는 함수
    void StopAudioCapture();

private:
    // 오디오 캡처 컴포넌트
    UPROPERTY()
    class UAudioCaptureComponent* AudioCaptureComponent;

    // 오디오 분석 컴포넌트들
    UPROPERTY()
    class UOnsetNRT* OnsetNRT;

    UPROPERTY()
    class ULoudnessNRT* LoudnessNRT;

    UPROPERTY()
    class UConstantQNRT* ConstantQNRT;

    // 오디오 데이터를 처리하는 함수
    void ProcessAudioData();

public:
    // 현재 볼륨을 저장하는 변수
    float CurrentVolume;

    // 주파수 대역 데이터를 저장하는 배열
    TArray<float> FrequencyBands;
};


