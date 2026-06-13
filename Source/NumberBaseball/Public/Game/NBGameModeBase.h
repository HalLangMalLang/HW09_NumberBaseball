// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NBGameModeBase.generated.h"

class ANBPlayerController;

UCLASS()
class NUMBERBASEBALL_API ANBGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void OnPostLogin(AController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	virtual void BeginPlay() override;

	// 타이머 콜백 - GameState에서 호출
	void OnTurnTimeout();

	// 게임 시작이 가능한지 확인
	void CheckAndTryStartGame();

	void GameStart();

	// 게임 재시작
	void ResetGame();

	// 입력 처리 - PlayerController에서 호출
	void ProcessPlayerInput(ANBPlayerController* InChatPlayerController, const FString& InChatMessageString);
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameRules")
	int32 TargetMatchCount = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameRules")
	int32 TurnTimeLeft = 30;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameRules")
	int32 GameStartTime = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameRules")
	int32 GameEndTime = 5;

	// 접속한 전체 유저 컨트롤러 관리
	UPROPERTY()
	TArray<TObjectPtr<ANBPlayerController>> AllPlayerControllers;

	// 게임중인 유저 컨트롤러 관리
	UPROPERTY()
	TArray<TObjectPtr<ANBPlayerController>> GameControllers;

	// TargetMatchCount개수 만큼 고유한 숫자(1~9, 중복 없음) 생성
	FString GenerateBaseballNumber();

	// 채팅이 게임 입력인지 확인
	bool IsMinigameInput(const FString& Message) const;

	// 중복 숫자 검사 및 Strike/Ball 계산
	FString CheckNumberMatch(ANBPlayerController* InChatPlayerController, const FString& InNumberString, const FString& InGuessNumberString);

private:
	// 게임 상태
	bool bIsGameRunning = false;
	int32 CurrentTurnIndex = 0;
	FString BaseballNumber;

	// 타이머 관리
	void OnTurnTimer();
	void ClearTurnTimer();
	void OnStartTimer();
	void OnEndTimer();

	// 턴 변경 알림
	void NotifyTurnChange(const FString& PrefixMessage = TEXT(""));

	// 플레이어 추측 횟수 증가
	void IncreaseGuessCount(ANBPlayerController* InChatPlayerController);

	// 남은 횟수가 있는지 확인
	bool CountCheck(ANBPlayerController* InChatPlayerController);

	// 게임 판별
	void CheckGame(ANBPlayerController* InChatPlayerController, int InStrikeCount);
};