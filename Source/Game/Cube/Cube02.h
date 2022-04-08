#pragma once

#include "Common.h"

#include "Cube/BaseCube.h"
#include "Renderer/DataTypes.h"
#include "Renderer/Renderable.h"

class Cube02 : public BaseCube
{
public:
	Cube02() = default;
	~Cube02() = default;

	virtual void Update(_In_ FLOAT deltaTime) override;
};