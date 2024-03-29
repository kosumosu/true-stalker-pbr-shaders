#ifndef	JITTER_H_INCLUDED
#define	JITTER_H_INCLUDED

#include "gamma.h"

#define JITTER_TEXTURE_SIZE	64.0f

sampler		smp_jitter;
Texture2D	jitter0;

uint hash( uint x ) {
    x += ( x << 10u );
    x ^= ( x >>  6u );
    x += ( x <<  3u );
    x ^= ( x >> 11u );
    x += ( x << 15u );
    return x;
}

// Compound versions of the hashing algorithm I whipped together.
uint hash( uint2 v ) { return hash( v.x ^ hash(v.y)                         ); }
uint hash( uint3 v ) { return hash( v.x ^ hash(v.y) ^ hash(v.z)             ); }
uint hash( uint4 v ) { return hash( v.x ^ hash(v.y) ^ hash(v.z) ^ hash(v.w) ); }

////

uint xxhash32(const uint p)
{
	const uint PRIME32_2 = 2246822519U, PRIME32_3 = 3266489917U;
	const uint PRIME32_4 = 668265263U, PRIME32_5 = 374761393U;
	uint h32 = p + PRIME32_5;
	h32 = PRIME32_4 * ((h32 << 17) | (h32 >> (32 - 17)));
	h32 = PRIME32_2 * (h32 ^ (h32 >> 15));
	h32 = PRIME32_3 * (h32 ^ (h32 >> 13));
	return h32 ^ (h32 >> 16);
}

uint xxhash32(const uint2 vec)
{
	const uint PRIME32_2 = 2246822519U, PRIME32_3 = 3266489917U;
	const uint PRIME32_4 = 668265263U, PRIME32_5 = 374761393U;
	uint h32 = vec.y + PRIME32_5 + vec.x * PRIME32_3;
	h32 = PRIME32_4 * ((h32 << 17) | (h32 >> (32 - 17)));
	h32 = PRIME32_2 * (h32 ^ (h32 >> 15));
	h32 = PRIME32_3 * (h32 ^ (h32 >> 13));
	return h32 ^ (h32 >> 16);
}

uint xxhash32(const uint3 vec)
{
	const uint PRIME32_2 = 2246822519U, PRIME32_3 = 3266489917U;
	const uint PRIME32_4 = 668265263U, PRIME32_5 = 374761393U;
	uint h32 = vec.z + PRIME32_5 + vec.x * PRIME32_3;
	h32 = PRIME32_4 * ((h32 << 17) | (h32 >> (32 - 17)));
	h32 += vec.y * PRIME32_3;
	h32 = PRIME32_4 * ((h32 << 17) | (h32 >> (32 - 17)));
	h32 = PRIME32_2 * (h32 ^ (h32 >> 15));
	h32 = PRIME32_3 * (h32 ^ (h32 >> 13));
	return h32 ^ (h32 >> 16);
}

uint xxhash32(const uint4 vec)
{
	const uint PRIME32_2 = 2246822519U, PRIME32_3 = 3266489917U;
	const uint PRIME32_4 = 668265263U, PRIME32_5 = 374761393U;
	uint h32 = vec.w + PRIME32_5 + vec.x * PRIME32_3;
	h32 = PRIME32_4 * ((h32 << 17) | (h32 >> (32 - 17)));
	h32 += vec.y * PRIME32_3;
	h32 = PRIME32_4 * ((h32 << 17) | (h32 >> (32 - 17)));
	h32 += vec.z * PRIME32_3;
	h32 = PRIME32_4 * ((h32 << 17) | (h32 >> (32 - 17)));
	h32 = PRIME32_2 * (h32 ^ (h32 >> 15));
	h32 = PRIME32_3 * (h32 ^ (h32 >> 13));
	return h32 ^ (h32 >> 16);
}


// Construct a float with half-open range [0:1] using low 23 bits.
// All zeroes yields 0.0, all ones yields the next smallest representable value below 1.0.
float floatConstruct( uint m ) {
    const uint ieeeMantissa = 0x007FFFFFu; // binary32 mantissa bitmask
    const uint ieeeOne      = 0x3F800000u; // 1.0 in IEEE binary32

    m &= ieeeMantissa;                     // Keep only mantissa bits (fractional part)
    m |= ieeeOne;                          // Add fractional part to 1.0

    float  f = asfloat( m );       // Range [1:2]
    return saturate(f - 1.0);                        // Range [0:1]
}

float random( float x ) {
	return floatConstruct(hash(asuint(x)));
}

float random( float2 x ) {
	return floatConstruct(hash(asuint(x)));
}

float random( float3 x ) {
	return floatConstruct(hash(asuint(x)));
}

float temporal_random( float x, float phase_shift ) {
	return floatConstruct(hash(asuint(float2(x, timers.x + phase_shift))));
}

float temporal_random( float2 x, float phase_shift ) {
	return floatConstruct(hash(asuint(float3(x, timers.x + phase_shift))));
}

float temporal_random( float3 x, float phase_shift ) {
	return floatConstruct(hash(asuint(float4(x, timers.x + phase_shift))));
}


////
float4 sample_jitter(float2 tc) {
    return jitter0.Sample( smp_jitter, tc * screen_res.xy * 1.f / JITTER_TEXTURE_SIZE );
}

////

float3 sample_dither_unorm_scalar(float2 tc, float phase_shift) {
	//return 0.0.xxx;
	return random(float3(tc, timers.x + phase_shift + 987));
	//return sample_jitter(tc) * intermediate_half_quantum;
	//return sample_jitter(tc) * 0.5 / 255.0;
    //return /*sample_jitter(tc)*/ random_color(tc) * 0.5 / 255.0;
}

float3 sample_dither_snorm_scalar(float2 tc, float phase_shift) {
	//return 0.0.xxx;
	return sample_dither_unorm_scalar(tc,phase_shift) * 2.0 - 1.0;
	//return sample_jitter(tc) * intermediate_half_quantum;
	//return sample_jitter(tc) * 0.5 / 255.0;
    //return /*sample_jitter(tc)*/ random_color(tc) * 0.5 / 255.0;
}

float3 sample_dither_snorm(float2 tc, float phase_shift) {
	//return 0.0.xxx;
	return (float3(
		random(float3(tc, timers.x + phase_shift + 987)),
		random(float3(tc, timers.y + phase_shift + 99999.411)),
		random(float3(tc, timers.z + phase_shift + 77999339.73))
		) * 2.0 - 1.0);
	//return sample_jitter(tc) * intermediate_half_quantum;
	//return sample_jitter(tc) * 0.5 / 255.0;
    //return /*sample_jitter(tc)*/ random_color(tc) * 0.5 / 255.0;
}


/// Remapping

float uniform_to_triangle( float nrnd0 )
{
	// Based on https://www.shadertoy.com/view/4t2SDh
    // Convert uniform distribution into triangle-shaped distribution.
    float orig = nrnd0 * 2.0 - 1.0;
    nrnd0 = orig * rsqrt(abs(orig));
    nrnd0 = max(-1.0, nrnd0); // Nerf the NaN generated by 0*rsqrt(0). Thanks @FioraAeterna!
    nrnd0 = nrnd0 - sign(orig) + 0.5;
    
    // Result is range [-0.5,1.5] which is
    // useful for actual dithering.
    // convert to [0,1] for histogram.
    return (nrnd0 + 0.5) * 0.5;
}


/// Final dither

// Use phase shift to decorrelate noise if you use dither several times at single pixel each frame
// Don't use same phase_shift in different places in your code to reduce the total visible noise
float sample_intermediate_dither_scalar(float value, float2 tc, float phase_shift) {
	//return 0.0.xxx;
	return random(float3(tc, timers.x + phase_shift)) * intermediate_half_quantum(value);
	//return sample_jitter(tc) * intermediate_half_quantum;
	//return sample_jitter(tc) * 0.5 / 255.0;
    //return /*sample_jitter(tc)*/ random_color(tc) * 0.5 / 255.0;
}

float3 sample_intermediate_dither(float3 value, float2 tc, float phase_shift) {
	//return 0.0.xxx;
	return sample_dither_snorm(tc, phase_shift)
		*  intermediate_half_quantum(value);
	//return sample_jitter(tc) * intermediate_half_quantum;
	//return sample_jitter(tc) * 0.5 / 255.0;
    //return /*sample_jitter(tc)*/ random_color(tc) * 0.5 / 255.0;
}

float3 sample_bloom_dither(float3 value, float2 tc, float phase_shift) {
	//return 0.0.xxx;
	return sample_intermediate_dither(value, tc, phase_shift);
	//return sample_jitter(tc) * intermediate_half_quantum;
	//return sample_jitter(tc) * 0.5 / 255.0;
    //return /*sample_jitter(tc)*/ random_color(tc) * 0.5 / 255.0;
}

#endif