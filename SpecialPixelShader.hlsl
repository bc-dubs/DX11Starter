// Ben Coukos-Wiley
// 3/3/2023
// A pixel shader based on a (U,V)-defined vector field

#include "ShaderIncludes.hlsli"

cbuffer ExternalData : register(b0) {
	float4 colorTint;		// Unused in this shader
	float4 functionVars;	// X = x coefficient, Y = y coefficient, Z = first lightness divider, W = second lightness divider
	int xFunction;			// The index of the X function
	int yFunction;			// The index of the Y function
}

// A helper method to apply the selected vector field function
float applyFunction(int index, float x, float y) {
	switch (index) { // Has a switch statement but should not actually branch since the index is the same for every pixel
	case 0:
		return x;
	case 1:
		return y;
	case 2:
		return x + y;
	case 3:
		return x - y;
	case 4:
		return pow(x, 2);
	case 5:
		return pow(y, 2);
	case 6:
		return pow(x + y, 2);
	case 7:
		return pow(x - y, 2);
	case 8:
		return sqrt(x);
	case 9:
		return sqrt(y);
	case 10:
		return sqrt(x + y);
	case 11:
		return sqrt(x - y);
	case 12:
		return pow(2, x);
	case 13:
		return pow(2, y);
	case 14:
		return pow(2, x + y);
	case 15:
		return pow(2, x - y);
	case 16:
		return log2(x);
	case 17:
		return log2(y);
	case 18:
		return log2(x + y);
	case 19:
		return log2(x - y);
	case 20:
		return sin(x);
	case 21:
		return sin(y);
	case 22:
		return sin(x + y);
	case 23:
		return sin(x - y);
	case 24:
		return cos(x);
	case 25:
		return cos(y);
	case 26:
		return cos(x + y);
	case 27:
		return cos(x - y);
	case 28:
		return tan(x);
	case 29:
		return tan(y);
	case 30:
		return tan(x + y);
	case 31:
		return tan(x - y);
	}
	return x;
}

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
	// Values are now -0.5 to 0.5, shifting the origin to the center of the UV
	float u = input.uv.x - 0.5;
	float v = input.uv.y - 0.5;

	// Apply the user's selected X and Y functions to the (U, V) coordinate to get the corresponding <X, Y> vector for this pixel
	float2 vec = float2(applyFunction(xFunction, functionVars.x * u, functionVars.x * v), applyFunction(yFunction, functionVars.y * v, functionVars.y * u));

	// I originally wanted the direction of vec to correspond to a location on the HSL hue wheel, while its magnitude corresponded to lightness
	// But since converting to RGB would involve a lot of math and potentially 6-way branching I made a simpler, more scuffed transition instead
	float3 uPos = float3(1, 0, 0);		// The color that represents positive U/X (Red)
	float3 uNeg = float3(0, 1, 1);		// The color that represents negative U/X (Cyan)
	float3 vPos = float3(0.5, 1, 0);	// The color that represents positive V/Y (Greenish-yellow)
	float3 vNeg = float3(0.5, 0, 1);	// The color that represents negative V/Y (Purplish-blue)

	// So in theory: 
	//   - a red pixel is pointing along the positive U axis
	//	 - a purple-blue pixel is pointing along the negative V axis
	//	 - a green pixel is pointing at about 60% negative U and 30 % positive V 

	// - The X magnitude is multiplied by uPos or uNeg based on its X direction (+/-). 
	//		- A similar process is done for Y, then the results are added and normalized for a final hue value.
	// - To artificially create lightness, vec's magnitude is both multiplied by the normalized hue and added to all color components. 
	//		- Both uses of the magnitude are divided by separate coefficients to allow for greater precision in the user controls.
	float3 color = normalize(max(vec.x, 0) * uPos - min(vec.x, 0) * uNeg + max(vec.y, 0) * vPos - min(vec.y, 0) * vNeg) * length(vec) / functionVars.z + length(vec).xxxx/ functionVars.w;

	return float4(color, 1);
}

