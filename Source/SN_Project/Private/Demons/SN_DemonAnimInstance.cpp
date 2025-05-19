// Fill out your copyright notice in the Description page of Project Settings.


#include "Demons/SN_DemonAnimInstance.h"

void USN_DemonAnimInstance::PlayMontage(UAnimMontage* Montage)
{

	if (!IsValid(Montage))
	{
		return;
	}
	
	Montage_Play(Montage);
	
}
