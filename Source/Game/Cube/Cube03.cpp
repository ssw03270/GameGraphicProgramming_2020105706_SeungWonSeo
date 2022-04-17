#include "Cube/Cube03.h"

/*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
  Method:   Cube::Cube

  Summary:  Constructor

  Args:     const std::filesystem::path& textureFilePath
			  Path to the texture to use
M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
/*--------------------------------------------------------------------
  TODO: Cube::Cube definition (remove the comment)
--------------------------------------------------------------------*/

Cube03::Cube03(const std::filesystem::path& textureFilePath)
	: BaseCube(textureFilePath)
{ }

/*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
  Method:   Cube::Update

  Summary:  Updates the cube every frame

  Args:     FLOAT deltaTime
			  Elapsed time

  Modifies: [m_world].
M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
void Cube03::Update(_In_ FLOAT deltaTime)
{
	static FLOAT s_totalTime = 0.0f;
	s_totalTime += deltaTime;

	XMMATRIX mOrbit01 = XMMatrixRotationZ(-s_totalTime);
	XMMATRIX mOrbit02 = XMMatrixRotationY(-s_totalTime);
	XMMATRIX mOrbit03 = XMMatrixRotationX(-s_totalTime);
	XMMATRIX mTranslate = XMMatrixTranslation(-4.0f, 0.0f, 0.0f);

	m_world = mTranslate * mOrbit01 * mOrbit02 * mOrbit03;
}