#version 460
#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_ray_tracing : require

#include "payload.glsl"
layout(location = 1) rayPayloadInEXT ShadowRayPayload srp;

hitAttributeEXT vec2 hitAttribute;

void main() 
{
    srp.visible = 0.0f;
}