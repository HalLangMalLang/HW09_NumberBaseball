// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "NBPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class NUMBERBASEBALL_API ANBPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	ANBPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	FString GetPlayerNameString();

	FString GetAttemptsString();

	void InitializeState(int32 InCurrentGuessCount, int32 InMaxGuessCount);

	UPROPERTY(Replicated)
	FString PlayerNameString;

	UPROPERTY(Replicated)
	int32 CurrentGuessCount;

	UPROPERTY(Replicated)
	int32 MaxGuessCount;
};
