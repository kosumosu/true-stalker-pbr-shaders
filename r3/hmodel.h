#ifndef HMODEL_H
#define HMODEL_H

#include "common.h"
#include "lmodel.h"

TextureCube		env_s0;
TextureCube		env_s1;
TextureCube		sky_s0;
TextureCube		sky_s1;

uniform float4	env_color;        // color.w  = lerp factor
uniform float3x4	m_v2w;

void hmodel_pbr
(
	out float3 hdiffuse, out float3 hspecular, 
	float m, float h, float s, float3 Pnt, float3 normal
)
{
	// [ SSS Test ]. Overwrite terrain material
	bool m_terrain = abs(m - MAT_TERRAIN) <= 0.04f;
	bool m_flora = abs(m - MAT_FLORA) <= 0.04f;
	if (m_terrain)
		m = 0;

        // hscale - something like diffuse reflection
	float3	nw		= mul( m_v2w, normal );
	float	hscale	= h;	//. *        (.5h + .5h*nw.y);

#ifdef         USE_GAMMA_22
			hscale	= (hscale*hscale);        // make it more linear
#endif

	// reflection vector
	float3	v2PntL	= normalize( Pnt );
	float3	v2Pnt	= mul( m_v2w, v2PntL );
	float3	vreflect= reflect( v2Pnt, nw );
	//float	hspec	= .5h + .5h * dot( vreflect, v2Pnt );
	float	hspec	= vreflect.z; // == dot(vreflect, up)

	// material	// sample material
	float4	light_	= s_material.SampleLevel( smp_material, float3( hscale, hspec, m ), 0 ).xxxy;
	float4 light = float4(hscale.xxx, light_.y * 0.5);

	// diffuse color
	float3	e0d		= env_s0.SampleLevel( smp_rtlinear, nw, 0 );
	float3	e1d		= env_s1.SampleLevel( smp_rtlinear, nw, 0 );
	float3	env_d	= env_color.xyz * lerp( e0d, e1d, env_color.w );
			env_d	*=env_d;	// contrast
			env_d	*=env_d;	// contrast
			hdiffuse= env_d * light.xyz * light.xyz + L_ambient.rgb;

	// specular color
	vreflect.y      = vreflect.y*2-1;	// fake remapping
	float3	e0s		= env_s0.SampleLevel( smp_rtlinear, vreflect, 0 );
	float3	e1s		= env_s1.SampleLevel( smp_rtlinear, vreflect, 0 );
	float3	env_s	= env_color.xyz * lerp( e0s, e1s, env_color.w);
			env_s	*=env_s;	// contrast
			env_s	*=env_s;	// contrast
		hspecular	= env_s*light.w * light.w*s*s;                //*h*m*s        ;        //env_s        *light.w         * s;
}

void hmodel
(
	out float3 hdiffuse, out float3 hspecular, 
	float m, float h, float s, float3 Pnt, float3 normal
)
{
	// [ SSS Test ]. Overwrite terrain material
	bool m_terrain = abs(m - MAT_TERRAIN) <= 0.04f;
	bool m_flora = abs(m - MAT_FLORA) <= 0.04f;
	if (m_terrain)
		m = 0;

        // hscale - something like diffuse reflection
	float3	nw		= mul( m_v2w, normal );
	float	hscale	= h;	//. *        (.5h + .5h*nw.y);

#ifdef         USE_GAMMA_22
			hscale	= (hscale*hscale);        // make it more linear
#endif

	// reflection vector
	float3	v2PntL	= normalize( Pnt );
	float3	v2Pnt	= mul( m_v2w, v2PntL );
	float3	vreflect= reflect( v2Pnt, nw );
	float	hspec	= .5h + .5h * dot( vreflect, v2Pnt );

	// material	// sample material
	float4	light	= s_material.SampleLevel( smp_material, float3( hscale, hspec, m ), 0 ).xxxy;

	// diffuse color
	float3	e0d		= env_s0.SampleLevel( smp_rtlinear, nw, 0 );
	float3	e1d		= env_s1.SampleLevel( smp_rtlinear, nw, 0 );
	float3	env_d	= env_color.xyz * lerp( e0d, e1d, env_color.w );
			env_d	*=env_d;	// contrast
			hdiffuse= env_d * light.xyz * light.xyz + L_ambient.rgb * L_ambient.rgb;

	// specular color
	vreflect.y      = vreflect.y*2-1;	// fake remapping
	float3	e0s		= env_s0.SampleLevel( smp_rtlinear, vreflect, 0 );
	float3	e1s		= env_s1.SampleLevel( smp_rtlinear, vreflect, 0 );
	float3	env_s	= env_color.xyz * lerp( e0s, e1s, env_color.w);
			env_s	*=env_s;	// contrast
			env_s	*=env_s;	// contrast
		hspecular	= env_s*light.w * light.w*s*s;                //*h*m*s        ;        //env_s        *light.w         * s;
}
#endif