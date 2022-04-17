#pragma once

#include "Common.h"

#include "Cube/BaseCube.h"
#include "Renderer/DataTypes.h"
#include "Renderer/Renderable.h"

class Cube03 : public BaseCube
{
public:
    Cube03(const std::filesystem::path& textureFilePath);
    Cube03(const Cube03& other) = delete;
    Cube03(Cube03&& other) = delete;
    Cube03& operator=(const Cube03& other) = delete;
    Cube03& operator=(Cube03&& other) = delete;
    ~Cube03() = default;

    virtual void Update(_In_ FLOAT deltaTime) override;
};