// Fill out your copyright notice in the Description page of Project Settings.

#include "UE4Compute.h"
#include "ComputeGlobalShader.h"

#include "ShaderParameterUtils.h"

//!< In shader use "CSUniform"
IMPLEMENT_GLOBAL_SHADER_PARAMETER_STRUCT(FComputeShaderUniformBuffer, "CSUniform");

FComputeGlobalShader::FComputeGlobalShader(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
	: FGlobalShader(Initializer)
{
	//!< In shader use "OutTexture"
	ShaderResourceParam.Bind(Initializer.ParameterMap, TEXT("OutTexture"));
}

bool FComputeGlobalShader::Serialize(FArchive& Ar)
{
	auto HasOutdated = FGlobalShader::Serialize(Ar);
	Ar << ShaderResourceParam;
	return HasOutdated;
}

void FComputeGlobalShader::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
{
	FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	OutEnvironment.CompilerFlags.Add(CFLAG_StandardOptimization);
}

void FComputeGlobalShader::SetUniformBuffer(FRHICommandList& CommandList, const FComputeShaderUniformBuffer& UniformBuffer)
{
	TUniformBufferRef<FComputeShaderUniformBuffer> Buffer;
	Buffer = TUniformBufferRef<FComputeShaderUniformBuffer>::CreateUniformBufferImmediate(UniformBuffer, UniformBuffer_SingleDraw);
	SetUniformBufferParameter(CommandList, GetComputeShader(), GetUniformBufferParameter<FComputeShaderUniformBuffer>(), Buffer);
}
void FComputeGlobalShader::SetUAV(FRHICommandList& CommandList, const FUnorderedAccessViewRHIRef UAV)
{
	if (ShaderResourceParam.IsBound())
	{
		CommandList.SetUAVParameter(GetComputeShader(), ShaderResourceParam.GetBaseIndex(), UAV);
	}
}
void FComputeGlobalShader::UnsetUAV(FRHICommandList& CommandList)
{
	if (ShaderResourceParam.IsBound())
	{
		CommandList.SetUAVParameter(GetComputeShader(), ShaderResourceParam.GetBaseIndex(), FUnorderedAccessViewRHIRef());
	}
}

IMPLEMENT_SHADER_TYPE(, FComputeGlobalShader, TEXT("/Project/Private/ComputeGlobalShader.usf"), TEXT("Main"), SF_Compute)