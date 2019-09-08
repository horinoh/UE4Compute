// Fill out your copyright notice in the Description page of Project Settings.

#include "UE4Compute.h"
#include "ComputeComponent.h"

UComputeComponent::UComputeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
}

void UComputeComponent::InitializeComponent()
{
	Super::InitializeComponent();

	FRHIResourceCreateInfo CreateInfo;
	Texture2DRHI = RHICreateTexture2D(512, 512, PF_A32B32G32R32F, 1, 1, TexCreate_ShaderResource | TexCreate_UAV, CreateInfo);
	if (nullptr != Texture2DRHI)
	{
		UAV = RHICreateUnorderedAccessView(Texture2DRHI);

		Texture2D = UTexture2D::CreateTransient(Texture2DRHI->GetSizeX(), Texture2DRHI->GetSizeY(), Texture2DRHI->GetFormat());
		if (nullptr != Texture2D)
		{
			//RandomFillTexture2D();
			Texture2D->UpdateResource();
		}

		Colors.SetNumUninitialized(Texture2DRHI->GetSizeX() * Texture2DRHI->GetSizeY());
	}
}
void UComputeComponent::UninitializeComponent()
{
	Super::UninitializeComponent();

	Texture2DRHI.SafeRelease();
	UAV.SafeRelease();
}
void UComputeComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UniformBuffer.Iterations = 100;

	ENQUEUE_RENDER_COMMAND(Dispatch)(
		[this](FRHICommandList& RHICmdList)
		{
			Dispatch();
		}
	);

	ToTexture2D();
}

void UComputeComponent::Dispatch()
{
	if (nullptr != Texture2DRHI)
	{
		auto& CommandList = GRHICommandList.GetImmediateCommandList();
		TShaderMapRef<FComputeGlobalShader> Shader(GetGlobalShaderMap(ERHIFeatureLevel::Type::SM5));

		CommandList.SetComputeShader(Shader->GetComputeShader());

		Shader->SetUniformBuffer(CommandList, UniformBuffer);
		Shader->SetUAV(CommandList, UAV);
		{
			DispatchComputeShader(CommandList, *Shader, Texture2DRHI->GetSizeX() / 32, Texture2DRHI->GetSizeY() / 32, 1);
		}
		Shader->UnsetUAV(CommandList);
	}
}

void UComputeComponent::RandomFillTexture2D()
{
	if (nullptr != Texture2DRHI && nullptr != Texture2D)
	{
		auto& Mip = Texture2D->PlatformData->Mips[0];
		TArray<FVector4> DefaultColor;
		const auto Dim = Texture2D->GetSizeX() * Texture2D->GetSizeY();
		for (auto i = 0; i < Dim; ++i)
		{
			DefaultColor.Add(FVector4(FMath::RandRange(0.0f, 1.0f), FMath::RandRange(0.0f, 1.0f), FMath::RandRange(0.0f, 1.0f), 1.0f));
		}
		auto* Dest = Mip.BulkData.Lock(LOCK_READ_WRITE);
		FMemory::Memcpy(Dest, DefaultColor.GetData(), GPixelFormats[Texture2D->GetPixelFormat()].BlockBytes * DefaultColor.Num());
		Mip.BulkData.Unlock();
	}
}
void UComputeComponent::ToTexture2D()
{
	/**
	@note TODO
	Cannot find easy way of convert FTexture2DRHIRef to UTexture2D
	*/
	if (nullptr != Texture2DRHI && nullptr != Texture2D && nullptr != Texture2D->Resource)
	{
		//!< Write content of Texture2DRHI to Colors
		ENQUEUE_RENDER_COMMAND(GetColors)(
			[this](FRHICommandList& RHICmdList)
			{
				uint32 Stride = 0;
				auto* SrcData = RHILockTexture2D(Texture2DRHI, 0, RLM_ReadOnly, Stride, false);
				const auto BB = GPixelFormats[Texture2DRHI->GetFormat()].BlockBytes;
				FMemory::Memcpy(Colors.GetData(), SrcData, BB * Texture2DRHI->GetSizeX() * Texture2DRHI->GetSizeY());
				RHIUnlockTexture2D(Texture2DRHI, 0, false);
			}
		);

		//!< Write content of Colors to Texture2D
		ENQUEUE_RENDER_COMMAND(UpdateTexture)(
			[this](FRHICommandList& RHICmdList)
			{
				const auto W = Texture2D->Resource->GetSizeX();
				const auto H = Texture2D->Resource->GetSizeY();
				const auto BB = GPixelFormats[Texture2D->GetPixelFormat()].BlockBytes;
				RHIUpdateTexture2D(Texture2D->Resource->TextureRHI->GetTexture2D(), 0, FUpdateTextureRegion2D(0, 0, 0, 0, W, H), BB * W, reinterpret_cast<const uint8 *>(Colors.GetData()));
			}
		);
	}
}
