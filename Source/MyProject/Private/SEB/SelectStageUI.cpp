// Fill out your copyright notice in the Description page of Project Settings.


#include "SEB/SelectStageUI.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/UniformGridPanel.h"
#include "Kismet/GameplayStatics.h"
#include "SEB/GameStartUI.h"
#include "SEB/MusicInfoDT.h"
#include "SEB/StageUI.h"
#include "Components/WidgetComponent.h"

void USelectStageUI::NativePreConstruct()
{
	Super::NativePreConstruct();

	// DataTable 초기화
	static const FString ContextString(TEXT("Music Info Context"));
	static const FSoftObjectPath DataTablePath(TEXT("/Game/SEB/Blueprints/DT_MusicInfo.DT_MusicInfo"));
	UDataTable* MusicInfoDataObject = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *DataTablePath.ToString()));

	if(MusicInfoDataObject)
	{
		UE_LOG(LogTemp, Error, TEXT("DataTable Succeed!!!"));
		MusicDataTable = MusicInfoDataObject;
	}
	if (!StageUIClass || !MainScroll) return;

	MainScroll->ClearChildren();

	TArray<FName> RowNames = MusicDataTable->GetRowNames();
	int32 RowCount = RowNames.Num();

	for(int i=0; i<RowCount; i++)
	{
		UStageUI* StageUI = CreateWidget<UStageUI>(this, StageUIClass);
		FMusicInfoDT* Row = MusicDataTable->FindRow<FMusicInfoDT>(RowNames[i], TEXT(""));
		
		if (StageUI)
		{
			StageUI->SetArtistText(FText::FromString(Row->ArtistName));
			StageUI->SetMusicText(FText::FromString(Row->MusicName));
			StageUI->SetDifficultyImage(Row->Difficulty);
			StageUI->ParentSelectStageUI = this;  
			MainScroll->AddChild(StageUI);
		}
	}
	//배열의 첫번째 값들로 정보 초기화
	ChangeStageName(FText::FromString(MusicDataTable->FindRow<FMusicInfoDT>(RowNames[0], TEXT(""))->ArtistName),FText::FromString(MusicDataTable->FindRow<FMusicInfoDT>(RowNames[0], TEXT(""))->MusicName) );
}

void USelectStageUI::NativeConstruct()
{
	Super::NativeConstruct();
	UpArrowBtn->OnClicked.AddDynamic(this, &USelectStageUI::OnUPArrowClicked);
	DownArrowBtn->OnClicked.AddDynamic(this, &USelectStageUI::OnDownArrowClicked);
	BackBtn->OnClicked.AddDynamic(this, &USelectStageUI::OnBackClicked);
	PlayBtn->OnClicked.AddDynamic(this, &USelectStageUI::OnPlayClicked);
	
}


void USelectStageUI::OnUPArrowClicked()
{
	if (MainScroll)
	{
		float CurrentOffset = MainScroll->GetScrollOffset();
		MainScroll->SetScrollOffset(CurrentOffset - 300.0f);
	}
}

void USelectStageUI::OnDownArrowClicked()
{
	if (MainScroll)
	{
		float CurrentOffset = MainScroll->GetScrollOffset();
		MainScroll->SetScrollOffset(CurrentOffset + 300.0f);
	}
}

void USelectStageUI::OnBackClicked()
{
	//SpawnWidget의 WidgetClass를 GameStartUI로 바꾸고싶어
	// WidgetClass를 변경할 클래스
	//TSubclassOf<UUserWidget> NewWidgetClass = UGameStartUI::StaticClass();

	// SpawnWidget의 WidgetClass를 변경
	ASpawnWidget* SpawnWidget = Cast<ASpawnWidget>(UGameplayStatics::GetActorOfClass(GetWorld(), ASpawnWidget::StaticClass()));
	
	UWidgetComponent* WidgetComponent = SpawnWidget->FindComponentByClass<UWidgetComponent>();
	WidgetComponent->SetWidgetClass(NewWidgetClass);
}

void USelectStageUI::OnPlayClicked()
{
	// UI 숨김
	SetVisibility(ESlateVisibility::Hidden);
	// 게임 시작 -> 레벨이동?
}


//버튼 클릭 시 좌측 정보 업데이트
void USelectStageUI::ChangeStageName(const FText& NewText, const FText& NewInfoText)
{
	//NewText라는 ArtistName열을 가진 행 탐색
	FMusicInfoDT* SpecificRow = FindRowByColumnValue("ArtistName", NewText.ToString());

	ArtistName->SetText(NewText);
	MusicName->SetText(NewInfoText);
	
	//BestScore 변경
	BestScore->SetText(FText::AsNumber(SpecificRow->BestScore));

	// 썸네일 변경
	ThumbnailPath = new FText(FText::FromString(SpecificRow->Thumbnail));
	UTexture2D* ThumbnailTexture = LoadObject<UTexture2D>(nullptr, *ThumbnailPath->ToString());
	if (ThumbnailTexture)
	{
		FSlateBrush NewBrush;
		NewBrush.SetResourceObject(ThumbnailTexture);
		Thumbnail->SetBrush(NewBrush);
	}

	//난이도 별 이미지 변경
	int32 num = SpecificRow->Difficulty;
	if(DifficultyGridPanel)
	{
		for(int i=0; i<DifficultyGridPanel->GetChildrenCount(); i++)
		{
			UWidget* ChildWidget = DifficultyGridPanel->GetChildAt(i);
			UImage* ImageWidget = Cast<UImage>(ChildWidget);
			
			if(i>=num) {
				if(ImageWidget)
				{
					FText* StarPath = new FText(FText::FromString(TEXT("/Script/Engine.Texture2D'/Game/SEB/UI/Resources/Star.Star'")));
					SetStarFill(ImageWidget, StarPath);
				}
			}
			else
			{
				if(ImageWidget)
				{
					FText* StarFillPath = new FText(FText::FromString(TEXT("/Script/Engine.Texture2D'/Game/SEB/UI/Resources/Star_fill.Star_fill'")));
					SetStarFill(ImageWidget, StarFillPath);
				}
			}

			
		}
	}
	
}


void USelectStageUI::SetStarFill(UImage* ImageWidget, FText* Path)
{
	const TCHAR* MyCharArr = *Path->ToString();
	UTexture2D* NewTexture = LoadObject<UTexture2D>(nullptr, MyCharArr);
	if (NewTexture)
	{
		FSlateBrush NewBrush;
		NewBrush.SetResourceObject(NewTexture);
		ImageWidget->SetBrush(NewBrush);
	}
}

FMusicInfoDT* USelectStageUI::FindRowByColumnValue(const FString& ColumnName, const FString& ColumnValue)
{
	if (!MusicDataTable) return nullptr;

	static const FString ContextString(TEXT("FindRowByColumnValue"));
	TArray<FName> RowNames = MusicDataTable->GetRowNames();

	for (const FName& RowName : RowNames)
	{
		FMusicInfoDT* Row = MusicDataTable->FindRow<FMusicInfoDT>(RowName, ContextString);
		if (Row)
		{
			if (ColumnName == "ArtistName" && Row->ArtistName == ColumnValue)
			{
				return Row;
			}
		}
	}

	return nullptr;
}
