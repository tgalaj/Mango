#version 450
#define FXAA_PC 1
#define FXAA_GLSL_130 1
// #define FXAA_QUALITY__PRESET 10 // Fastest (Lowest) Quality
// #define FXAA_QUALITY__PRESET 12 // Default Quality
// #define FXAA_QUALITY__PRESET 29 // High Quality
#define FXAA_QUALITY__PRESET 39 // Extremely High Quality

#define FXAA_GREEN_AS_LUMA 1

#include "FXAA3_11.glh"

//precision highp float;

layout(binding = 0) uniform sampler2D filterTexture;

//uniform vec2 RCPFrame;

in vec2 texcoord;
out vec4 fragColor;

void main(void)
{
    vec2 RCPFrame = 1.0f / textureSize(filterTexture, 0).xy;

    fragColor.rgb = FxaaPixelShader(texcoord,
                                    FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f),	// FxaaFloat4 fxaaConsolePosPos,
                                    filterTexture,						// FxaaTex fxaaConsole360TexExpBiasNegOne,
                                    filterTexture,						// FxaaTex tex,
                                    filterTexture,						// FxaaTex fxaaConsole360TexExpBiasNegTwo,
                                    RCPFrame,							// FxaaFloat2 fxaaQualityRcpFrame,
                                    FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f),	// FxaaFloat4 fxaaConsoleRcpFrameOpt,
                                    FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f),	// FxaaFloat4 fxaaConsoleRcpFrameOpt2,
                                    FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f),	// FxaaFloat4 fxaaConsole360RcpFrameOpt2,
                                    0.75f,								// FxaaFloat fxaaQualitySubpix,
                                    0.166f,								// FxaaFloat fxaaQualityEdgeThreshold,
                                    0.0833f,							// FxaaFloat fxaaQualityEdgeThresholdMin,
                                    0.0f,								// FxaaFloat fxaaConsoleEdgeSharpness,
                                    0.0f,								// FxaaFloat fxaaConsoleEdgeThreshold,
                                    0.0f,								// FxaaFloat fxaaConsoleEdgeThresholdMin,
                                    FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f)	// FxaaFloat fxaaConsole360ConstDir,
    ).rgb;
    
    fragColor.a = 1.0f;
}