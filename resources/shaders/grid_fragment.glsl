#version 450 core
in vec2 worldPos;
out vec4 FragColor;

uniform vec3 gridColor;
uniform float stepSize;
uniform float thickness;

void main() {
    vec2 grid = abs(fract(worldPos / stepSize - 0.5) - 0.5) / fwidth(worldPos / stepSize);
    float line = min(grid.x, grid.y);
    float alpha = 1.0 - min(line / thickness, 1.0);
    
    if (alpha < 0.1) discard;
    FragColor = vec4(gridColor, alpha);
}