#ifndef	JITTER_H_INCLUDED
#define	JITTER_H_INCLUDED

#include "gamma.h"

#define JITTER_TEXTURE_SIZE	64.0f

sampler		smp_jitter;
Texture2D	jitter0;

float nrand(float2 uv)
{
    return frac(sin(dot(uv, float2(12.9898, 78.233))) * 43758.5453) * 2.0 - 1.0;
}

float3 random_color(float2 uv) {
	float n1 = nrand(uv);
	float n2 = nrand(uv * 1.163547 + float2(0.1, 0.3) + float2(n1, 0));
	float n3 = nrand(uv * 1.3254481 + float2(n1, n2));
	return float3(n1, n2, n3);
}

float4 sample_jitter(float2 tc) {
    return jitter0.Sample( smp_jitter, tc * screen_res * 1.f / JITTER_TEXTURE_SIZE );
}

float3 sample_intermediate_dither(float2 tc) {
	//return 0.0.xxx;
	return sample_jitter(tc) * intermediate_half_quantum;
	//return sample_jitter(tc) * 0.5 / 255.0;
    //return /*sample_jitter(tc)*/ random_color(tc) * 0.5 / 255.0;
}

#endif