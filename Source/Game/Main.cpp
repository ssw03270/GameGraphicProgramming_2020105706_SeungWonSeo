/*+===================================================================
  File:      MAIN.CPP

  Summary:   This application demonstrates creating a Direct3D 11 device

  Origin:    http://msdn.microsoft.com/en-us/library/windows/apps/ff729718.aspx

  Originally created by Microsoft Corporation under MIT License
  � 2022 Kyung Hee University
===================================================================+*/

#include "Common.h"

#include <memory>

#include "Cube/Cube01.h"
#include "Cube/Cube02.h"

/*--------------------------------------------------------------------
  TODO: Include custom cubes (remove the comment)
--------------------------------------------------------------------*/
#include "Game/Game.h"

/*F+F+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  Function: wWinMain

  Summary:  Entry point to the program. Initializes everything and
            goes into a message processing loop. Idle time is used to
            render the scene.

  Args:     HINSTANCE hInstance
              Handle to an instance.
            HINSTANCE hPrevInstance
              Has no meaning.
            LPWSTR lpCmdLine
              Contains the command-line arguments as a Unicode
              string
            INT nCmdShow
              Flag that says whether the main application window
              will be minimized, maximized, or shown normally

  Returns:  INT
              Status code.
-----------------------------------------------------------------F-F*/
INT WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ INT nCmdShow)
{

#ifdef _DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    std::unique_ptr<library::Game> game = std::make_unique<library::Game>(L"Game Graphics Programming Lab 04: 3D Spaces and Transformations");

    std::shared_ptr<library::VertexShader> vertexShader = std::make_shared<library::VertexShader>(L"Shaders/Shaders.fxh", "VS", "vs_5_0");
    if (FAILED(game->GetRenderer()->AddVertexShader(L"MainShader", vertexShader)))
    {
        return 0;
    }
    
    std::shared_ptr<library::PixelShader> pixelShader = std::make_shared<library::PixelShader>(L"Shaders/Shaders.fxh", "PS", "ps_5_0");
    if (FAILED(game->GetRenderer()->AddPixelShader(L"MainShader", pixelShader)))
    {
        return 0;
    }

    /*--------------------------------------------------------------------
      TODO: Add your cubes and set their shaders (remove the comment)
    --------------------------------------------------------------------*/
    std::shared_ptr<Cube01> cube01 = std::make_shared<Cube01>();
    std::shared_ptr<Cube02> cube02 = std::make_shared<Cube02>();

    if (FAILED(game->GetRenderer()->AddRenderable(L"Cube01", cube01)))
    {
        return 0;
    }

    if (FAILED(game->GetRenderer()->SetVertexShaderOfRenderable(L"Cube01", L"MainShader")))
    {
        return 0;
    }

    if (FAILED(game->GetRenderer()->SetPixelShaderOfRenderable(L"Cube01", L"MainShader")))
    {
        return 0;
    }

    if (FAILED(game->GetRenderer()->AddRenderable(L"Cube02", cube02)))
    {
        return 0;
    }

    if (FAILED(game->GetRenderer()->SetVertexShaderOfRenderable(L"Cube02", L"MainShader")))
    {
        return 0;
    }

    if (FAILED(game->GetRenderer()->SetPixelShaderOfRenderable(L"Cube02", L"MainShader")))
    {
        return 0;
    }

    if (FAILED(game->Initialize(hInstance, nCmdShow)))
    {
        return 0;
    }

    return game->Run();
}