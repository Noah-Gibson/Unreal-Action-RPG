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
	QuestRuntimeMap.Empty();
    AllQuests.Empty();

    // Load all quests from a specific folder
    FString AssetPath = TEXT("/Game/MyGame/Quest/Quests");

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

    for (const auto& Pair : QuestRuntimeMap)
    {
        if (Pair.Value.QuestState == EQuestState::Active)
        {
            return false; // only allow one active quest for now
        }
	}

	FQuestRuntimeData Data = QuestRuntimeMap.FindOrAdd(QuestAsset->QuestID);
    Data.QuestAsset = QuestAsset;
    Data.CurrentObjectiveIndex = 0;
	Data.QuestState = EQuestState::Active;

    OnQuestStarted.Broadcast(QuestAsset->QuestID, Data);
    return true;
}

// Get the current active quest's objective
bool UQuestSubsystem::GetCurrentObjective(FQuestObjective& OutObjective) const
{
    for (const auto& Pair : QuestRuntimeMap)
    {
        const FQuestRuntimeData& Data = Pair.Value;

        if (Data.QuestState == EQuestState::Active)
        {
            if (!Data.QuestAsset ||
                !Data.QuestAsset->Objectives.IsValidIndex(Data.CurrentObjectiveIndex))
            {
                return false;
            }

            OutObjective = Data.QuestAsset->Objectives[Data.CurrentObjectiveIndex];
            return true;
        }
    }

    return false; // no active quest found
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
        FQuestRuntimeData* ExistingData = QuestRuntimeMap.Find(QuestTag);

        if (ExistingData &&
            (ExistingData->QuestState == EQuestState::Active ||
                ExistingData->QuestState == EQuestState::Completed))
        {
            continue;
        }

        // Check start tag
        if (QuestAsset->StartEventTag == EventTag)
        {
            FQuestRuntimeData& NewData = QuestRuntimeMap.FindOrAdd(QuestTag);
            NewData.QuestAsset = QuestAsset;
            NewData.CurrentObjectiveIndex = 0;
            NewData.QuestState = EQuestState::Active;
            OnQuestStarted.Broadcast(QuestTag, NewData);
        }
    }

    // Update objectives for all active quests
    for (auto& Pair : QuestRuntimeMap)
    {
        FGameplayTag QuestTag = Pair.Key;
        FQuestRuntimeData& Data = Pair.Value;

        if (Data.QuestState != EQuestState::Active)
            continue;

        UQuestDataAsset* QuestAsset = Data.QuestAsset;

        if (!QuestAsset ||
            !QuestAsset->Objectives.IsValidIndex(Data.CurrentObjectiveIndex))
            continue;

        FQuestObjective& CurrentObjective =
            QuestAsset->Objectives[Data.CurrentObjectiveIndex];

        if (CurrentObjective.RequiredEventTag == EventTag)
        {
            Data.CurrentObjectiveIndex++;
            OnQuestUpdated.Broadcast(QuestTag, Data);

            // Complete quest if done
            if (Data.CurrentObjectiveIndex >= QuestAsset->Objectives.Num())
            {
                Data.QuestState = EQuestState::Completed;
                OnQuestCompleted.Broadcast(QuestTag, Data);
            }
        }
    }
}


// Get the current active quest (assuming only one active quest)
bool UQuestSubsystem::GetActiveQuest(FGameplayTag& OutQuestID, FQuestRuntimeData& OutQuestData) const
{
    for (const auto& Pair : QuestRuntimeMap)
    {
        if (Pair.Value.QuestState == EQuestState::Active)
        {
            OutQuestID = Pair.Key;
            OutQuestData = Pair.Value;
            return true;
        }
    }

    return false;
}

bool UQuestSubsystem::IsQuestActive(FGameplayTag QuestID) const
{
    const FQuestRuntimeData* Data = QuestRuntimeMap.Find(QuestID);

    return Data && Data->QuestState == EQuestState::Active;
}


bool UQuestSubsystem::IsQuestCompleted(FGameplayTag QuestID) const
{
    const FQuestRuntimeData* Data = QuestRuntimeMap.Find(QuestID);

    return Data && Data->QuestState == EQuestState::Completed;
}

bool UQuestSubsystem::IsQuestNotStarted(FGameplayTag QuestID) const
{
    const FQuestRuntimeData* Data = QuestRuntimeMap.Find(QuestID);

    // If it doesn't exist in the map OR explicitly marked NotStarted
    return !Data || Data->QuestState == EQuestState::NotStarted;
}

FQuestRuntimeData UQuestSubsystem::GetQuestData(FGameplayTag QuestID) const
{
    const FQuestRuntimeData* Data = QuestRuntimeMap.Find(QuestID);
    return Data ? *Data : FQuestRuntimeData{};
}
