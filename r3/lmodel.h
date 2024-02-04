#ifndef	LMODEL_H
#define LMODEL_H

#include "common.h"

struct light_t
{
	float3 diffuse;
	float3 specular;
};

// Simple subsurface scattering
float3 SSS_orig(float3 N, float3 V, float3 L)
{
	float S = saturate(dot(V, -(L + N))) * ssfx_florafixes_2.x;
	return S * lerp(float3(1.0f, 1.0f, 1.0f), display_to_working_space(L_sun_color.rgb), ssfx_florafixes_2.y);
}

float3 SSS(float3 N, float3 V, float3 L)
{
	return saturate(dot(V, L - N));
	//return saturate(dot(N, L));
}

//////////////////////////////////////////////////////////////////////////////////////////
// Lighting formulas			// 
light_t plight_infinity( float m, float3 pnt, float3 normal, float4 c_tex, float3 light_direction )
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
	light = display_to_working_space(light);

#ifdef SSFX_FLORAFIX
	if (m_flora) //Be aware of precision loss/errors
	{
		//Simple subsurface scattering
		float3 subsurface = SSS(N,V,L);
		light.rgb += subsurface*c_tex.rgb*0.3f;
	}
#endif

	light_t result;
	result.diffuse = light.rgb;
	result.specular = light.a;

	return result;
}

light_t plight_local( float m, float3 pnt, float3 normal, float3 light_position, float light_range_rsq, out float rsqr )
{
  	float3 N		= normal;							// normal 
  	float3 L2P 	= pnt-light_position;                         		// light2point 
  	float3 V 	= -normalize	(pnt);					// vector2eye
  	float3 L 	= -normalize	((float3)L2P);					// vector2light
  	float3 H		= normalize	(L+V);						// float-angle-vector
		rsqr	= dot		(L2P,L2P);					// distance 2 light (squared)
  	float  att 	= saturate	(1 - rsqr*light_range_rsq);			// q-linear attenuate
	float4 light	= s_material.Sample( smp_material, float3( dot(L,N), dot(H,N), m ) ).xxxy;		// sample material

	float4 res = display_to_working_space(att * light);

	light_t result;
	result.diffuse = res.rgb;
	result.specular = res.a;

	return result;
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


#define PI 3.14159265

#define USE_PBR 1

float3 orenNayarDiffuse(
	float3 lightDirection,
	float3 viewDirection,
	float3 surfaceNormal,
	float roughness,
	float3 albedo) {

	float LdotV = dot(lightDirection, viewDirection);
	float NdotL = dot(lightDirection, surfaceNormal);
	float NdotV = dot(surfaceNormal, viewDirection);

	float s = LdotV - NdotL * NdotV;
	float t = lerp(1.0, max(NdotL, NdotV), step(0.0, s));

	float sigma2 = roughness * roughness;
	float3 A = 1.0.xxx + sigma2 * (albedo / (sigma2 + 0.13) + 0.5.xxx / (sigma2 + 0.33));
	float B = 0.45 * sigma2 / (sigma2 + 0.09);

	return /*albedo * */ max(0.0, NdotL) * (A + B.xxx * s / t) / PI; // albedo will be accounted for later
}

float3 fresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - saturate(cosTheta), 5.0);
}

float DistributionGGX(float NdotH, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
	NdotH = max(NdotH, 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}

float GeometrySmith(float NdotV, float NdotL, float roughness)
{
	NdotV = max(NdotV, 0.0);
	NdotL = max(NdotL, 0.0);

    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

float3 torranceCook(
	float3 lightDirection,
	float3 viewDirection,
	float3 surfaceNormal,
	float roughness,
	float3 albedo,
	float metallic) {

	float3 halfVector = normalize(viewDirection + lightDirection);

	//float LdotV = dot(lightDirection, viewDirection);
	float NdotL = dot(lightDirection, surfaceNormal);
	float NdotV = dot(surfaceNormal, viewDirection);

	float HdotV = dot(halfVector, viewDirection);
	float NdotH = dot(surfaceNormal, halfVector);

	float3 F0 = 0.04.xxx;
	F0 = lerp(F0, albedo, metallic);
	float3 F  = fresnelSchlick(max(HdotV, 0.0), F0);

	float NDF = DistributionGGX(NdotH, roughness);
	float G = GeometrySmith(NdotV, NdotL, roughness);

	float3 numerator = NDF * G * F;
	float denominator = 4.0 * max(NdotV, 0.0) * max(NdotL, 0.0) + 0.001;
	float3 specular = numerator / denominator;

    return (/*kD * albedo / PI +*/ specular / max(albedo, 0.01.xxx)) * max(NdotL, 0.0);
}


light_t light_pbr( float m, float3 albedo, float gloss, float final_gloss_multiplier, float3 diffuseLightDirection, float3 specularLightDirection, float3 viewDirection, float3 normal)
{
	light_t result;

	float orenNayarRoughness;
	float torranceCookRoughness;
	float torranceCookMetalness;
	float actual_gloss;

	float material = m * 3;
	float materialBlend = material - floor(material);

	float NdotL_diffuse = max(0.0, dot(diffuseLightDirection, normal));
	float NdotV = max(0.0, dot(viewDirection, normal));
	float normal_reflectance = 0.04;
	float fresnel_factor = fresnelSchlick(NdotL_diffuse, normal_reflectance).x * fresnelSchlick(NdotV, normal_reflectance).x;

	if (material >= 0.0 && material < 1.0) { // OrenNayar - Blinn
		orenNayarRoughness = lerp (0.5, 0.1, materialBlend);
		torranceCookRoughness = lerp(1, 0.6, gloss) * lerp(1, 0.7, materialBlend);
		torranceCookMetalness = 0;
		actual_gloss = lerp(0.2, 1.0, gloss);

		// result.diffuse = 0.0.xxx;
		// result.specular = 0.0.xxx;
		// return result;

		//return float4(0.1, 1, 0.1, 0);
	} else if (material < 2.0) {  // Blinn - Phong
		orenNayarRoughness = lerp (0.2, 0.0, materialBlend);
		torranceCookRoughness = saturate(0.9 - gloss * 0.7) * lerp(0.8, 0.4, materialBlend);
		torranceCookMetalness = 0;
		actual_gloss = lerp(0.1, 1.0, gloss);

		// result.diffuse = 0.0.xxx;
		// result.specular = 0.0.xxx;
		// return result;

	} else if (material < 3.0) {  // Phong-Metal
		orenNayarRoughness = lerp (0.2, 0.0, materialBlend);
		torranceCookRoughness = lerp(saturate(1.0 - gloss * 0.9) * 0.4, 0.3, materialBlend);
		torranceCookMetalness = materialBlend * gloss;
		actual_gloss = lerp(0.2, 1.0, gloss);

		// result.diffuse = 0.0.xxx;
		// result.specular = 0.0.xxx;
		// return result;

	} else if (material < 4.0) {  // Metal-OrenNayar
		orenNayarRoughness = lerp (0.0, 0.75, materialBlend);
		torranceCookRoughness = lerp(0.3, saturate(1.0 - gloss * 0.9) * 0.6, materialBlend);
		torranceCookMetalness = (1.0 - materialBlend) * gloss;
		actual_gloss = lerp(0.2, 1.0, gloss);

		fresnel_factor = 1;

		// orenNayarRoughness = 0.3;
		// torranceCookRoughness = 0.25;
		// torranceCookMetalness = 1.0;
		// gloss = 0.5 + gloss * 0.5;
		// gloss = 1;

	} else {
		// Wierd vivid color
		result.diffuse = float3(1, 0, 1);
		result.specular = 0.0.xxx;
		return result;
	}

	float3 diffuse = orenNayarDiffuse(diffuseLightDirection, viewDirection, normal, orenNayarRoughness, albedo);
	float3 specular = torranceCook(specularLightDirection, viewDirection, normal, torranceCookRoughness, 0.8.xxx, torranceCookMetalness); 
	//float3 specular = 8 * pow(max(0.0, dot(normalize(viewDirection + specularLightDirection), normal)), 64.0);

	actual_gloss *= final_gloss_multiplier;

	result.diffuse = diffuse * saturate(1.0 - actual_gloss * fresnel_factor);
	//result.diffuse = dot(diffuseLightDirection, normal);
	result.specular = specular * actual_gloss;
	return result;
}

light_t light_pbr( float m, float3 albedo, float gloss, float final_gloss_multiplier, float3 lightDirection, float3 viewDirection, float3 normal)
{
	return light_pbr(m, albedo, gloss, final_gloss_multiplier, lightDirection, lightDirection, viewDirection, normal);
}

#define SSFX_FLORAFIX

light_t plight_infinity_pbr( float m, float3 albedo, float gloss, float3 pnt, float3 normal, float3 lightDirection )
{
	// [ SSS Test ]. Overwrite terrain material
	bool m_terrain = abs(m - MAT_TERRAIN) <= MAT_FLORA_ELIPSON;
	bool m_flora = abs(m - MAT_FLORA) <= MAT_FLORA_ELIPSON;
	if (m_terrain) {
	 	m = 0.0;
		gloss = saturate(gloss * 4);

		// light_t lt;
		// lt.diffuse = float3(10,2,2);
		// lt.specular = 0.0.xxx;
		// return lt;
	}

	float final_gloss_multiplier = 1.0;
	if (m_flora) {
		gloss = 0.0;
		final_gloss_multiplier = 0.4;
	}

	float3 viewDirection = -normalize(pnt);
	light_t light = light_pbr(m, albedo, gloss, final_gloss_multiplier, -lightDirection, viewDirection, normal);  // no albedo (s_diffuse) in True Stalker is available here
	//light_t light = light_pbr(m, 0.6.xxx, gloss, -lightDirection, viewDirection, normal);
	//light_t light = light_pbr(m, 0.6.xxx, 0.8, -lightDirection, viewDirection, normal);

#ifdef SSFX_FLORAFIX
	if (m_flora) //Be aware of precision loss/errors
	{
		//light.diffuse.rgb *= 0.5;
		//Simple subsurface scattering
		float3 subsurface = SSS(normal, viewDirection, lightDirection);
		light.diffuse.rgb += subsurface*0.4;
	}
#endif

	return light;
}

light_t plight_local_pbr( float m, float3 albedo, float gloss, float3 pnt, float3 normal, float3 light_position, float light_range_rsq, out float rsqr )
{
	bool m_terrain = abs(m - MAT_TERRAIN) <= MAT_FLORA_ELIPSON;
	bool m_flora = abs(m - MAT_FLORA) <= MAT_FLORA_ELIPSON;
	if (m_terrain) {
		m = 0.0;
		gloss = saturate(gloss * 4);
	}

	float final_gloss_multiplier = 1.0;
	if (m_flora) {
		gloss = 0.0;
		final_gloss_multiplier = 0.4;
	}
	

	float3 pointToLight = light_position - pnt;
	float3 lightDirection = normalize(pointToLight);
	float3 viewDirection = -normalize(pnt);

	light_t light = light_pbr(m, albedo, gloss, final_gloss_multiplier, lightDirection, viewDirection, normal);  // no albedo (s_diffuse) in True Stalker is available here
	//light_t light = light_pbr(m, 0.6.xxx, gloss, lightDirection, viewDirection, normal); 
	rsqr = dot (pointToLight, pointToLight);
	float  att 	= display_to_working_space(saturate	(1 - rsqr*light_range_rsq));			// q-linear attenuate
	//float att = 6.0 / rsqr;

#ifdef SSFX_FLORAFIX
	if (m_flora) //Be aware of precision loss/errors
	{
		//light.diffuse.rgb *= 0.5;
		//Simple subsurface scattering
		float3 subsurface = SSS(normal, viewDirection, lightDirection);
		light.diffuse.rgb += subsurface*0.4;
	}
#endif

	light.diffuse *= att;
	light.specular *= att;

	return light;
}


#endif