


/* For.Sampler State  */
sampler PointSampler = sampler_state
{
	filter = min_mag_mip_point;
	AddressU = wrap;
	AddressV = wrap;
};

sampler LinearSampler = sampler_state
{
	filter = min_mag_mip_linear;
	AddressU = wrap;
	AddressV = wrap;
};

/* For.Rasterizer State  */
RasterizerState RS_Default
{
	FillMode = solid;
};

RasterizerState RS_Wireframe
{
	FillMode = wireframe;
};

/* For.DepthStencil State  */
DepthStencilState DSS_Default
{
	DepthEnable = true;
	DepthWriteMask = all;
	DepthFunc = less_equal;
};

DepthStencilState DSS_Not_ZTest_Not_ZWrite
{
	DepthEnable = false;
	DepthWriteMask = zero;	
};

/* For.Blend State  */
BlendState BS_Default
{
	BlendEnable[0] = false;
};

BlendState BS_AlphaBlend
{
	BlendEnable[0] = true;
	BlendEnable[1] = true;
	
	SrcBlend = src_alpha;
	DestBlend = inv_src_alpha;
	BlendOp = add;
};
