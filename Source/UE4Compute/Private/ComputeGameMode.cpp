// Fill out your copyright notice in the Description page of Project Settings.

#include "UE4Compute.h"
#include "ComputeGameMode.h"

#include "ComputeHUD.h"

AComputeGameMode::AComputeGameMode()
{
	HUDClass = AComputeHUD::StaticClass();
}
