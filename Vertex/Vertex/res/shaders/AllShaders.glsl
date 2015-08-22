#VS

void main()
{
    gl_Position = vec4(1.0f, 1.0f, 0.0f, 1.0f);
}

#FS

out vec4 fragColor;

void main()
{
    fragColor = vec4(0.5f, 0.1f, 0.0f, 1.0f);
}