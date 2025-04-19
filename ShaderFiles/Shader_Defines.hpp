

/* For.SamplerState */
sampler LinearSampler = sampler_state
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = wrap;
	AddressV = wrap;
};

sampler PointSampler = sampler_state
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = wrap;
	AddressV = wrap;
};

/* For.RaterizerState */
RasterizerState	RS_Default
{
	FillMode = solid;
	CullMode = back;
	FrontCounterClockwise = false;
};

RasterizerState	RS_Wireframe
{
	FillMode = Wireframe;
	CullMode = back;
	FrontCounterClockwise = false;
};

RasterizerState	RS_Sky
{
	FillMode = solid;
	CullMode = front;
	FrontCounterClockwise = false;
};

/* For.BlendState */
BlendState BS_Default
{
	BlendEnable[0] = false;
};

BlendState BS_RGBBlend
{
	BlendEnable[0] = true;
	BlendEnable[1] = true;

	SrcBlend = one;
	DestBlend = one;
	BlendOp = Add;
};

BlendState BS_AlphaBlend
{
	BlendEnable[0] = true;
	SrcBlend = Src_Alpha;
	DestBlend = Inv_Src_Alpha;
	BlendOp = Add;
};

/* For.DepthStencilState */
DepthStencilState DSS_Default
{
	DepthEnable = true;
	DepthWriteMask = all;
	DepthFunc = less_equal;
};

DepthStencilState DSS_Sky
{
	DepthEnable = false;
	DepthWriteMask = zero;
};

