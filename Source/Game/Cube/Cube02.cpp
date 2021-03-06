#include "Cube/Cube02.h"

void Cube02::Update(_In_ FLOAT deltaTime)
{
	float speed = 10;
	XMMATRIX mSpin = XMMatrixRotationZ(-deltaTime * speed);
	XMMATRIX mOrbit = XMMatrixRotationY(-deltaTime * 2.0f);
	XMMATRIX mTranslate = XMMatrixTranslation(-4.0f, 0.0f, 0.0f);
	XMMATRIX mScale = XMMatrixScaling(0.3f, 0.3f, 0.3f);

	m_world = mScale * mSpin * mTranslate * mOrbit;
}