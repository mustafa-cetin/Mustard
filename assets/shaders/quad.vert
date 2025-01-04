#version 430 core

void main()
{
    // generating vertices on gpu mostly because we have 2d engine
    // -1 1    1 1
    // -1 -1   1 -1
    vec2 vertices[6] =
    {
        // top left
        vec2(-0.5, 0.5),
        // bot left
        vec2(-0.5, -0.5),

        // top right
        vec2(0.5, 0.5),

        // top right
        vec2(0.5, 0.5),

        // bot left

        vec2(-0.5, -0.5),

        // bot right
        vec2(0.5, -0.5),
    };

    gl_Position = vec4(vertices[gl_VertexID], 1.0, 1.0);
}