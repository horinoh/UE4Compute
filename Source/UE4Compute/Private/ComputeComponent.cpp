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
	UAV = RHICreateUnorderedAccessView(Texture2DRHI);

	RandomFillTexture2D();
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
	ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER(Dispatch,
		UComputeComponent*, ComputeComp, this,
		{
			ComputeComp->Dispatch();
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
	if (nullptr != Texture2DRHI)
	{
		Colors.SetNumUninitialized(Texture2DRHI->GetSizeX() * Texture2DRHI->GetSizeY());

		Texture2D = UTexture2D::CreateTransient(Texture2DRHI->GetSizeX(), Texture2DRHI->GetSizeY(), Texture2DRHI->GetFormat());
		if(nullptr != Texture2D)
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

			Texture2D->UpdateResource();
		}
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
		ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(GetColors,
			FTexture2DRHIRef, Src, Texture2DRHI,
			TArray<FVector4>&, Dest, Colors,
			{
				uint32 Stride = 0;
				auto* SrcData = RHILockTexture2D(Src, 0, RLM_ReadOnly, Stride, false);
				const auto BB = GPixelFormats[Src->GetFormat()].BlockBytes;
				FMemory::Memcpy(Dest.GetData(), SrcData, BB * Src->GetSizeX() * Src->GetSizeY());
				RHIUnlockTexture2D(Src, 0, false);
			}
		);
		//!< Write content of Colors to Texture2D
		ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(UpdateTexture,
			TArray<FVector4>&, Src, Colors,
			UTexture2D*, Dest, Texture2D,
			{
				const auto W = Dest->Resource->GetSizeX();
				const auto H = Dest->Resource->GetSizeY();
				const auto BB = GPixelFormats[Dest->GetPixelFormat()].BlockBytes;
				RHIUpdateTexture2D(Dest->Resource->TextureRHI->GetTexture2D(), 0, FUpdateTextureRegion2D(0, 0, 0, 0, W, H), BB * W, reinterpret_cast<const uint8 *>(Src.GetData()));
			}
		);
	}
}
