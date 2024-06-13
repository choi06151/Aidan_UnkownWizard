// Fill out your copyright notice in the Description page of Project Settings.


#include "SEB/LeftUI.h"

#include "Components/TextBlock.h"

void ULeftUI::NativeConstruct()
{
	Super::NativeConstruct();

	// 시간 변수 초기화
	TimeElapsed = 0.0f;

	// 타이머 설정: 1초마다 UpdateTimer 함수를 호출
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ULeftUI::UpdateTImer, 0.01f, true);

}

void ULeftUI::UpdateTImer()
{
	// 시간 증가
	TimeElapsed += 0.01f;

	// 타이머 업데이트 로직
	if (PlayTime)
	{
		PlayTime->SetText(FText::FromString(GetFormattedTime()));
	}
}

FString ULeftUI::GetFormattedTime() const
{
	int32 TotalMilliseconds = static_cast<int32>(TimeElapsed * 1000.0f);
	int32 Minutes = TotalMilliseconds / 60000;
	TotalMilliseconds %= 60000;
	int32 Seconds = TotalMilliseconds / 1000;
	int32 Centiseconds = (TotalMilliseconds % 1000) / 10;  

	return FString::Printf(TEXT("%02d:%02d:%02d"), Minutes, Seconds, Centiseconds);
}
