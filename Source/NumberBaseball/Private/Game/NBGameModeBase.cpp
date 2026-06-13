// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/NBGameModeBase.h"
#include "Game/NBGameStateBase.h"
#include "Player/NBPlayerController.h"
#include "Player/NBPlayerState.h"
#include "EngineUtils.h"

void ANBGameModeBase::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	if (ANBPlayerController* NBPlayerController = Cast<ANBPlayerController>(NewPlayer))
	{
		NBPlayerController->NotificationText = FText::FromString(TEXT("Connected to the game server"));

		AllPlayerControllers.Add(NBPlayerController);

		ANBPlayerState* NBPS = NBPlayerController->GetPlayerState<ANBPlayerState>();
		if (IsValid(NBPS))
		{
			NBPS->InitializeState(0, TargetMatchCount);
			NBPS->PlayerNameString = TEXT("Player") + FString::FromInt(AllPlayerControllers.Num());
		}

		if (ANBGameStateBase* NBGS = GetGameState<ANBGameStateBase>())
		{
			NBGS->MulticastRPCBroadcastLoginMessage(NBPS->PlayerNameString);
		}
	}

	CheckAndTryStartGame();
}

void ANBGameModeBase::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	ANBPlayerController* ExitPC = Cast<ANBPlayerController>(Exiting);
	if (!ExitPC) return;

	AllPlayerControllers.Remove(ExitPC);

	if (GameControllers.Contains(ExitPC))
	{
		int32 ExitIndex = GameControllers.Find(ExitPC);
		bool bCurrentTurn = (CurrentTurnIndex == ExitIndex);

		GameControllers.Remove(ExitPC);

		if (bIsGameRunning && GameControllers.Num() < 2)
		{
			ClearTurnTimer();
			for (auto& NBPC : GameControllers)
			{
				if (NBPC)
				{
					NBPC->NotificationText = FText::FromString(TEXT("상대방이 탈주하여 승리했습니다!"));
				}
			}
			OnEndTimer();
			return;
		}

		if (bIsGameRunning)
		{
			if (CurrentTurnIndex > ExitIndex)
			{
				CurrentTurnIndex--;
			}

			if (bCurrentTurn)
			{
				if (CurrentTurnIndex >= GameControllers.Num())
				{
					CurrentTurnIndex = 0;
				}

				OnTurnTimer();

				NotifyTurnChange(TEXT("현재 턴 플레이어가 퇴장하여 턴이 넘어갑니다."));
			}
		}
	}
}

void ANBGameModeBase::BeginPlay()
{
	Super::BeginPlay();
}

void ANBGameModeBase::ProcessPlayerInput(ANBPlayerController* InChatPlayerController, const FString& InChatMessageString)
{
	FString ChatMessageString;

	if (bIsGameRunning && IsMinigameInput(InChatMessageString))
	{
		if (GameControllers[CurrentTurnIndex] != InChatPlayerController)
		{
			InChatPlayerController->ClientRPCPrintChatMessageString(TEXT("당신의 턴이 아닙니다!"));
			return;
		}

		if (CountCheck(InChatPlayerController))
		{
			OnTurnTimer();

			FString CheckNumberMatchString = CheckNumberMatch(InChatPlayerController, BaseballNumber, InChatMessageString);
			int32 StrikeCount = FCString::Atoi(*CheckNumberMatchString.Left(1));

			if (ANBPlayerState* NBPlayerState = InChatPlayerController->GetPlayerState<ANBPlayerState>())
			{
				ChatMessageString = FString::Printf(TEXT("%s : %s -> %s"), *NBPlayerState->GetPlayerNameString(), *InChatMessageString, *CheckNumberMatchString);
				if (StrikeCount != TargetMatchCount)
				{
					ChatMessageString += NBPlayerState->GetAttemptsString();
				}

				for (ANBPlayerController* NBPC : AllPlayerControllers)
				{
					if (NBPC)
					{
						NBPC->ClientRPCPrintChatMessageString(ChatMessageString);
					}
				}

				CheckGame(InChatPlayerController, StrikeCount);
			}
		}
	}
	else
	{
		if (ANBPlayerState* NBPlayerState = InChatPlayerController->GetPlayerState<ANBPlayerState>())
		{
			ChatMessageString = FString::Printf(TEXT("%s : %s"), *NBPlayerState->GetPlayerNameString(), *InChatMessageString);
			for (TActorIterator<ANBPlayerController> It(GetWorld()); It; ++It)
			{
				if (ANBPlayerController* NBPlayerController = *It)
				{
					NBPlayerController->ClientRPCPrintChatMessageString(ChatMessageString);
				}
			}
		}
	}
}

FString ANBGameModeBase::GenerateBaseballNumber()
{
	TArray<int32> Numbers;

	for (int32 i = 1; i <= 9; i++)
	{
		Numbers.Add(i);
	}

	FString Result;

	for (int32 i = 0; i < TargetMatchCount; i++)
	{
		int32 Index = FMath::RandRange(0, Numbers.Num() - 1);
		Result.Append(FString::FromInt(Numbers[Index]));
		Numbers.RemoveAt(Index);
	}

	UE_LOG(LogTemp, Log, TEXT("%s"), *Result);

	return Result;
}

bool ANBGameModeBase::IsMinigameInput(const FString& Message) const
{
	if (Message.Len() != TargetMatchCount)
	{
		return false;
	}

	for (TCHAR C : Message)
	{
		if (!FChar::IsDigit(C) || C == '0')
		{
			return false;
		}
	}

	return true;
}

FString ANBGameModeBase::CheckNumberMatch(ANBPlayerController* InChatPlayerController, const FString& InNumberString, const FString& InGuessNumberString)
{
	TSet<TCHAR> UniqueDigits;

	for (int32 i = 0; i < TargetMatchCount; i++)
	{
		UniqueDigits.Add(InGuessNumberString[i]);
	}

	if (UniqueDigits.Num() != TargetMatchCount)
	{
		return TEXT("0 다시 입력하세요.");
	}


	int32 StrikeCount = 0, BallCount = 0;

	IncreaseGuessCount(InChatPlayerController);

	for (int32 i = 0; i < TargetMatchCount; i++)
	{
		if (InNumberString[i] == InGuessNumberString[i])
		{
			StrikeCount++;
		}
		else
		{
			FString PlayerGuessChar = FString::Printf(TEXT("%c"), InGuessNumberString[i]);
			if (InNumberString.Contains(PlayerGuessChar))
			{
				BallCount++;
			}
		}
	}

	if (StrikeCount == 0 && BallCount == 0)
	{
		return TEXT("0 OUT");
	}

	return FString::Printf(TEXT("%dS %dB"), StrikeCount, BallCount);
}

void ANBGameModeBase::IncreaseGuessCount(ANBPlayerController* InChatPlayerController)
{
	if (ANBPlayerState* NBPS = InChatPlayerController->GetPlayerState<ANBPlayerState>())
	{
		NBPS->CurrentGuessCount++;
	}
}

bool ANBGameModeBase::CountCheck(ANBPlayerController* InChatPlayerController)
{
	if (ANBPlayerState* NBPS = InChatPlayerController->GetPlayerState<ANBPlayerState>())
	{
		return NBPS->CurrentGuessCount < TargetMatchCount ? true : false;
	}

	return false;
}

void ANBGameModeBase::ResetGame()
{
	bIsGameRunning = false;

	if (ANBGameStateBase* NBGameState = GetGameState<ANBGameStateBase>())
	{
		NBGameState->ResetPlayersCurrentCount();
	}

	CheckAndTryStartGame();
}

void ANBGameModeBase::CheckGame(ANBPlayerController* InChatPlayerController, int InStrikeCount)
{
	if (InStrikeCount == TargetMatchCount)
	{
		ClearTurnTimer();
		ANBPlayerState* NBPS = InChatPlayerController->GetPlayerState<ANBPlayerState>();
		for (const auto& NBPC : GameControllers)
		{
			if (NBPC)
			{
				FString CombinedMessageString = NBPS->GetPlayerNameString() + TEXT(" 승리!");
				NBPC->NotificationText = FText::FromString(CombinedMessageString);
			}
		}
		OnEndTimer();
	}
	else
	{
		bool bIsDraw = true;
		for (const auto& NBPC : AllPlayerControllers)
		{
			if (ANBPlayerState* NBPS = NBPC->GetPlayerState<ANBPlayerState>())
			{
				if (NBPS->CurrentGuessCount < NBPS->MaxGuessCount)
				{
					bIsDraw = false;
					break;
				}
			}
		}

		if (bIsDraw)
		{
			ClearTurnTimer();
			for (const auto& NBPC : AllPlayerControllers)
			{
				NBPC->NotificationText = FText::FromString(TEXT("무승부!"));

			}
			OnEndTimer();
		}
		else
		{
			CurrentTurnIndex = (CurrentTurnIndex + 1) % GameControllers.Num();
			NotifyTurnChange();
		}
	}
}

void ANBGameModeBase::OnTurnTimer()
{
	if (ANBGameStateBase* GS = GetGameState<ANBGameStateBase>())
	{
		GS->StartTurnTimer(TurnTimeLeft);
	}
}

void ANBGameModeBase::ClearTurnTimer()
{
	if (ANBGameStateBase* GS = GetGameState<ANBGameStateBase>())
	{
		GS->ClearTurnTimer();
	}
}

void ANBGameModeBase::OnStartTimer()
{
	if (ANBGameStateBase* GS = GetGameState<ANBGameStateBase>())
	{
		GS->GameStartTimer(GameStartTime);
	}
}

void ANBGameModeBase::OnEndTimer()
{
	if (ANBGameStateBase* GS = GetGameState<ANBGameStateBase>())
	{
		GS->GameEndTimer(GameEndTime);
	}
}

void ANBGameModeBase::CheckAndTryStartGame()
{
	if (bIsGameRunning) return;

	if (AllPlayerControllers.Num() >= 2)
	{
		bIsGameRunning = true;

		GameControllers.Empty();
		for (const auto& NBPC : AllPlayerControllers)
		{
			if (NBPC)
			{
				GameControllers.Add(NBPC);

				NBPC->NotificationText = FText::FromString(TEXT("인원이 충족되어 게임이 시작됩니다!"));
			}
		}

		OnStartTimer();
	}
	else
	{
		for (const auto& NBPC : AllPlayerControllers)
		{
			if (NBPC)
			{
				NBPC->NotificationText = FText::FromString(TEXT("다른 플레이어를 기다리는 중..."));
			}
		}
	}
}

void ANBGameModeBase::GameStart()
{
	CurrentTurnIndex = 0;

	BaseballNumber = GenerateBaseballNumber();

	OnTurnTimer();

	NotifyTurnChange();
}

void ANBGameModeBase::NotifyTurnChange(const FString& PrefixMessage)
{
	if (GameControllers.IsEmpty()) return;

	if (ANBPlayerController* CurrentPC = GameControllers[CurrentTurnIndex])
	{
		FString TurnMessage = PrefixMessage + TEXT(" ");

		if (ANBPlayerState* NBPS = CurrentPC->GetPlayerState<ANBPlayerState>())
		{
			TurnMessage += FString::Printf(TEXT("현재 턴 : %s"), NBPS ? *NBPS->GetPlayerNameString() : TEXT("알 수 없음"));

			for (auto& NBPC : GameControllers)
			{
				NBPC->NotificationText = FText::FromString(TurnMessage);
			}
		}
	}
}

void ANBGameModeBase::OnTurnTimeout()
{
	if (ANBPlayerController* CurrentPC = GameControllers[CurrentTurnIndex])
	{
		if (ANBPlayerState* NBPS = CurrentPC->GetPlayerState<ANBPlayerState>())
		{
			NBPS->CurrentGuessCount++;
		}
	}

	CurrentTurnIndex = (CurrentTurnIndex + 1) % GameControllers.Num();

	OnTurnTimer();

	NotifyTurnChange(TEXT("시간 초과! 턴이 넘어갑니다."));
}