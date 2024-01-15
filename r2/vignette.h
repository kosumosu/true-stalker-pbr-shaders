// Vignette
#define intensity effect_params.y

float3 VignettePass(float2 center, float3 colorInput)
{
	float vig = 1.0f - smoothstep(0.1f, 1.0f, length(center));
	return lerp(colorInput, colorInput*vig, intensity);
}