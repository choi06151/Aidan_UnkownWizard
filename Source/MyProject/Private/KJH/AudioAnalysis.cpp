// Fill out your copyright notice in the Description page of Project Settings.


#include "KJH/AudioAnalysis.h"
#include "AudioCaptureComponent.h"
#include "Sound/SoundWaveProcedural.h"
#include "AudioSynesthesiaNRT.h"
#include "Kismet/GameplayStatics.h"
#include "Audio.h"
#include "AudioAnalytics.h"
#include "Components/SynthComponent.h"
#include "AudioCapture.h"
#include "OnsetNRT.h"
#include "LoudnessNRT.h"
#include "ConstantQNRT.h"

// Sets default values for this component's properties
UAudioAnalysis::UAudioAnalysis()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// 오디오 캡처 컴포넌트 생성
	AudioCaptureComponent = CreateDefaultSubobject<UAudioCaptureComponent>(TEXT("AudioCaptureComponent"));
}


// Called when the game starts
void UAudioAnalysis::BeginPlay()
{
	Super::BeginPlay();
	
}


// Called every frame
void UAudioAnalysis::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 매 프레임마다 오디오 데이터를 처리
	//ProcessAudioData();
}

//void UAudioAnalysis::InitializeAudioCapture()
//{
//    if (AudioCaptureComponent)
//    {
//        // 오디오 분석 컴포넌트들 초기화
//        OnsetNRT = NewObject<UOnsetNRT>(this);
//        OnsetNRT->Settings = NewObject<UOnsetNRTSettings>(OnsetNRT);
//        OnsetNRT->Settings->AnalysisDuration = 10.0f;
//        OnsetNRT->Settings->AnalysisPeriod = 0.01f;
//        OnsetNRT->Settings->Sensitivity = 0.5f;
//
//        LoudnessNRT = NewObject<ULoudnessNRT>(this);
//        LoudnessNRT->Settings = NewObject<ULoudnessNRTSettings>(LoudnessNRT);
//        LoudnessNRT->Settings->DurationInSeconds = 10.0f;
//        LoudnessNRT->Settings->AnalysisPeriod = 0.01f;
//
//        ConstantQNRT = NewObject<UConstantQNRT>(this);
//        ConstantQNRT->Settings = NewObject<UConstantQNRTSettings>(ConstantQNRT);
//        ConstantQNRT->Settings->DurationInSeconds = 10.0f;
//        ConstantQNRT->Settings->AnalysisPeriod = 0.01f;
//    }
//}
//
//void UAudioAnalysis::StartAudioCapture()
//{
//    if (AudioCaptureComponent)
//    {
//        // 오디오 캡처 시작
//        AudioCaptureComponent->OnBeginGenerate();
//    }
//}
//
//void UAudioAnalysis::StopAudioCapture()
//{
//    if (AudioCaptureComponent)
//    {
//        // 오디오 캡처 종료
//        AudioCaptureComponent->OnEndGenerate();
//    }
//}
//
//void UAudioAnalysis::ProcessAudioData()
//{
//    if (AudioCaptureComponent && AudioCaptureComponent->IsCapturing())
//    {
//        // 캡처된 오디오 데이터를 가져옴
//        TArray<uint8> AudioData;
//        AudioCaptureComponent->GetCaptureAudioData(AudioData);
//        if (AudioData.Num() > 0)
//        {
//            // 오디오 데이터를 분석
//            OnsetNRT->AnalyzeAudio(AudioData, 44100, 1024);
//            LoudnessNRT->AnalyzeAudio(AudioData, 44100, 1024);
//            ConstantQNRT->AnalyzeAudio(AudioData, 44100, 1024);
//
//            // 온셋(음의 시작) 데이터 처리
//            TArray<float> OnsetTimes;
//            OnsetNRT->GetNormalizedChannelOnsets(0, OnsetTimes);
//
//            // 볼륨 데이터 처리
//            TArray<float> LoudnessValues;
//            LoudnessNRT->GetNormalizedChannelLoudness(0, LoudnessValues);
//            CurrentVolume = LoudnessValues.Last();
//
//            // 주파수 데이터 처리
//            TArray<float> Magnitudes;
//            ConstantQNRT->GetNormalizedChannelConstantQ(0, 0.0f, Magnitudes);
//            FrequencyBands = Magnitudes;
//        }
//    }
//}

