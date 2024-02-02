#ifndef	GAMMA_H_INCLUDED
#define	GAMMA_H_INCLUDED

#define GAMMA_SPACE 0
#define LINEAR_SPACE_WITH_GAMMA_COMBINE 1
#define LINEAR_SPACE 2


#define GAMMA_MODE LINEAR_SPACE_WITH_GAMMA_COMBINE


#if GAMMA_MODE == LINEAR_SPACE
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


//////////////////
#elif GAMMA_MODE == LINEAR_SPACE_WITH_GAMMA_COMBINE

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


//////////////////
#elif GAMMA_MODE == GAMMA_SPACE

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


#endif