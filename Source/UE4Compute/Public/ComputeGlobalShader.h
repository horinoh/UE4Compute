// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GlobalShader.h"

/**
@brief ユニフォームバッファ
*/
BEGIN_UNIFORM_BUFFER_STRUCT(FComputeShaderUniformBuffer, )
DECLARE_UNIFORM_BUFFER_STRUCT_MEMBER(uint32, Iterations)
END_UNIFORM_BUFFER_STRUCT(FComputeShaderUniformBuffer)

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
	virtual ~FComputeGlobalShader() {}

	//!< FShader
	virtual bool Serialize(FArchive& Ar) override;

	static bool ShouldCache(EShaderPlatform Platform) { return IsFeatureLevelSupported(Platform, ERHIFeatureLevel::SM5); }
	static void ModifyCompilationEnvironment(EShaderPlatform Platform, FShaderCompilerEnvironment& OutEnvironment);

	void SetUniformBuffer(FRHICommandList& CommandList, const FComputeShaderUniformBuffer& UniformBuffer);
	void SetUAV(FRHICommandList& CommandList, const FUnorderedAccessViewRHIRef UAV);
	void UnsetUAV(FRHICommandList& CommandList);

private:
	FShaderResourceParameter ShaderResourceParam;
};
