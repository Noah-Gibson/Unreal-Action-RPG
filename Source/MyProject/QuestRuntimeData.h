#pragma once

#include "CoreMinimal.h"
#include "QuestDataAsset.h"
#include "QuestRuntimeData.generated.h"


UENUM(BlueprintType)
enum class EQuestState : uint8
{
    NotStarted,
    Active,
    Completed,
    Failed
};

USTRUCT(BlueprintType)
struct FQuestRuntimeData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<UQuestDataAsset> QuestAsset = nullptr;

    UPROPERTY(BlueprintReadOnly)
    int32 CurrentObjectiveIndex = 0;

    UPROPERTY(BlueprintReadOnly)
    EQuestState QuestState = EQuestState::NotStarted;
};
