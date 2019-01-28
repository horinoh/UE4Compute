// Fill out your copyright notice in the Description page of Project Settings.

#include "UE4Compute.h"

class FComputeGameModuleImpl : public FDefaultGameModuleImpl
{
public:
	virtual void StartupModule() override
	{
		FDefaultGameModuleImpl::StartupModule();

		AddShaderSourceDirectoryMapping(TEXT("/Project"), FPaths::Combine(FPaths::ProjectDir(), TEXT("Shaders")));
	}
};

IMPLEMENT_PRIMARY_GAME_MODULE(FComputeGameModuleImpl, UE4Compute, "UE4Compute" );
