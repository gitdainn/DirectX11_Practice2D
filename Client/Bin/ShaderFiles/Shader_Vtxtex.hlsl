#include "Shader_Defines.hpp"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D		g_Texture;
texture2D		g_DepthTexture;
float4			g_vColor;
int				g_iUVIndexX;
int				g_iUVIndexY;
int				g_iUVTexNumX;
int				g_iUVTexNumY;

struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos: TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;

	return Out;
}

struct PS_IN
{	
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos: TEXCOORD1;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;		
};

/* 픽셀의 색을 결정한다. */
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	
	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV);

    if (0.1 >= Out.vColor.a)
        discard;
	
    return Out;
}

/* 픽셀의 색을 결정한다. */
PS_OUT PS_UV_ANIM(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    float fX = (In.vTexUV.x + g_iUVIndexX) / g_iUVTexNumX;
    float fY = (In.vTexUV.y + g_iUVIndexY) / g_iUVTexNumY;
    float2 NewUV = float2(fX, fY);
	
	/* @note - LinearSampler는 hpp에서 정의한 밉맵 방식
	우리는 현재 wrap 방식으로 1을 초과하면 반복되도록 사용 중
	*/
    Out.vColor = g_Texture.Sample(LinearSampler, NewUV);

    if (0.1 >= Out.vColor.a)
        discard;
	
    return Out;
}

PS_OUT PS_MAIN_SOFTEFFECT(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV);

	float2		vUV;

	vUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	float4		vDepthDesc = g_DepthTexture.Sample(PointSampler, vUV);

	float		fOldZ = vDepthDesc.y * 300.f;
	float		fViewZ = In.vProjPos.w;

	Out.vColor.a = Out.vColor.a * saturate(fOldZ - fViewZ);

	return Out;
}

technique11		DefaultTechnique
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
}