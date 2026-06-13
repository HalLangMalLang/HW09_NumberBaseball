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

	// 플레이어가 입력했을 때 처리할 함수
	void ProcessPlayerInput(ANBPlayerController* InChatPlayerController, const FString& InChatMessageString);

	FString GenerateBaseballNumber();

	bool IsMinigameInput(const FString& Message) const;

	FString CheckNumberMatch(ANBPlayerController* InChatPlayerController,const FString& InNumberString, const FString& InGuessNumberString);

	void IncreaseGuessCount(ANBPlayerController* InChatPlayerController);

	bool CountCheck(ANBPlayerController* InChatPlayerController);

	void ResetGame();

	void CheckGame(ANBPlayerController* InChatPlayerController, int InStrikeCount);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameRules")
	int32 TargetMatchCount = 3;

	FString BaseballNumber;

	UPROPERTY()
	TArray<TObjectPtr<ANBPlayerController>> AllPlayerControllers;

	UPROPERTY()
	TArray<TObjectPtr<ANBPlayerController>> GameControllers;

	FTimerHandle GameTimerHandle;

	UPROPERTY(EditAnywhere)
	int32 SetTurnTimeLeft = 30;

	int32 TurnTimeLeft = 10;

	void OnTimerTick();
	void StartGameTimer();

	void CheckAndTryStartGame();

	void AdvanceToNextTurn(const FString& PrefixMessage = TEXT(""));

	void OnTurnTimeout();

private:
	bool bIsGameRunning = false;

	int32 CurrentTurnIndex = 0;
};
