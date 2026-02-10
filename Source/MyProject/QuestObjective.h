#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "QuestObjective.generated.h"


// The quest objective struct
USTRUCT(BlueprintType)
struct FQuestObjective
{
    GENERATED_BODY()

    // Description shown in UI
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText Description;

    // Tag used to trigger completion
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayTag RequiredEventTag;
};
