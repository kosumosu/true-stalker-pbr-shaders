#ifndef	LMODEL_H
#define LMODEL_H

#include "common.h"

// Simple subsurface scattering
float3 SSS(float3 N, float3 V, float3 L)
{
	float S = saturate(dot(V, -(L + N))) * ssfx_florafixes_2.x;
	return S * lerp(float3(1.0f, 1.0f, 1.0f), L_sun_color.rgb, ssfx_florafixes_2.y);
}

//////////////////////////////////////////////////////////////////////////////////////////
// Lighting formulas			// 
float4 plight_infinity( float m, float3 pnt, float3 normal, float4 c_tex, float3 light_direction )
{
	// [ SSS Test ]. Overwrite terrain material
	bool m_terrain = abs(m - MAT_TERRAIN) <= MAT_FLORA_ELIPSON;
	bool m_flora = abs(m - MAT_FLORA) <= MAT_FLORA_ELIPSON;
	if (m_terrain)
		m = 0;

  	float3 N			= normal;							// normal 
  	float3 V 		= -normalize	(pnt);					// vector2eye
  	float3 L 		= -light_direction;						// vector2light
  	float3 H			= normalize	(L+V);						// float-angle-vector 
  	float4 light			= s_material.Sample( smp_material, float3( dot(L,N), dot(H,N), m ) ).xxxy;		// sample material

#ifdef SSFX_FLORAFIX
	if (m_flora) //Be aware of precision loss/errors
	{
		//Simple subsurface scattering
		float3 subsurface = SSS(N,V,L);
		light.rgb += subsurface*c_tex.rgb*0.3f;
	}
#endif

	return light;
}

float4 plight_local( float m, float3 pnt, float3 normal, float3 light_position, float light_range_rsq, out float rsqr )
{
  	float3 N		= normal;							// normal 
  	float3 L2P 	= pnt-light_position;                         		// light2point 
  	float3 V 	= -normalize	(pnt);					// vector2eye
  	float3 L 	= -normalize	((float3)L2P);					// vector2light
  	float3 H		= normalize	(L+V);						// float-angle-vector
		rsqr	= dot		(L2P,L2P);					// distance 2 light (squared)
  	float  att 	= saturate	(1 - rsqr*light_range_rsq);			// q-linear attenuate
	float4 light	= s_material.Sample( smp_material, float3( dot(L,N), dot(H,N), m ) ).xxxy;		// sample material
  	return att*att*light*light;
}

//	TODO: DX10: Remove path without blending
float4 blendp( float4 value, float4 tcp)
{
	return 	value;
}

float4 blend( float4 value, float2 tc)
{
	return 	value;
}

#endif