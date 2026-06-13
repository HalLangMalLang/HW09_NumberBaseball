#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "NBGameStateBase.generated.h"


UCLASS()
class NUMBERBASEBALL_API ANBGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	ANBGameStateBase();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCBroadcastLoginMessage(const FString& InNameString = FString(TEXT("xxxxx")));

	// 턴 남은 시간(클라이언트 동기화)
	UPROPERTY(ReplicatedUsing = OnRep_TurnTimeLeft)
	int32 TurnTimeLeft = 0;

	// 타이머 관리 (GameMode에서 호출)
	void StartTurnTimer(int SetTimerValue);
	void ClearTurnTimer();

	void GameStartTimer(int SetTimerValue);
	void GameEndTimer(int SetTimerValue);

	// 플레이어 상태 관리(GameMode에서 호출)
	void ResetPlayersCurrentCount();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	FTimerHandle TurnTimerHandle;
	FTimerHandle GameStartTimerHandle;
	FTimerHandle GameEndTimerHandle;

	void OnTurnTimerTick();
	void OnGameStartTimer();
	void OnGameEndTimer();

	UFUNCTION()
	void OnRep_TurnTimeLeft();
};