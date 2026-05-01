// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SinGlobalStructs.h"
#include "Components/SkeletalMeshComponent.h"
#include "SinMeshComponent.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FMorphUpdateSignature, const FSinNamedFloat, Morph, float, New, float, Delta);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FMorphUpdateSignature, FName, ID, float, New, float, Delta);

UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SIN_API USinMeshComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Morphs")
		void ManageMorphAdditive(const FSinNamedFloat& Morph, bool Add);
		virtual void ManageMorphAdditive_Implementation(const FSinNamedFloat& Morph, bool Add);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Morphs")
		void ManageDynamicBodyScalar(const FSinNamedFloat& Scalar, bool Add);
		virtual void ManageDynamicBodyScalar_Implementation(const FSinNamedFloat& Scalar, bool Add);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Morphs")
		void ManageMorphAdditives(const TArray<FSinNamedFloat>& Morphs, bool Add);
		virtual void ManageMorphAdditives_Implementation(const TArray<FSinNamedFloat>& Morphs, bool Add);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Morphs")
		void SinSetMorph(const FSinNamedFloat& Morph);
		virtual void SinSetMorph_Implementation(const FSinNamedFloat& Morph);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Morphs")
		void SinSetMorphs(const TArray<FSinNamedFloat>& Morphs);
		virtual void SinSetMorphs_Implementation(const TArray<FSinNamedFloat>& Morphs);

	/**
	 * Get Morph target with given name
	 */
	UFUNCTION(BlueprintCallable, Category="Components|SkeletalMesh")
		void SinGetMorphTarget(bool& Found, float& Value, FName MorphTargetName) const;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Morphs")
		FMorphUpdateSignature OnSignalMorphChangedDelegate;
};
