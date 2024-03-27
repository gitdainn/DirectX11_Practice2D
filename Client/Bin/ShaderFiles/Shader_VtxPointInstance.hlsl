#include "Shader_Defines.hpp"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D		g_Texture;

vector			g_vCamPosition;


struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vPSize : PSIZE;	

	row_major matrix LocalMatrix : WORLD;
};

struct VS_OUT
{
	float4		vPosition : POSITION;	
	float2		vPSize : PSIZE;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;
	
	vector vPosition = mul(float4(In.vPosition, 1.f), In.LocalMatrix);

	matrix	WorldMatrix = g_WorldMatrix;

	WorldMatrix._11_12_13 = normalize(WorldMatrix._11_12_13);
	WorldMatrix._21_22_23 = normalize(WorldMatrix._21_22_23);
	WorldMatrix._31_32_33 = normalize(WorldMatrix._31_32_33);

	Out.vPosition = mul(vPosition, g_WorldMatrix);
	Out.vPSize = float2(In.vPSize.x * length(g_WorldMatrix._11_12_13), 
		In.vPSize.y * length(g_WorldMatrix._21_22_23));

	return Out;
}

struct GS_IN
{
	/*사각형을 구성하기위한 베이스 정보를 가진 정점.*/
	float4		vPosition : POSITION;
	float2		vPSize : PSIZE;
};

/* 지오메트리 셰이더에서 만들어낸 정점. */
struct GS_OUT
{	
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

/* 셰이더 내에서 정점을 생성해준다.  */
[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> Triangle)
{
	GS_OUT			Out[4];

	float3			vRight, vUp, vLook;

	vLook = g_vCamPosition.xyz - In[0].vPosition.xyz;

	vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * (In[0].vPSize.x * 0.5f);

	vUp = normalize(cross(vLook, vRight)) * (In[0].vPSize.y * 0.5f);

	Out[0].vPosition = vector(In[0].vPosition.xyz + vRight + vUp, 1.f);
	Out[0].vTexUV = float2(0.f, 0.f);

	Out[1].vPosition = vector(In[0].vPosition.xyz - vRight + vUp, 1.f);
	Out[1].vTexUV = float2(1.f, 0.f);

	Out[2].vPosition = vector(In[0].vPosition.xyz - vRight - vUp, 1.f);
	Out[2].vTexUV = float2(1.f, 1.f);

	Out[3].vPosition = vector(In[0].vPosition.xyz + vRight - vUp, 1.f);
	Out[3].vTexUV = float2(0.f, 1.f);

	matrix			matVP = mul(g_ViewMatrix, g_ProjMatrix);

	Out[0].vPosition = mul(Out[0].vPosition, matVP);
	Out[1].vPosition = mul(Out[1].vPosition, matVP);
	Out[2].vPosition = mul(Out[2].vPosition, matVP);
	Out[3].vPosition = mul(Out[3].vPosition, matVP);

	Triangle.Append(Out[0]);
	Triangle.Append(Out[1]);
	Triangle.Append(Out[2]);
	Triangle.RestartStrip();

	Triangle.Append(Out[0]);
	Triangle.Append(Out[2]);
	Triangle.Append(Out[3]);
	Triangle.RestartStrip();
}

struct PS_IN
{	
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;		
};

/* 픽셀의 색을 결정한다. */
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	
	Out.vColor = g_Texture.Sample(PointSampler, In.vTexUV);
	Out.vColor.rgb = float3(1.f, 0.f, 0.f);

	if (Out.vColor.a <= 0.1f)
		discard;

	return Out;
}

technique11		DefaultTechnique
{
	pass BackGround
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}