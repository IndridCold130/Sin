// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SinMeshComponent.h"

void USinMeshComponent::ManageMorphAdditive_Implementation(const FSinNamedFloat& Morph, bool Add)
{
	if (!GetSkeletalMeshAsset()->K2_GetAllMorphTargetNames().Contains(Morph.ID))
	{
		ManageDynamicBodyScalar(Morph, Add);
		return;
	}
	float PreviousValue = GetMorphTarget(Morph.ID);
	Add ? SetMorphTarget(Morph.ID, (GetMorphTarget(Morph.ID) + Morph.Value)) :
		SetMorphTarget(Morph.ID, (GetMorphTarget(Morph.ID) + Morph.Value * -1));
	OnSignalMorphChangedDelegate.Broadcast(Morph, GetMorphTarget(Morph.ID), PreviousValue);
}

void USinMeshComponent::ManageDynamicBodyScalar_Implementation(const FSinNamedFloat& Scalar, bool Add)
{
}


void USinMeshComponent::ManageMorphAdditives_Implementation(const TArray<FSinNamedFloat>& Morphs, bool Add)
{
	if (Morphs.IsEmpty())
	{
		return;
	}
	for (auto& Elem : Morphs)
	{
		ManageMorphAdditive(Elem, Add);
	}
}

void USinMeshComponent::SinSetMorph_Implementation(const FSinNamedFloat& Morph)
{
	if (!GetSkeletalMeshAsset()->K2_GetAllMorphTargetNames().Contains(Morph.ID))
	{
		ManageDynamicBodyScalar(Morph, true);
		return;
	}
	float PreviousValue = GetMorphTarget(Morph.ID);
	SetMorphTarget(Morph.ID, Morph.Value);
	OnSignalMorphChangedDelegate.Broadcast(Morph, GetMorphTarget(Morph.ID), PreviousValue);

}

void USinMeshComponent::SinSetMorphs_Implementation(const TArray<FSinNamedFloat>& Morphs)
{
	if (Morphs.IsEmpty())
	{
		return;
	}
	for (auto& Elem : Morphs)
	{
		SinSetMorph(Elem);
	}
}

void USinMeshComponent::SinGetMorphTarget(bool& Found, float& Value, FName MorphTargetName) const
{
	//Found = GetMorphTargetCurves().Contains(MorphTargetName);
	const float* CurveValPtr = GetMorphTargetCurves().Find(MorphTargetName);
	CurveValPtr ? Found = true : Found = false;
	if (CurveValPtr)
	{
		Value = *CurveValPtr;
	}
}
