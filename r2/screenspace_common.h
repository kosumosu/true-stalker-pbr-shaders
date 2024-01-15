// Screen space shadows
// Author: Ascii1457

#define SSFX_READY

#include "common.h"
#include "lmodel.h"
#include "hmodel.h"

#include "screenspace_common_noise.h"

uniform float4 screen_res;

struct RayTrace
{
	float2 r_pos;
	float2 r_step;
	float2 r_start;
	float r_length;
	float z_start;
	float z_end;
};

bool SSFX_is_saturated(float2 value) { return (value.x == saturate(value.x) && value.y == saturate(value.y)); }

bool SSFX_is_valid_uv(float2 value) { return (value.x >= 0.0f && value.x <= 1.0f) && (value.y >= 0.0f && value.y <= 1.0f); }

float2 SSFX_view_to_uv(float3 Pos)
{
	float4 tc = mul(m_P, float4(Pos, 1));
	return (tc.xy / tc.w) * float2(0.5f, -0.5f) + 0.5f;
}

float SSFX_get_depth(float2 tc)
{
	return tex2D(s_position, tc).z;
}

RayTrace SSFX_ray_init(float3 ray_start_vs, float3 ray_dir_vs, float ray_max_dist, int ray_steps, float noise)
{
	RayTrace rt;
	
	float3 ray_end_vs = ray_start_vs + ray_dir_vs * ray_max_dist;
	
	// Ray depth ( from start and end point )
	rt.z_start		= ray_start_vs.z;
	rt.z_end		= ray_end_vs.z;

	// Compute ray start and end (in UV space)
	rt.r_start		= SSFX_view_to_uv(ray_start_vs);
	float2 ray_end	= SSFX_view_to_uv(ray_end_vs);

	// Compute ray step
	float2 ray_diff	= ray_end - rt.r_start;
	rt.r_step		= ray_diff / (float)ray_steps; 
	rt.r_length		= length(ray_diff);
	
	rt.r_pos		= rt.r_start + rt.r_step * noise;
	
	return rt;
}

float3 SSFX_ray_intersect(RayTrace Ray)
{
	float len = length(Ray.r_pos - Ray.r_start);
	float alpha = len / Ray.r_length;
	float depth_ray = (Ray.z_start * Ray.z_end) / lerp(Ray.z_end, Ray.z_start, alpha);
	float depth_scene = SSFX_get_depth(Ray.r_pos);
	
	return float3(depth_ray.x - depth_scene, depth_scene, len);
}
