// Fill out your copyright notice in the Description page of Project Settings.

#include "UE4Compute.h"
#include "ComputeGlobalShader.h"

#include "ShaderParameterUtils.h"
#include "ShaderCompilerCore.h"

//!< In shader use "CSUniform"
IMPLEMENT_GLOBAL_SHADER_PARAMETER_STRUCT(FComputeShaderUniformBuffer, "CSUniform");

FComputeGlobalShader::FComputeGlobalShader(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
	: FGlobalShader(Initializer)
{
	//!< In shader use "OutTexture"
	ShaderResourceParam.Bind(Initializer.ParameterMap, TEXT("OutTexture"));
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
	SetUniformBufferParameter(CommandList, CommandList.GetBoundComputeShader(), GetUniformBufferParameter<FComputeShaderUniformBuffer>(), Buffer);
}
void FComputeGlobalShader::SetUAV(FRHICommandList& CommandList, const FUnorderedAccessViewRHIRef UAV)
{
	if (ShaderResourceParam.IsBound())
	{
		CommandList.SetUAVParameter(CommandList.GetBoundComputeShader(), ShaderResourceParam.GetBaseIndex(), UAV);
	}
}
void FComputeGlobalShader::UnsetUAV(FRHICommandList& CommandList)
{
	if (ShaderResourceParam.IsBound())
	{
		CommandList.SetUAVParameter(CommandList.GetBoundComputeShader(), ShaderResourceParam.GetBaseIndex(), FUnorderedAccessViewRHIRef());
	}
}

IMPLEMENT_SHADER_TYPE(, FComputeGlobalShader, TEXT("/Project/Private/ComputeGlobalShader.usf"), TEXT("Main"), SF_Compute)