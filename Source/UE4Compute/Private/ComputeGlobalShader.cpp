// Fill out your copyright notice in the Description page of Project Settings.

#include "UE4Compute.h"
#include "ComputeGlobalShader.h"

#include "ShaderParameterUtils.h"

/**
@brief ���j�t�H�[���o�b�t�@
@note �V�F�[�_������g�p����閼�O�� "CSUniform" �ɂȂ�
*/
IMPLEMENT_UNIFORM_BUFFER_STRUCT(FComputeShaderUniformBuffer, TEXT("CSUniform"))

FComputeGlobalShader::FComputeGlobalShader(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
	: FGlobalShader(Initializer)
{
	//!< �V�F�[�_������g�p����閼�O�� "OutTexture" �ɂȂ�
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
@note �V�F�[�_�t�@�C������ "ComputeGlobalShader" (.usf�t�@�C�����A�t�@�C����Engine/Shaders/�ȉ��փR�s�[���Ă���)
@note �R�[�������V�F�[�_�֐����� "Main"
*/
IMPLEMENT_SHADER_TYPE(, FComputeGlobalShader, TEXT("ComputeGlobalShader"), TEXT("Main"), SF_Compute);