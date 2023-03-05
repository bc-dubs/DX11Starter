cbuffer ExternalData : register(b0) {
	float4 colorTint;
	float4 functionVars;	// X = x coefficient, Y = y coefficient
	int xFunction;
	int yFunction;
}

// Struct representing the data we expect to receive from earlier pipeline stages
// - Should match the output of our corresponding vertex shader
// - The name of the struct itself is unimportant
// - The variable names don't have to match other shaders (just the semantics)
// - Each variable must have a semantic, which defines its usage
struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float4 screenPosition	: SV_POSITION;	// XYZW position (System Value Position)
	float2 uv				: TEXCOORD;     // UV position
};

float applyFunction(int index, float x, float y) {
	switch (index) { // Has a switch statement but should not actually branch fortunately
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
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
	// Just return the input color
	// - This color (like most values passing through the rasterizer) is 
	//   interpolated for each pixel between the corresponding vertices 
	//   of the triangle we're rendering

	float u = input.uv.x - 0.5;
	float v = input.uv.y - 0.5;

	float2 vec = float2(applyFunction(xFunction, functionVars.x * u, functionVars.x * v), applyFunction(yFunction, functionVars.y * v, functionVars.y * u));

	float3 uPos = float3(1, 0, 0);
	float3 uNeg = float3(0, 1, 1);
	float3 vPos = float3(0.5, 1, 0);
	float3 vNeg = float3(0.5, 0, 1);

	float3 color = normalize(max(vec.x, 0) * uPos - min(vec.x, 0) * uNeg + max(vec.y, 0) * vPos - min(vec.y, 0) * vNeg) * length(vec) / functionVars.z + length(vec).xxxx/ functionVars.w;

	return float4(color, 1);
}

