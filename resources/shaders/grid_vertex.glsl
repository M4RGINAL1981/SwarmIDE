#version 450 core
layout (location = 0) in vec2 aPos; // NDC [-1, 1]

out vec2 worldPos;

uniform vec2 cameraOffset;
uniform float cameraZoom;
uniform vec2 windowSize;

void main() {
    // Устанавливаем позицию вершины
    gl_Position = vec4(aPos, 0.0, 1.0);
    
    // 1. Переводим NDC [-1, 1] в пиксели экрана (0,0 — левый верхний угол)
    vec2 screenPos = vec2(
        (aPos.x + 1.0) * 0.5 * windowSize.x, 
        (1.0 - aPos.y) * 0.5 * windowSize.y
    );
    
    // 2. Центрируем экран для правильного зума из центра
    vec2 halfWin = windowSize * 0.5;
    vec2 centeredScreenPos = screenPos - halfWin;

    // 3. Вычисляем мировые координаты
    worldPos = (centeredScreenPos / cameraZoom) - cameraOffset;
}