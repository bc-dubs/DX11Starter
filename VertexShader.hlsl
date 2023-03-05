cbuffer ExternalData : register(b0) {
	matrix world;
	matrix view;
	matrix projection;
}

// Struct representing a single vertex worth of data
// - Variable order must match the vertex definition in our C++ code
// - The name of the struct itself is unimportant, but should be descriptive
// - Each variable must have a semantic (e.g. POSITION), which defines its usage
struct VertexShaderInput
{ 
	float3 localPosition	: POSITION;     // XYZ position
	float3 normal			: NORMAL;       // XYZ normal
	float2 uv				: TEXCOORD;		// UV position
};

// Struct representing the data we're sending down the pipeline
// - Should match our pixel shader's input (hence the name: Vertex to Pixel)
// - At a minimum, we need a piece of data defined tagged as SV_POSITION
struct VertexToPixel
{
	float4 screenPosition	: SV_POSITION;	// XYZW position (System Value Position)
	float2 uv				: TEXCOORD;     // UV position
};

// --------------------------------------------------------
// The entry point (main method) for our vertex shader
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
VertexToPixel main( VertexShaderInput input )
{
	// Set up output struct
	VertexToPixel output;

	// X and Y components must be between -1 and 1, Z component must be between 0 and 1.  
	// Each of these components is then automatically divided by the W component (1.0 for now)
	matrix wvp = mul(projection, mul(view, world));
	output.screenPosition = mul(wvp, float4(input.localPosition, 1.0f));

	// Pass the color through 
	output.uv = input.uv;

	// Send the output to the next stage (pixel shader)
	return output;
}