#pragma once

#include "CoreMinimal.h"
#include "QuestDataAsset.h"
#include "QuestRuntimeData.generated.h"

USTRUCT(BlueprintType)
struct FQuestRuntimeData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<UQuestDataAsset> QuestAsset = nullptr;

    UPROPERTY(BlueprintReadOnly)
    int32 CurrentObjectiveIndex = 0;
};
