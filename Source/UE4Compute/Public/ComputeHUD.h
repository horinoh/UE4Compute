// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/HUD.h"
#include "ComputeHUD.generated.h"

/**
 * 
 */
UCLASS()
class UE4COMPUTE_API AComputeHUD : public AHUD
{
	GENERATED_BODY()

public:
	AComputeHUD(const FObjectInitializer& ObjectInitializer);

	//!< AHUD
	virtual void DrawHUD() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Compute)
	class UComputeComponent* ComputeComp;
};
