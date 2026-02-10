#include "QuestSubsystem.h"
#include "QuestDataAsset.h"
#include "QuestRuntimeData.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Modules/ModuleManager.h"

void UQuestSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    // Clear previous states
    ActiveQuests.Empty();
    CompletedQuests.Empty();
    AllQuests.Empty();

    // Load all quests from a specific folder
    FString AssetPath = TEXT("/Game/Quest/Quests");

    // Get all assets in that folder
    TArray<FAssetData> AssetList;
    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
    FARFilter Filter;
    Filter.ClassNames.Add(UQuestDataAsset::StaticClass()->GetFName());
    Filter.bRecursivePaths = true;
    Filter.PackagePaths.Add(FName(*AssetPath));

    AssetRegistryModule.Get().GetAssets(Filter, AssetList);

    // Convert asset data into loaded objects
    for (const FAssetData& AssetData : AssetList)
    {
        UQuestDataAsset* QuestAsset = Cast<UQuestDataAsset>(AssetData.GetAsset());
        if (QuestAsset)
        {
            AllQuests.Add(QuestAsset);
        }
    }

    //UE_LOG(LogTemp, Warning, TEXT("Loaded %d quest assets"), AllQuests.Num());
    //for (UQuestDataAsset* Quest : AllQuests)
    //{
    //    UE_LOG(LogTemp, Warning, TEXT(" - %s"), *Quest->GetName());
    //}
}


// Start a quest using its data asset
bool UQuestSubsystem::StartQuest(UQuestDataAsset* QuestAsset)
{
    if (!QuestAsset || !QuestAsset->QuestID.IsValid())
    {
        return false;
    }

    if (ActiveQuests.Num() > 0)
    {
        return false; // only allow one active quest
    }

    FQuestRuntimeData Data;
    Data.QuestAsset = QuestAsset;
    Data.CurrentObjectiveIndex = 0;

    ActiveQuests.Add(QuestAsset->QuestID, Data);

    OnQuestStarted.Broadcast(QuestAsset->QuestID, Data);
    return true;
}

// Get the current active quest's objective
bool UQuestSubsystem::GetCurrentObjective(FQuestObjective& OutObjective) const
{
    if (ActiveQuests.Num() == 0)
    {
        return false;
    }

    const auto& Pair = *ActiveQuests.CreateConstIterator();
    const FQuestRuntimeData& Data = Pair.Value;

    if (!Data.QuestAsset || !Data.QuestAsset->Objectives.IsValidIndex(Data.CurrentObjectiveIndex))
    {
        return false;
    }

    OutObjective = Data.QuestAsset->Objectives[Data.CurrentObjectiveIndex];
    return true;
}

// Notify subsystem of an event, potentially advancing the current objective
void UQuestSubsystem::NotifyEvent(FGameplayTag EventTag)
{
    // UE_LOG(LogTemp, Warning, TEXT("NotifyEvent called with tag: %s"), *EventTag.ToString());

    if (!EventTag.IsValid())
    {
        return;
    }

    // Start any quests whose start tag matches the event
    for (UQuestDataAsset* QuestAsset : AllQuests)
    {
        if (!QuestAsset)
            continue;

        FGameplayTag QuestTag = QuestAsset->QuestID;

        // Skip if already active or completed
        if (ActiveQuests.Contains(QuestTag) || CompletedQuests.Contains(QuestTag))
            continue;

        // Check start tag
        if (QuestAsset->StartEventTag == EventTag)
        {
            FQuestRuntimeData NewData;
            NewData.QuestAsset = QuestAsset;
            NewData.CurrentObjectiveIndex = 0;
            ActiveQuests.Add(QuestTag, NewData);
            OnQuestStarted.Broadcast(QuestTag, NewData);
        }
    }

    // Update objectives for all active quests
    TArray<FGameplayTag> ActiveKeys;
    ActiveQuests.GetKeys(ActiveKeys);

    for (const FGameplayTag& QuestTag : ActiveKeys)
    {
        FQuestRuntimeData& Data = ActiveQuests[QuestTag];
        UQuestDataAsset* QuestAsset = Data.QuestAsset;

        if (!QuestAsset || !QuestAsset->Objectives.IsValidIndex(Data.CurrentObjectiveIndex))
            continue;

        FQuestObjective& CurrentObjective = QuestAsset->Objectives[Data.CurrentObjectiveIndex];

        if (CurrentObjective.RequiredEventTag == EventTag)
        {
            Data.CurrentObjectiveIndex++;
            OnQuestUpdated.Broadcast(QuestTag, Data);

            // Complete quest if done
            if (Data.CurrentObjectiveIndex >= QuestAsset->Objectives.Num())
            {
                ActiveQuests.Remove(QuestTag);
                CompletedQuests.Add(QuestTag);
                OnQuestCompleted.Broadcast(QuestTag, Data);
            }
        }
    }
}



// Get the current active quest (assuming only one active quest)
bool UQuestSubsystem::GetActiveQuest(FGameplayTag& OutQuestID, FQuestRuntimeData& OutQuestData) const
{
    if (ActiveQuests.Num() == 0)
    {
        return false;
    }

    const auto& Pair = *ActiveQuests.CreateConstIterator();
    OutQuestID = Pair.Key;
    OutQuestData = Pair.Value;
    return true;
}
