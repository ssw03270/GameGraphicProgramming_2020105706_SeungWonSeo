#include "Cube/Cube03.h"

void Cube03::Update(_In_ FLOAT deltaTime)
{
	XMMATRIX mOrbit01 = XMMatrixRotationZ(-deltaTime);
	XMMATRIX mOrbit02 = XMMatrixRotationY(-deltaTime);
	XMMATRIX mOrbit03 = XMMatrixRotationX(-deltaTime);
	XMMATRIX mTranslate = XMMatrixTranslation(-4.0f, 0.0f, 0.0f);
	XMMATRIX mScale = XMMatrixScaling(0.1f, 0.1f, 0.1f);

	m_world = mScale * mTranslate * mOrbit01 * mOrbit02 * mOrbit03;
}