// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ComputeGlobalShader.h"

#include "Components/ActorComponent.h"
#include "ComputeComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE4COMPUTE_API UComputeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UComputeComponent();

	//!< UActorComponent
	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;

	void Dispatch();

	void RandomFillTexture2D();
	void ToTexture2D();
	UTexture2D* GetTexture2D() const { return Texture2D; }

private:
	FTexture2DRHIRef Texture2DRHI;
	FUnorderedAccessViewRHIRef UAV;
	FComputeShaderUniformBuffer UniformBuffer;

private:
	UPROPERTY(Transient)
	TArray<FVector4> Colors;
	UPROPERTY(VisibleAnywhere, Category = ShaderDebug)
	UTexture2D* Texture2D;
};
