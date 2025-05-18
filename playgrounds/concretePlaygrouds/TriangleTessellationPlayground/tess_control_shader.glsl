#version 410 core

uniform vec3 tessLevelOuter;
uniform float tessLevelInner;

layout(vertices = 3) out;

void main()
{
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    if (gl_InvocationID == 0) 
    {
        gl_TessLevelOuter[0] = tessLevelOuter.x;
        gl_TessLevelOuter[1] = tessLevelOuter.y;
        gl_TessLevelOuter[2] = tessLevelOuter.z;

        // 内部细分等级（对于三角形只有一个）
        gl_TessLevelInner[0] = tessLevelInner;
    }
}
