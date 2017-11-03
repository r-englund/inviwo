/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2017 Inviwo Foundation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *********************************************************************************/

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vData
{
    vec4 worldPosition;
    vec4 position;
    vec3 normal;
    vec3 viewNormal;
    vec4 color;
} vertices[];

out fData
{
    vec4 worldPosition;
    vec4 position;
    vec3 normal;
    vec3 viewNormal;
    vec4 color;
    float area;
#ifdef ALPHA_SHAPE
    vec3 sideLengths;
#endif
} frag;

void main(void) 
{
    //compute the area of the triangle,
    //needed for several shading computations
    float area = length(cross(vertices[1].position.xyz-vertices[0].position.xyz,
                  vertices[2].position.xyz-vertices[0].position.xyz)) * 0.5f;
    
    //compute side lengths of the triangles
#ifdef ALPHA_SHAPE
    vec3 sideLengths;
    sideLengths.x = length(vertices[1].position.xyz - vertices[0].position.xyz);
    sideLengths.y = length(vertices[2].position.xyz - vertices[0].position.xyz);
    sideLengths.z = length(vertices[2].position.xyz - vertices[1].position.xyz);
#endif
    
    //pass-through other parameters
    for (int i=0; i<3; ++i)
    {
        frag.worldPosition = vertices[i].worldPosition;
        frag.position = vertices[i].position;
        frag.normal = vertices[i].normal;
        frag.viewNormal = vertices[i].viewNormal;
        frag.color = vertices[i].color;
        frag.area = area;
#ifdef ALPHA_SHAPE
        frag.sideLengths = sideLengths;
#endif
        gl_Position = vertices[i].position;
        EmitVertex();
    }
    EndPrimitive();
}
