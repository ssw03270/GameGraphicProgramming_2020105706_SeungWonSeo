#include "Renderer/Renderer.h"

namespace library
{
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::Renderer

      Summary:  Constructor

      Modifies: [m_driverType, m_featureLevel, m_d3dDevice, m_d3dDevice1,
                 m_immediateContext, m_immediateContext1, m_swapChain,
                 m_swapChain1, m_renderTargetView, m_depthStencil,
                 m_depthStencilView, m_cbChangeOnResize, m_camera,
                 m_projection, m_renderables, m_vertexShaders, 
                 m_pixelShaders].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderer::Renderer definition (remove the comment)
    --------------------------------------------------------------------*/
    Renderer::Renderer()
        : m_driverType(D3D_DRIVER_TYPE_NULL)
        , m_featureLevel(D3D_FEATURE_LEVEL_11_0)
        , m_d3dDevice(nullptr)
        , m_d3dDevice1(nullptr)
        , m_immediateContext(nullptr)
        , m_immediateContext1(nullptr)
        , m_swapChain(nullptr)
        , m_swapChain1(nullptr)
        , m_renderTargetView(nullptr)
        , m_depthStencil(nullptr)
        , m_depthStencilView(nullptr)
        , m_cbChangeOnResize(nullptr)
        , m_cbLights(nullptr)
        , m_camera({ 0.0f, 3.0f, -6.0f, 0.0f })
        , m_projection()
        , m_renderables(std::unordered_map<std::wstring, std::shared_ptr<Renderable>>())
        , m_aPointLights()
        , m_vertexShaders(std::unordered_map<std::wstring, std::shared_ptr<VertexShader>>())
        , m_pixelShaders(std::unordered_map<std::wstring, std::shared_ptr<PixelShader>>())
    { }
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::Initialize

      Summary:  Creates Direct3D device and swap chain

      Args:     HWND hWnd
                  Handle to the window

      Modifies: [m_d3dDevice, m_featureLevel, m_immediateContext,
                 m_d3dDevice1, m_immediateContext1, m_swapChain1,
                 m_swapChain, m_renderTargetView, m_cbChangeOnResize, 
                 m_projection, m_cbLights, m_camera, m_vertexShaders, 
                 m_pixelShaders, m_renderables].

      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderer::Initialize definition (remove the comment)
    --------------------------------------------------------------------*/
    HRESULT Renderer::Initialize(_In_ HWND hWnd)
    {
        HRESULT hr = S_OK;

        RECT rc;

        GetClientRect(hWnd, &rc);

        UINT width = rc.right - static_cast<UINT>(rc.left);
        UINT height = rc.bottom - static_cast<UINT>(rc.top);

        UINT createDeviceFlags = 0;

        D3D_DRIVER_TYPE driverTypes[] =
        {
            D3D_DRIVER_TYPE_HARDWARE,
            D3D_DRIVER_TYPE_WARP,
            D3D_DRIVER_TYPE_REFERENCE,
        };
        UINT numDriverTypes = ARRAYSIZE(driverTypes);

        D3D_FEATURE_LEVEL featureLevels[] =
        {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
        };
        UINT numFeatureLevels = ARRAYSIZE(featureLevels);

        for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
        {
            m_driverType = driverTypes[driverTypeIndex];
            hr = D3D11CreateDevice(nullptr, m_driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
                D3D11_SDK_VERSION, m_d3dDevice.GetAddressOf(), &m_featureLevel, m_immediateContext.GetAddressOf());

            if (hr == E_INVALIDARG)
            {
                // DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
                hr = D3D11CreateDevice(nullptr, m_driverType, nullptr, createDeviceFlags, &featureLevels[1], numFeatureLevels - 1,
                    D3D11_SDK_VERSION, m_d3dDevice.GetAddressOf(), &m_featureLevel, m_immediateContext.GetAddressOf());
            }

            if (SUCCEEDED(hr))
                break;
        }
        if (FAILED(hr))
            return hr;

        // Obtain DXGI factory from device (since we used nullptr for pAdapter above)
        ComPtr<IDXGIFactory1>           dxgiFactory(nullptr);
        {
            ComPtr<IDXGIDevice>           dxgiDevice(nullptr);
            hr = m_d3dDevice.As(&dxgiDevice);
            if (SUCCEEDED(hr))
            {
                ComPtr<IDXGIAdapter>           adapter(nullptr);

                hr = dxgiDevice->GetAdapter(adapter.GetAddressOf());
                if (SUCCEEDED(hr))
                {
                    hr = adapter->GetParent(__uuidof(IDXGIFactory1), (&dxgiFactory));
                }
            }
        }
        if (FAILED(hr))
            return hr;

        // Create swap chain
        ComPtr<IDXGIFactory2>           dxgiFactory2(nullptr);
        hr = dxgiFactory.As(&dxgiFactory2);
        if (dxgiFactory2)
        {
            // DirectX 11.1 or later
            hr = m_d3dDevice.As(&m_d3dDevice1);
            if (SUCCEEDED(hr))
            {
                hr = m_immediateContext.As(&m_immediateContext1);
            }

            DXGI_SWAP_CHAIN_DESC1 sd =
            {
                .Width = width,
                .Height = height,
                .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
                .SampleDesc =
                {
                    .Count = 1,
                    .Quality = 0
                },
                .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
                .BufferCount = 1
            };

            hr = dxgiFactory2->CreateSwapChainForHwnd(m_d3dDevice.Get(), hWnd, &sd, nullptr, nullptr, m_swapChain1.GetAddressOf());
            if (SUCCEEDED(hr))
            {
                hr = m_swapChain1.As(&m_swapChain);
            }
        }
        else
        {
            // DirectX 11.0 systems
            DXGI_SWAP_CHAIN_DESC sd =
            {
                .BufferDesc =
                {
                    .Width = width,
                    .Height = height,
                    .RefreshRate =
                    {
                        .Numerator = 60,
                        .Denominator = 1
                    },
                    .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
                },
                .SampleDesc =
                {
                    .Count = 1,
                    .Quality = 1,
                },
                .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
                .BufferCount = 1,
                .OutputWindow = hWnd,
                .Windowed = TRUE,
            };

            hr = dxgiFactory->CreateSwapChain(m_d3dDevice.Get(), &sd, m_swapChain.GetAddressOf());
        }

        // Note this tutorial doesn't handle full-screen swapchains so we block the ALT+ENTER shortcut
        dxgiFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);


        if (FAILED(hr))
            return hr;

        // Create a render target view
        ComPtr<ID3D11Texture2D>           pBackBuffer(nullptr);

        hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (&pBackBuffer));
        if (FAILED(hr))
            return hr;

        hr = m_d3dDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, m_renderTargetView.GetAddressOf());
        if (FAILED(hr))
            return hr;


        // Setup the viewport
        D3D11_VIEWPORT vp =
        {
            .TopLeftX = 0,
            .TopLeftY = 0,
            .Width = (FLOAT)width,
            .Height = (FLOAT)height,
            .MinDepth = 0.0f,
            .MaxDepth = 1.0f
        };

        m_immediateContext->RSSetViewports(1, &vp);

        D3D11_TEXTURE2D_DESC descDepth =
        {
            .Width = width,
            .Height = height,
            .MipLevels = 1,
            .ArraySize = 1,
            .Format = DXGI_FORMAT_D24_UNORM_S8_UINT,
            .SampleDesc = {.Count = 1, .Quality = 0 },
            .Usage = D3D11_USAGE_DEFAULT,
            .BindFlags = D3D11_BIND_DEPTH_STENCIL,
            .CPUAccessFlags = 0,
            .MiscFlags = 0
        };

        hr = m_d3dDevice->CreateTexture2D(&descDepth, nullptr, m_depthStencil.GetAddressOf());
        if (FAILED(hr))
            return hr;

        // Create the depth stencil view
        D3D11_DEPTH_STENCIL_VIEW_DESC descDSV =
        {
            .Format = descDepth.Format,
            .ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D,
            .Texture2D = {.MipSlice = 0 }
        };

        hr = m_d3dDevice->CreateDepthStencilView(m_depthStencil.Get(), &descDSV, m_depthStencilView.GetAddressOf());
        if (FAILED(hr))
            return hr;

        m_immediateContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());

        // Initialize the view matrix

        m_camera.Initialize(m_d3dDevice.Get());

        // Initialize the projection matrix
        m_projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, width / (FLOAT)height, 0.01f, 100.0f);

        D3D11_BUFFER_DESC bd = {};
        // Create the constant buffer
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(CBChangeOnResize);
        bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bd.CPUAccessFlags = 0;
        bd.MiscFlags = 0;
        bd.StructureByteStride = 0;

        hr = m_d3dDevice->CreateBuffer(&bd, nullptr, m_cbChangeOnResize.GetAddressOf());
        if (FAILED(hr))
            return hr;

        CBChangeOnResize cbChangesOnResize;
        cbChangesOnResize.Projection = XMMatrixTranspose(m_projection);
        m_immediateContext->UpdateSubresource(m_cbChangeOnResize.Get(), 0, nullptr, &cbChangesOnResize, 0, 0);

        // Create the constant buffer
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(CBLights);
        bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bd.CPUAccessFlags = 0;
        bd.MiscFlags = 0;
        bd.StructureByteStride = 0;

        hr = m_d3dDevice->CreateBuffer(&bd, nullptr, m_cbLights.GetAddressOf());
        if (FAILED(hr))
            return hr;

        for (auto iterator = m_pixelShaders.begin(); iterator != m_pixelShaders.end(); ++iterator) {
            hr = iterator->second->Initialize(m_d3dDevice.Get());

            if (FAILED(hr))
            {
                return hr;
            }
        }

        for (auto iterator = m_vertexShaders.begin(); iterator != m_vertexShaders.end(); ++iterator) {
            hr = iterator->second->Initialize(m_d3dDevice.Get());

            if (FAILED(hr))
            {
                return hr;
            }
        }

        for (auto iterator = m_renderables.begin(); iterator != m_renderables.end(); ++iterator) {
            hr = iterator->second->Initialize(m_d3dDevice.Get(), m_immediateContext.Get());

            if (FAILED(hr))
            {
                return hr;
            }
        }

        m_immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        return S_OK;
    }
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::AddRenderable

      Summary:  Add a renderable object

      Args:     PCWSTR pszRenderableName
                  Key of the renderable object
                const std::shared_ptr<Renderable>& renderable
                  Shared pointer to the renderable object

      Modifies: [m_renderables].

      Returns:  HRESULT
                  Status code.
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderer::AddRenderable definition (remove the comment)
    --------------------------------------------------------------------*/
    HRESULT Renderer::AddRenderable(_In_ PCWSTR pszRenderableName, _In_ const std::shared_ptr<Renderable>& renderable)
    {
        if (m_renderables.find(pszRenderableName) != m_renderables.end())
            return E_FAIL;

        m_renderables.insert(std::make_pair(pszRenderableName, renderable));
        return S_OK;
    }
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::AddPointLight

      Summary:  Add a point light

      Args:     size_t index
                  Index of the point light
                const std::shared_ptr<PointLight>& pointLight
                  Shared pointer to the point light object

      Modifies: [m_aPointLights].

      Returns:  HRESULT
                  Status code.
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderer::AddPointLight definition (remove the comment)
    --------------------------------------------------------------------*/
    HRESULT Renderer::AddPointLight(_In_ size_t index, _In_ const std::shared_ptr<PointLight>& pPointLight)
    {
        if (index >= NUM_LIGHTS)
            return E_FAIL;
        m_aPointLights[index] = pPointLight;
        return S_OK;
    }
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::AddVertexShader

      Summary:  Add the vertex shader into the renderer

      Args:     PCWSTR pszVertexShaderName
                  Key of the vertex shader
                const std::shared_ptr<VertexShader>&
                  Vertex shader to add

      Modifies: [m_vertexShaders].

      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderer::AddVertexShader definition (remove the comment)
    --------------------------------------------------------------------*/
    HRESULT Renderer::AddVertexShader(_In_ PCWSTR pszVertexShaderName, _In_ const std::shared_ptr<VertexShader>& vertexShader)
    {
        if (m_vertexShaders.find(pszVertexShaderName) != m_vertexShaders.end())
            return E_FAIL;

        m_vertexShaders.insert(std::make_pair(pszVertexShaderName, vertexShader));
        return S_OK;
    }
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::AddPixelShader

      Summary:  Add the pixel shader into the renderer

      Args:     PCWSTR pszPixelShaderName
                  Key of the pixel shader
                const std::shared_ptr<PixelShader>&
                  Pixel shader to add

      Modifies: [m_pixelShaders].

      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderer::AddPixelShader definition (remove the comment)
    --------------------------------------------------------------------*/
    HRESULT Renderer::AddPixelShader(_In_ PCWSTR pszPixelShaderName, _In_ const std::shared_ptr<PixelShader>& pixelShader)
    {
        if (m_pixelShaders.find(pszPixelShaderName) != m_pixelShaders.end())
            return E_FAIL;

        m_pixelShaders.insert(std::make_pair(pszPixelShaderName, pixelShader));
        return S_OK;
    }
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::HandleInput

      Summary:  Add the pixel shader into the renderer and initialize it

      Args:     const DirectionsInput& directions
                  Data structure containing keyboard input data
                const MouseRelativeMovement& mouseRelativeMovement
                  Data structure containing mouse relative input data

      Modifies: [m_camera].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderer::HandleInput definition (remove the comment)
    --------------------------------------------------------------------*/
    void Renderer::HandleInput(_In_ const DirectionsInput& directions, _In_ const MouseRelativeMovement& mouseRelativeMovement, _In_ FLOAT deltaTime)
    {
        m_camera.HandleInput(
            directions,
            mouseRelativeMovement,
            deltaTime
        );

    }
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::Update

      Summary:  Update the renderables each frame

      Args:     FLOAT deltaTime
                  Time difference of a frame
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderer::Update definition (remove the comment)
    --------------------------------------------------------------------*/
    void Renderer::Update(_In_ FLOAT deltaTime)
    {
        for (auto iterator = m_renderables.begin(); iterator != m_renderables.end(); ++iterator) {
            iterator->second->Update(deltaTime);
        }
        for (int i = 0; i < NUM_LIGHTS; i++)
        {
            m_aPointLights[i]->Update(deltaTime);
        }
    }
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::Render

      Summary:  Render the frame
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderer::Render definition (remove the comment)
    --------------------------------------------------------------------*/
    void Renderer::Render()
    {
        //
        // Clear the back buffer
        //
        m_immediateContext->ClearRenderTargetView(m_renderTargetView.Get(), Colors::MidnightBlue);

        //
        // Clear the depth buffer to 1.0 (max depth)
        //
        m_immediateContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

        CBChangeOnCameraMovement cbChangeOnCameraMovement;
        cbChangeOnCameraMovement.View = XMMatrixTranspose(m_camera.GetView());
        XMStoreFloat4(&cbChangeOnCameraMovement.CameraPosition, m_camera.GetEye());
        m_immediateContext->UpdateSubresource(m_camera.GetConstantBuffer().Get(), 0, nullptr, &cbChangeOnCameraMovement, 0, 0);
        m_immediateContext->VSSetConstantBuffers(0u, 1u, m_camera.GetConstantBuffer().GetAddressOf());
        m_immediateContext->PSSetConstantBuffers(0u, 1u, m_camera.GetConstantBuffer().GetAddressOf());

        CBLights cbLights;
        for (int i = 0; i < NUM_LIGHTS; ++i) {
            cbLights.LightColors[i] = m_aPointLights[i]->GetColor();
            cbLights.LightPositions[i] = m_aPointLights[i]->GetPosition();
            m_immediateContext->UpdateSubresource(m_cbLights.Get(), 0u, nullptr, &cbLights, 0u, 0u);
            m_immediateContext->VSSetConstantBuffers(3u, 1u, m_cbLights.GetAddressOf());
            m_immediateContext->PSSetConstantBuffers(3u, 1u, m_cbLights.GetAddressOf());
        }

        for (auto iterator = m_renderables.begin(); iterator != m_renderables.end(); ++iterator) {
            // Set the vertex buffer
            UINT stride = sizeof(SimpleVertex);
            UINT offset = 0;
            m_immediateContext->IASetVertexBuffers(0, 1, iterator->second->GetVertexBuffer().GetAddressOf(), &stride, &offset);

            // Set the index  buffer
            m_immediateContext->IASetIndexBuffer(iterator->second->GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0);

            // Set the input layout
            m_immediateContext->IASetInputLayout(iterator->second->GetVertexLayout().Get());

            CBChangesEveryFrame cbChangesEveryFrame;
            cbChangesEveryFrame.World = XMMatrixTranspose(iterator->second->GetWorldMatrix());
            cbChangesEveryFrame.OutputColor = iterator->second->GetOutputColor();
            m_immediateContext->UpdateSubresource(iterator->second->GetConstantBuffer().Get(), 0, nullptr, &cbChangesEveryFrame, 0, 0);

            // Renders a triangle
            m_immediateContext->VSSetShader(iterator->second->GetVertexShader().Get(), nullptr, 0);
            m_immediateContext->VSSetConstantBuffers(0, 1, m_camera.GetConstantBuffer().GetAddressOf());
            m_immediateContext->VSSetConstantBuffers(1, 1, m_cbChangeOnResize.GetAddressOf());
            m_immediateContext->VSSetConstantBuffers(2, 1, iterator->second->GetConstantBuffer().GetAddressOf());
            m_immediateContext->PSSetShader(iterator->second->GetPixelShader().Get(), nullptr, 0);
            m_immediateContext->PSSetConstantBuffers(2, 1, iterator->second->GetConstantBuffer().GetAddressOf());
            
            if (iterator->second->HasTexture())
            {
                m_immediateContext->PSSetShaderResources(0, 1, iterator->second->GetTextureResourceView().GetAddressOf());
                m_immediateContext->PSSetSamplers(0, 1, iterator->second->GetSamplerState().GetAddressOf());
            }

            m_immediateContext->DrawIndexed(iterator->second->GetNumIndices(), 0, 0);        // 36 vertices needed for 12 triangles in a triangle list
        }
        m_swapChain->Present(0, 0);
    }
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::SetVertexShaderOfRenderable

      Summary:  Sets the vertex shader for a renderable

      Args:     PCWSTR pszRenderableName
                  Key of the renderable
                PCWSTR pszVertexShaderName
                  Key of the vertex shader

      Modifies: [m_renderables].

      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderer::SetVertexShaderOfRenderable definition (remove the comment)
    --------------------------------------------------------------------*/
    HRESULT Renderer::SetVertexShaderOfRenderable(_In_ PCWSTR pszRenderableName, _In_ PCWSTR pszVertexShaderName)
    {
        if (m_renderables.find(pszRenderableName) == m_renderables.end())
            return E_FAIL;

        if (m_vertexShaders.find(pszVertexShaderName) == m_vertexShaders.end())
            return E_FAIL;

        m_renderables.at(pszRenderableName)->SetVertexShader(m_vertexShaders.at(pszVertexShaderName));
        return S_OK;
    }
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::SetPixelShaderOfRenderable

      Summary:  Sets the pixel shader for a renderable

      Args:     PCWSTR pszRenderableName
                  Key of the renderable
                PCWSTR pszPixelShaderName
                  Key of the pixel shader

      Modifies: [m_renderables].

      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderer::SetPixelShaderOfRenderable definition (remove the comment)
    --------------------------------------------------------------------*/
    HRESULT Renderer::SetPixelShaderOfRenderable(_In_ PCWSTR pszRenderableName, _In_ PCWSTR pszPixelShaderName)
    {
        if (m_renderables.find(pszRenderableName) == m_renderables.end())
            return E_FAIL;

        if (m_pixelShaders.find(pszPixelShaderName) == m_pixelShaders.end())
            return E_FAIL;

        m_renderables.at(pszRenderableName)->SetPixelShader(m_pixelShaders.at(pszPixelShaderName));
        return S_OK;
    }
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::GetDriverType

      Summary:  Returns the Direct3D driver type

      Returns:  D3D_DRIVER_TYPE
                  The Direct3D driver type used
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderer::GetDriverType definition (remove the comment)
    --------------------------------------------------------------------*/
    D3D_DRIVER_TYPE Renderer::GetDriverType() const
    {
        return m_driverType;
    }
}