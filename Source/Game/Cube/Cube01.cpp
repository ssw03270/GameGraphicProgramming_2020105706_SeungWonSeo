#include "Cube/Cube01.h"

void Cube01::Update(_In_ FLOAT deltaTime)
{
    m_world = XMMatrixRotationY(deltaTime);
}