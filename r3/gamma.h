#ifndef	GAMMA_H_INCLUDED
#define	GAMMA_H_INCLUDED

#define GAMMA_SPACE 0
#define LINEAR_SPACE_WITH_GAMMA_COMBINE 1
#define LINEAR_SPACE 2
#define UNORM_INPUT_SRGB_BUFFERS 3


#define GAMMA_MODE UNORM_INPUT_SRGB_BUFFERS


float srgb_to_linear(float val)
{ 
    if( val < 0.04045f )
        val /= 12.92f;
    else
        val = pow((val + 0.055f)/1.055f,2.4f);
    return val;
}

float3 srgb_to_linear(float3 val)
{
	return float3(srgb_to_linear(val.r), srgb_to_linear(val.g), srgb_to_linear(val.b));
}

float4 srgb_to_linear(float4 val)
{
	return float4(srgb_to_linear(val.r), srgb_to_linear(val.g), srgb_to_linear(val.b), srgb_to_linear(val.a));
}

float linear_to_srgb(float val)
{
    return val < 0.0031308
        ? val * 12.92
        : 1.055 * pow(val, 1.0 / 2.4) - 0.055;
}

float srgb_to_linear_derivative(float val)
{ 
    if( val < 0.04045 )
        return 1.0 / 12.92;
    else
        return 2.1106 * pow(val + 0.055, 1.4);
}

float srgb_derivative_of_linear_value(float val)
{
    return val < 0.0031308
        ? 1.0 / 12.92
        : 2.27488 * pow(val, 0.583333);
}


#if GAMMA_MODE == LINEAR_SPACE

float3 display_to_albedo_space(float3 value) {
    return value;
}

float3 albedo_to_working_space(float3 value) {
    return value * value;
}

float4 display_to_working_space(float4 value) {
    return value * value;
}

float4 working_to_intermediate_space(float4 value) {
    return value;
}

float4 display_to_intermediate_space(float4 value) {
    return value * value;
}

float4 intermediate_to_working_space(float4 value) {
    return value;
}

float4 working_to_display_space(float4 value) {
    return pow(value, 0.5);
}

float intermediate_half_quantum(float value) {
    return 0.5 / 255;
}

//////////////////
#elif GAMMA_MODE == LINEAR_SPACE_WITH_GAMMA_COMBINE

float3 display_to_albedo_space(float3 value) {
    return value;
}

float3 albedo_to_working_space(float3 value) {
    return value * value;
}

float4 display_to_working_space(float4 value) {
    return value * value;
}

float4 working_to_intermediate_space(float4 value) {
    return pow(value, 0.5);
}

float4 display_to_intermediate_space(float4 value) {
    return value;
}

float4 intermediate_to_working_space(float4 value) {
    return value * value;
}

float4 working_to_display_space(float4 value) {
    return pow(value, 0.5);
}

float intermediate_half_quantum(float value) {
    return 0.5 / 255;
}

//////////////////
#elif GAMMA_MODE == UNORM_INPUT_SRGB_BUFFERS

float3 display_to_albedo_space(float3 value) {
    return srgb_to_linear(value);
}

float3 albedo_to_working_space(float3 value) {
    return value;
}

float4 display_to_working_space(float4 value) {
    return srgb_to_linear(value);
}

float4 working_to_intermediate_space(float4 value) {
    return value;
}

float4 display_to_intermediate_space(float4 value) {
    return srgb_to_linear(value);
}

float4 intermediate_to_working_space(float4 value) {
    return value;
}

float4 working_to_display_space(float4 value) {
    return value;
}

float intermediate_half_quantum(float value) {
    return srgb_derivative_of_linear_value(value) * 0.5 / 255.0;
}

//////////////////
#elif GAMMA_MODE == GAMMA_SPACE

float3 display_to_albedo_space(float3 value) {
    return value;
}

float3 albedo_to_working_space(float3 value) {
    return value;
}

float4 display_to_working_space(float4 value) {
    return value;
}

float4 working_to_intermediate_space(float4 value) {
    return value;
}

float4 display_to_intermediate_space(float4 value) {
    return value;
}

float4 intermediate_to_working_space(float4 value) {
    return value;
}

float4 working_to_display_space(float4 value) {
    return value;
}

float intermediate_half_quantum(float value) {
    return 0.5 / 255;
}

#else
#error "unimplemented"
#endif


float3 display_to_working_space(float3 value) {
    return display_to_working_space(value.rgbb).rgb;
}

float3 working_to_intermediate_space(float3 value) {
    return working_to_intermediate_space(value.rgbb).rgb;
}

float3 display_to_intermediate_space(float3 value) {
    return display_to_intermediate_space(value.rgbb).rgb;
}

float3 intermediate_to_working_space(float3 value) {
    return intermediate_to_working_space(value.rgbb).rgb;
}

float3 working_to_display_space(float3 value) {
    return working_to_display_space(value.rgbb).rgb;
}

//////////////////
float display_to_working_space(float value) {
    return display_to_working_space(value.rrrr).r;
}

float working_to_intermediate_space(float value) {
    return working_to_intermediate_space(value.rrrr).r;
}

float display_to_intermediate_space(float value) {
    return display_to_intermediate_space(value.rrrr).r;
}

float intermediate_to_working_space(float value) {
    return intermediate_to_working_space(value.rrrr).r;
}

float working_to_display_space(float value) {
    return working_to_display_space(value.rrrr).r;
}


float3 intermediate_half_quantum(float3 value) {
    return float3(
        intermediate_half_quantum(value.r),
        intermediate_half_quantum(value.g),
        intermediate_half_quantum(value.b)
    );
}

#endif