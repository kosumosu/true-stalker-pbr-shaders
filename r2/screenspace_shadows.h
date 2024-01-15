// Screen space shadows
// Author: Ascii1457

#include "screenspace_common.h"
#include "settings_screenspace_SSS.h"

#ifdef USE_SSS
float SSFX_ScreenSpaceShadows(float4 P, float2 tc)
{
	// Light vector
	float3 L_dir = mul(m_V, float4(-normalize(L_sun_dir_w), 0)).xyz;

	float result = 1.0f;

	// Weapons mask. Depth below or equal to 1.2
	// Make screen space shadows on hud only
	if (P.z < 1.2f)
	{
		// Weapon Factor.
		float pLen = length(P.z);
		float wpn_f = smoothstep(G_SSDO_WEAPON_LENGTH * 0.75f, G_SSDO_WEAPON_LENGTH, pLen);

		// Adjust Settings for weapons and scene.
		float ray_hardness	= lerp(10.0f * G_SSDO_WEAPON_HARDNESS, 15.0f * G_SSDO_SCENARY_HARDNESS, wpn_f);
		float ray_detail	= clamp(pLen * 0.003f, G_SSS_DETAILS, 10);
		float ray_thick		= (0.3f - wpn_f * 0.23f) + (pLen * 0.001f * wpn_f);
		float ray_len		= lerp(G_SSDO_WEAPON_SHADOW_LENGTH, G_SSDO_SCENARY_SHADOW_LENGTH, wpn_f);

		if (ray_hardness > 0)
		{
			RayTrace sss_ray = SSFX_ray_init(P, L_dir, ray_len, G_SSS_STEPS, SSFX_noise(tc));

			float fade_len = 1.0f + sss_ray.r_step;
			float shadow = 0;
	
			for (int i = 0; i < G_SSS_STEPS; i++)
			{
				// Check the march if ray is in screen...
				if (SSFX_is_valid_uv(sss_ray.r_pos))
				{
					// Sample current ray pos ( x = difference | y = sample depth | z = current ray len )
					float3 depth_ray = SSFX_ray_intersect(sss_ray);
	
					// Check depth difference
					float diff = depth_ray.x;

					// No Sky
					#define SKY_EPS float(0.001)
					diff *= depth_ray.y > SKY_EPS;

					// Disable weapons at some point to avoid wrong shadows on the ground
					if (P.z >= 1.2f)
						diff *= depth_ray.y >= 1.0f;
	
					// Negative: Ray is closer to the camera ( not occluded )
					// Positive: Ray is beyond the depth sample ( occluded )
					if (diff > ray_detail && diff < ray_thick)
					{
						shadow += (fade_len - depth_ray.z) * (1.0f - smoothstep( G_SSS_STEPS * G_SSS_FORCE_FADE, G_SSS_STEPS + 1, i));
					}

					// Step the ray
					sss_ray.r_pos += sss_ray.r_step;
				}
			}
	
			// Calc shadow and return.
			result = 1.0f - saturate(shadow * (1.0f / G_SSS_STEPS) * ray_hardness) * G_SSS_INTENSITY;
		}
	}

	return result;
}
#else // USE_SSS
float SSFX_ScreenSpaceShadows(float4 P, float2 tc)
{
	return 1.0f;
}
#endif