// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomGameViewportClient.h"

#include "Engine/Canvas.h"

UCustomGameViewportClient::UCustomGameViewportClient()
{
    bFading = false;
    bToBlack = false;
    FadeAlpha = 0.0f;
    FadeStartTime = 0.0f;
    FadeDuration = 1.0f;
}

void UCustomGameViewportClient::PostRender(UCanvas* Canvas)
{
    Super::PostRender(Canvas);

    if (bFading)
    {
        DrawScreenFade(Canvas);
    }
}

void UCustomGameViewportClient::ClearFade()
{
    bFading = false;
}

void UCustomGameViewportClient::Fade(const float Duration, const bool ToBlack)
{
    if (World)
    {
        bFading = true;
        bToBlack = ToBlack;
        FadeDuration = Duration;
        FadeStartTime = World->GetTimeSeconds();
    }
}

void UCustomGameViewportClient::DrawScreenFade(UCanvas* Canvas)
{
    if (bFading)
    {
        if (World)
        {
            const float Time = World->GetTimeSeconds();
            const float Alpha = FMath::Clamp((Time - FadeStartTime) / FadeDuration, 0.f, 1.f);

            if (Alpha == 1.f && !bToBlack)
            {
                bFading = false;
            }
            else
            {
                FColor OldColor = Canvas->DrawColor;
                FLinearColor FadeColor = FLinearColor::Black;
                FadeColor.A = bToBlack ? Alpha : 1 - Alpha;
                FVector2D ViewportSize;
                GEngine->GameViewport->GetViewportSize(ViewportSize);
                Canvas->SetClip(Canvas->ClipX + ViewportSize.X, Canvas->ClipY + ViewportSize.Y);
                Canvas->DrawColor = FadeColor.ToFColor(true);
                Canvas->DrawTile(Canvas->DefaultTexture,
                    0, 0,
                    Canvas->ClipX, Canvas->ClipY,
                    0, 0,
                    Canvas->DefaultTexture->GetSizeX(), Canvas->DefaultTexture->GetSizeY());
                Canvas->DrawColor = OldColor;
                
            }
        }
    }
}