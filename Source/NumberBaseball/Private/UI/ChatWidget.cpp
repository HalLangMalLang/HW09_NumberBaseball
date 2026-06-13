#include "UI/ChatWidget.h"
#include "Components\EditableTextBox.h"
#include "Player/NBPlayerController.h"

void UChatWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (EditableTextBoxChat->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnChatTextCommitted) == false)
	{
		EditableTextBoxChat->OnTextCommitted.AddDynamic(this, &ThisClass::OnChatTextCommitted);
	}
}

void UChatWidget::NativeDestruct()
{
	Super::NativeDestruct();

	if (EditableTextBoxChat->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnChatTextCommitted) == true)
	{
		EditableTextBoxChat->OnTextCommitted.RemoveDynamic(this, &ThisClass::OnChatTextCommitted);
	}
}

void UChatWidget::OnChatTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		if (APlayerController* OwningPlayerController = GetOwningPlayer())
		{
			if (ANBPlayerController* NBPlayerController = Cast<ANBPlayerController>(OwningPlayerController))
			{
				NBPlayerController->SetChatMessageString(Text.ToString());

				EditableTextBoxChat->SetText(FText());
			}
		}
	}
}