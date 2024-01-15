/////// Vibrance //////
#define Vibrance effect_params.x //[-1.00:1.00] //-Intelligently saturates (or desaturates if you use negative values) the pixels depending on their original saturation.
#define Vibrance_RGB_balance float3(1.50,1.50,1.00) //[-10.00:10.00] //-A per channel multiplier to the Vibrance strength so you can give more boost to certain colors over others. X = Red, Y = Green, Z = Blue

float3 VibrancePass(float3 colorInput)
{
	#ifndef Vibrance_RGB_balance
		#define Vibrance_RGB_balance float3(1.00, 1.00, 1.00)
	#endif
	#define Vibrance_coeff float3(Vibrance_RGB_balance * Vibrance)

	float3 color = colorInput;
	float3 lumCoeff = float3(0.212656, 0.715158, 0.072186);
	float luma = dot(lumCoeff, color.rgb);
	float max_color = max(colorInput.r, max(colorInput.g,colorInput.b));
	float min_color = min(colorInput.r, min(colorInput.g,colorInput.b));
	float color_saturation = max_color - min_color;
	color.rgb = lerp(luma, color.rgb, (1.0 + (Vibrance_coeff * (1.0 - (sign(Vibrance_coeff) * color_saturation)))));
	return color;
}