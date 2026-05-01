// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/SinInputConfig.h"

#include "GameplayTagContainer.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SinInputConfig)

USinInputConfig::USinInputConfig(const FObjectInitializer& ObjectInitializer)
{
}

const UInputAction* USinInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag) const
{
    for (const FSinInputAction& Action : AbilityInputActions)
    {
        if (Action.InputAction && (Action.InputTag == InputTag))
        {
            return Action.InputAction;
        }
    }
    return nullptr;
}

const UInputAction* USinInputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag) const
{
    for (const FSinInputAction& TaggedInputAction : NativeInputActions)
    {
        if (TaggedInputAction.InputAction && TaggedInputAction.InputTag == InputTag)
        {
            return TaggedInputAction.InputAction;
        }
    }

    return nullptr;
}