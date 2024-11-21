#include "Shader_Defines.hpp"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_Texture;
texture2D g_MaskTexture;
texture2D g_DepthTexture;

float g_fNormalizedX = { 1.f };
float g_fNormalizedY = { 1.f };

float3 g_ObjectSize;
float2 g_TextureSize;

float4 g_vColor;
int g_iUVIndexX;
int g_iUVIndexY;
int g_iUVTexNumX;
int g_iUVTexNumY;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexUV : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexUV : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexUV = In.vTexUV;
    Out.vProjPos = Out.vPosition;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexUV : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

/* 픽셀의 색을 결정한다. */
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV);

    if (0.1 >= Out.vColor.a)
        discard;
	
    return Out;
}

/* 픽셀의 색을 결정한다. */
PS_OUT PS_COLOR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV);

    if (0.1 >= Out.vColor.a)
        discard;
	
    // saturate(In): 0~1 범위로 반환
    Out.vColor.r = saturate(Out.vColor.r + g_vColor.x);
    Out.vColor.g = saturate(Out.vColor.g + g_vColor.y);
    Out.vColor.b = saturate(Out.vColor.b + g_vColor.z);
    
    return Out;
}

/* 픽셀의 색을 결정한다. */
PS_OUT PS_UV_ANIM(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    float fX = (In.vTexUV.x + g_iUVIndexX) / g_iUVTexNumX;
    float fY = (In.vTexUV.y + g_iUVIndexY) / g_iUVTexNumY;
    // float fX = (In.vTexUV.x) / g_iUVTexNumX +g_iUVIndexX; 
    // float fY = (In.vTexUV.y) / g_iUVTexNumY + g_iUVIndexY;
    float2 NewUV = float2(fX, fY);
	
	/* @note - LinearSampler는 hpp에서 정의한 밉맵 방식
	우리는 현재 wrap 방식으로 1을 초과하면 반복되도록 사용 중
	*/
    Out.vColor = g_Texture.Sample(LinearSampler, NewUV);

    if (0.1 >= Out.vColor.a)
        discard;
	
    return Out;
}

PS_OUT PS_FlipUV_ANIM(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
	// UV를 좌우반전하여 순서도 반전됐으므로 조정 (만약 가로 5인데 3까지 있으면 0, 1, 2가 아니라 4, 3, 2로 접근해야함)
    float fX = saturate(g_iUVTexNumX - g_iUVIndexX - 1) * 0.1 - (In.vTexUV.x + (g_iUVTexNumX - g_iUVIndexX)) / g_iUVTexNumX;
    float fY = (In.vTexUV.y + g_iUVIndexY) / g_iUVTexNumY;
    float2 NewUV = float2(fX, fY);
	
	/* @note - LinearSampler는 hpp에서 정의한 밉맵 방식
	우리는 현재 wrap 방식으로 1을 초과하면 반복되도록 사용 중
	g_Texture라는 텍스처를 0~1 범위의 UV 좌표로 변경
	*/
    Out.vColor = g_Texture.Sample(LinearSampler, NewUV);

    if (0.1 >= Out.vColor.a)
        discard;
	
    return Out;
}

PS_OUT PS_Discard(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    // LinearSampler는 텍스처UV를 크기만큼 반복해서 채움. (wrap)
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV);
    
    if (g_fNormalizedX < In.vTexUV.x)
        discard;
	
    if (g_fNormalizedY < In.vTexUV.y)
        discard;

    return Out;
}

/** 오브젝트 크기만큼 x축 이미지를 반복해서 출력합니다. */
PS_OUT PS_WRAP_X(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float fWrapNum = g_ObjectSize.x / g_TextureSize;
    In.vTexUV.x *= fWrapNum;
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV);
    
    return Out;
}

PS_OUT PS_MAIN_SOFTEFFECT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV);

    float2 vUV;

    vUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
    vUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

    float4 vDepthDesc = g_DepthTexture.Sample(PointSampler, vUV);

    float fOldZ = vDepthDesc.y * 300.f;
    float fViewZ = In.vProjPos.w;

    Out.vColor.a = Out.vColor.a * saturate(fOldZ - fViewZ);

    return Out;
}

technique11 DefaultTechnique
{
    pass Default
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass Color
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_COLOR();
    }

    pass UV_ANIM
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_UV_ANIM();
    }

    pass UV_INVERSION_ANIM
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_FlipUV_ANIM();
    }

    pass UV_DISCARD
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Discard();
    }

    pass UV_WRAP_X
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_WRAP_X();
    }
}