#version 330 core
layout(location = 0) out vec4 out_pxColor;

in vec2 TexCoord; // from vertex shader
in vec3 FragNormal; // from vertex shader

uniform sampler2D aTex;

void main()
{
    out_pxColor = texture(aTex, TexCoord);

    //out_pxColor = vec4(vec3(gl_FragCoord.z), 1.0);
}