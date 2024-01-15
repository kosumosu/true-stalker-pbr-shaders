#ifndef sslr_h_included
#define sslr_h_included

/*
Implementation of local reflections in the screen space with a dynamic step of tracing.
Materials used:
https://www.amk-team.ru/forum/topic/14078-sslr/
https://www.amk-team.ru/forum/topic/14229-narodnaya-solyanka-2016-ogsr-engine/?do=findComment&comment=1335750
And own developments, especially for the modification of True Stalker.
by Hozar_2002
All copyrights belong to their rightful owners.
*/
#if SSLR_RT_MODE == 3
#define MaxSamples int(125)
#elif SSLR_RT_MODE == 2
#define MaxSamples int(25)
#else
#define MaxSamples int(15)
#endif

uniform float4 screen_res;
uniform float4 L_sky_color;

#ifdef USE_DX9_LOAD
uniform float2 pos_decompression_params;
uniform samplerCUBE s_env0;
uniform samplerCUBE s_env1;
#else
TextureCube	s_env0;
TextureCube	s_env1;
#endif

float GetBorderAtten(float2 tc, float att)
{
	float att_h = att * screen_res.x * screen_res.w;
	float borderDistX = min(1.f-tc.x, tc.x);
	float borderDistY = min(1.f-tc.y, tc.y);
	float borderAttenX = borderDistX > att ? 1.f : borderDistX / att;  
	float borderAttenY = borderDistY > att_h ? 1.f : borderDistY / att_h;  
	return min(borderAttenX, borderAttenY);
}

float2 position_get_tc (float3 P)
{
	P.xy /= P.z * pos_decompression_params.xy;
	
	return P.xy * 0.5f + 0.5f;
}

float3 tc_get_position (float2 tc, float depth)
{
	
	float3 P = depth;
	tc = tc * 2.f - 1.f;
	P = float3( P.z * ( tc * pos_decompression_params.xy), P.z);

	return P;
}

float4 gbuffer_load_position (float2 tc, float last)
{
	
	#ifdef USE_DX9_LOAD
		float3 P = tex2Dlod (s_position, float3(tc, 0.f).xyzz);
	#else
			#ifndef USE_MSAA
				float3 P	= s_position.SampleLevel(smp_nofilter, tc, 0);
			#else
				float3 P	= s_position.Load(int3(tc * pos_decompression_params2.xy, 0), 0);
			#endif
	#endif
	
	float depth = P.z;
	if(P.z < 0.01f) P.z = last;
	tc = tc * 2.f - 1.f;
	P = float3(tc * pos_decompression_params.xy, 1.f) * P.z;

	return float4(P, depth);
}

#ifndef SSLR_RT_MODE

float4 ScreenSpaceLocalReflections (float3 vspos, float3 vsnorm)
{
	return 0;
}

#elif SSLR_RT_MODE == 1

float4 ScreenSpaceLocalReflections (float3 vspos, float3 vsnorm) //Initial beam length
{
	float3 refl_vec = normalize(reflect(normalize(vspos), normalize(vsnorm)));
	float2 reflPos = saturate(position_get_tc(refl_vec)); // We get the pixel position in Screen Space of the reflected geometry
	
	float canrefl = floor(sign(refl_vec.z) * 0.5h + 0.5h);
	
	float4 hit_pos = gbuffer_load_position(reflPos, 1e3);
		 
	canrefl *= smoothstep(length(vspos), length(vspos) * 1.15h, length(hit_pos));
	
	float borderAtten = GetBorderAtten(reflPos, 0.035f); // Grease the edges of the reflections for a smoother transition
	
	float3 color;
	
	#ifdef USE_DX9_LOAD
		color = tex2D(s_image, reflPos);
	#else
		color = s_image.Sample(smp_nofilter, reflPos);
	#endif
	
	return float4(color, borderAtten * canrefl);
}

#else
	
float4 ScreenSpaceLocalReflections (float3 vspos, float3 vsnorm)
{
	float3 refl_vec = normalize(reflect(normalize(vspos), normalize(vsnorm)));
	float2 reflPos = 0.f; //Coordinates of the Screen Space of the reflected geometry
	float L = 1.f / float(MaxSamples); //Initial beam length
	#ifdef USE_LOD_COLOR
	vspos += vsnorm * def_virtualh;
	#endif
	float isnsky = 1.f;
	
	float canrefl = saturate(dot(normalize(vspos), refl_vec) * 1.5f);
	
	float4 hit_pos;
	float3 new_pos;
	
	#ifdef USE_DX9_LOAD
		int i = 0; while(i < MaxSamples)
	#else
		for(int i = 0; i < MaxSamples; i ++)
	#endif
		{
			new_pos = vspos.xyz + refl_vec * L;
		  
			reflPos = position_get_tc(new_pos); // We get the pixel position in Screen Space
			{
				if((i + 1) < MaxSamples) // "=" not always plows well, as it seems to me
				{
					hit_pos = gbuffer_load_position(reflPos, hit_pos.z);
					#if SSLR_RT_MODE == 3
					if(i < 100)
						L += 0.06;
					else
					#endif
					L = length(hit_pos.xyz - vspos.xyz);
					isnsky = (hit_pos.w > 0.01f) ? 1.f : 0.f;
				}
			}
			#if SSLR_RT_MODE == 3
			if(abs(new_pos.z - hit_pos.z) < 0.05)
				i = 100;
			#endif
			#if defined(USE_LOD_COLOR) || (SSLR_RT_MODE == 3)
			bool outside = (length(vspos) - length(hit_pos)) > 0.05f;
			#else
			bool outside = (length(new_pos) - length(hit_pos)) > 0.05f;
			#endif
			bool exclude = min(min(reflPos.x, reflPos.y), 1.f - max(reflPos.x, reflPos.y)) <= 0.f;
			if(exclude || outside)
			{
				canrefl *= 0.f; // We prohibit reflection and stop tracing
			#ifdef USE_DX9_LOAD
				i = MaxSamples; // break;
			#else
				break;			
			#endif
			}
			#ifdef USE_DX9_LOAD
			i ++;
			#endif
		}
		 
	float borderAtten = GetBorderAtten(reflPos, 0.025f); // Grease the edges of the reflections for a smoother transition
	
	float fog = saturate (length(hit_pos) * fog_params.w + fog_params.x);
	float skyblend = 1.f - saturate (fog*fog);
	
	float3 color;
	
	#ifdef USE_DX9_LOAD
		color = tex2D(s_image, reflPos);
	#else
		color = s_image.Sample(smp_nofilter, reflPos);
	#endif
	
	return float4(color, borderAtten * isnsky * canrefl * skyblend);
}
#endif
	
float3 water_sunlight (float3 pnt, float3 normal)
{
    return L_sun_color.rgb * pow(abs(dot(normalize(pnt + L_sun_dir_w), normalize(normal))), 256.h) * 4.h;
}
	
// stalker_Utils.h - https://github.com/Xerxes1138/X-Ray-PBR
float3 RotateAroundYAxis(float3 v, float alpha)
{
        float sina, cosa;
        sincos(alpha, sina, cosa);
        float2x2 m = float2x2(cosa, -sina, sina, cosa);
        return normalize(float3(mul(m, v.xz), v.y).xzy);
}
	
float3 calc_envmap(float3 vreflect)
{
	vreflect = RotateAroundYAxis(vreflect, L_sky_color.w);
	
	float3 vreflectabs = abs(vreflect);
	float vreflectmax = max(vreflectabs.x, max(vreflectabs.y, vreflectabs.z));
	vreflect /= vreflectmax;
	
	if (vreflect.y<0.999)
		vreflect.y= vreflect.y * 2 - 1; 
	
	#ifdef USE_DX9_LOAD
		float3	env0	= texCUBElod (s_env0, float4(vreflect.xyz, 0)).xyz;
		float3	env1	= texCUBElod (s_env1, float4(vreflect.xyz, 0)).xyz;
	#else
		float3	env0	= s_env0.SampleLevel( smp_base, vreflect.xyz, 0).xyz;
		float3	env1	= s_env1.SampleLevel( smp_base, vreflect.xyz, 0).xyz;
	#endif
	//TODO: should be rotated along the Y axis, based on the weather config
	float3 color = lerp (env0,env1,L_ambient.w) * 0.66h * L_sky_color.xyz;
	
	#ifdef USE_DX9_LOAD
	float 	fWhiteIntensitySQR = 2.89;
	color	*= tex2Dlod (s_tonemap, float(0.f).xxxx).x;
	color	*= (1.0 + color/fWhiteIntensitySQR) / (color + 1.0);
	#else
	color	*= s_tonemap.Load( int3(0,0,0) ).x;
	#endif
	
	return color;
}
#endif

