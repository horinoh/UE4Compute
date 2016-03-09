// Fill out your copyright notice in the Description page of Project Settings.

#include "UE4Compute.h"
#include "ComputeGlobalShader.h"

#include "ShaderParameterUtils.h"

/**
@brief ユニフォームバッファ
@note シェーダ内から使用される名前が "CSUniform" になる
*/
IMPLEMENT_UNIFORM_BUFFER_STRUCT(FComputeShaderUniformBuffer, TEXT("CSUniform"))

FComputeGlobalShader::FComputeGlobalShader(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
	: FGlobalShader(Initializer)
{
	//!< シェーダ内から使用される名前が "OutTexture" になる
	ShaderResourceParam.Bind(Initializer.ParameterMap, TEXT("OutTexture"));
}

bool FComputeGlobalShader::Serialize(FArchive& Ar)
{
	auto HasOutdated = FGlobalShader::Serialize(Ar);
	Ar << ShaderResourceParam;
	return HasOutdated;
}

void FComputeGlobalShader::ModifyCompilationEnvironment(EShaderPlatform Platform, FShaderCompilerEnvironment& OutEnvironment)
{
	FGlobalShader::ModifyCompilationEnvironment(Platform, OutEnvironment);
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

/**
@note シェーダファイル名が "ComputeGlobalShader" (.usfファイル名、ファイルはEngine/Shaders/以下へコピーしておく)
@note コールされるシェーダ関数名が "Main"
*/
IMPLEMENT_SHADER_TYPE(, FComputeGlobalShader, TEXT("ComputeGlobalShader"), TEXT("Main"), SF_Compute);