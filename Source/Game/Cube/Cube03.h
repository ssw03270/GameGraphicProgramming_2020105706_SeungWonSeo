#pragma once

#include "Common.h"

#include "Cube/BaseCube.h"
#include "Renderer/DataTypes.h"
#include "Renderer/Renderable.h"

class Cube03 : public BaseCube
{
public:
	Cube03() = default;
	~Cube03() = default;

	virtual void Update(_In_ FLOAT deltaTime) override;
};