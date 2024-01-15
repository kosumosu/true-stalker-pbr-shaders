#include "common.h"

struct v2p_flare
{
	float2 Tex0		: TEXCOORD0;
	float4 tctexgen	: TEXCOORD1;
	float4 Color	: COLOR;
	float4 HPos		: SV_Position;	// Clip-space position 	(for rasterization)
};

uniform float4x4	mVPTexgen;
//////////////////////////////////////////////////////////////////////////////////////////
// Vertex
v2p_flare main ( v_TL I )
{
	v2p_flare	O;

	O.HPos				= mul(m_VP, I.P);						// xform, input in world coords
	O.Tex0				= I.Tex0;								// copy tc
	O.Color				= unpack_D3DCOLOR(I.Color);				// copy color
	O.tctexgen			= mul(mVPTexgen, I.P);
	O.tctexgen.z		= O.HPos.z;
	return	O;
}