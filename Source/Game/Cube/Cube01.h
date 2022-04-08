#pragma once

#include "Common.h"

#include "Cube/BaseCube.h"
#include "Renderer/DataTypes.h"
#include "Renderer/Renderable.h"

class Cube01 : public BaseCube
{
public:
	Cube01() = default;
	~Cube01() = default;

	virtual void Update(_In_ FLOAT deltaTime) override;
};