// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChatWidget.generated.h"

class UEditableTextBox;

UCLASS()
class NUMBERBASEBALL_API UChatWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

public:
	UFUNCTION()
	void OnChatTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> EditableTextBoxChat;
};
