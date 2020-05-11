// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GlobalShader.h"

/**
@brief ユニフォームバッファ
*/
BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT(FComputeShaderUniformBuffer, )
SHADER_PARAMETER(uint32, Iterations)
END_GLOBAL_SHADER_PARAMETER_STRUCT()

/**
 * 
 */
class UE4COMPUTE_API FComputeGlobalShader : public FGlobalShader
{
public:
	DECLARE_SHADER_TYPE(FComputeGlobalShader, Global);

public:
	FComputeGlobalShader() {}
	explicit FComputeGlobalShader(const ShaderMetaType::CompiledShaderInitializerType& Initializer);

	static bool ShouldCache(EShaderPlatform Platform) { return IsFeatureLevelSupported(Platform, ERHIFeatureLevel::SM5); }
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters) { return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5); }
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);

	void SetUniformBuffer(FRHICommandList& CommandList, const FComputeShaderUniformBuffer& UniformBuffer);
	void SetUAV(FRHICommandList& CommandList, const FUnorderedAccessViewRHIRef UAV);
	void UnsetUAV(FRHICommandList& CommandList);

private:
	FShaderResourceParameter ShaderResourceParam;
};
