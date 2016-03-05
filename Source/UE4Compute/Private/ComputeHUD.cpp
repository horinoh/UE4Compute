// Fill out your copyright notice in the Description page of Project Settings.

#include "UE4Compute.h"
#include "ComputeHUD.h"

#include "ComputeComponent.h"

AComputeHUD::AComputeHUD(const FObjectInitializer& ObjectInitializer)
{
	ComputeComp = ObjectInitializer.CreateDefaultSubobject<UComputeComponent>(this, TEXT("Compute"));
}

void AComputeHUD::DrawHUD()
{
	Super::DrawHUD();

	if (nullptr != Canvas)
	{
		if (nullptr != ComputeComp)
		{
			const auto Texture2D = ComputeComp->GetTexture2D();
			if (nullptr != Texture2D)
			{
				const FVector2D ScreenSize(Canvas->SizeX, Canvas->SizeY);
				FCanvasTileItem Item(FVector2D::ZeroVector, Texture2D->Resource, ScreenSize, FLinearColor::White);
				Item.BlendMode = SE_BLEND_Opaque;
				Canvas->DrawItem(Item);
			}
		}
	}
}
