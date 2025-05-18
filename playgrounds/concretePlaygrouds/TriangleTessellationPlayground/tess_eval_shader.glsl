#version 410 core
layout(triangles, equal_spacing, cw) in;

void main()
{
    // barycentric coordinates
    vec3 bary = gl_TessCoord;
    
    gl_Position =
          bary.x * gl_in[0].gl_Position
        + bary.y * gl_in[1].gl_Position
        + bary.z * gl_in[2].gl_Position;
}
