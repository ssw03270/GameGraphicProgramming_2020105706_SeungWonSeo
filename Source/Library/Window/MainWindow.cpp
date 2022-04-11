#include "Window/MainWindow.h"

namespace library
{
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   MainWindow::Initialize

      Summary:  Initializes main window

      Args:     HINSTANCE hInstance
                  Handle to the instance
                INT nCmdShow
                  Is a flag that says whether the main application window
                  will be minimized, maximized, or shown normally
                PCWSTR pszWindowName
                  The window name

      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: MainWindow::Initialize definition (remove the comment)
    --------------------------------------------------------------------*/
    HRESULT MainWindow::Initialize(_In_ HINSTANCE hInstance, _In_ INT nCmdShow, _In_ PCWSTR pszWindowName)
    {
        static bool raw_input_initialized = false;
        if (raw_input_initialized == false)
        {
            RAWINPUTDEVICE rid;

            rid.usUsagePage = 0x01; //Mouse
            rid.usUsage = 0x02;
            rid.dwFlags = 0;
            rid.hwndTarget = NULL;

            if (RegisterRawInputDevices(&rid, 1, sizeof(rid)) == FALSE)
            {
                exit(-1);
            }

            raw_input_initialized = true;
        }

        RECT rc = { 0, 0, 800, 600 };
        AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

        HRESULT hr = initialize(hInstance, nCmdShow, pszWindowName, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr);
        if (FAILED(hr))
        {
            return hr;
        }

        return S_OK;
    }
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   MainWindow::GetWindowClassName

      Summary:  Returns the name of the window class

      Returns:  PCWSTR
                  Name of the window class
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: MainWindow::GetWindowClassName definition (remove the comment)
    --------------------------------------------------------------------*/
    PCWSTR MainWindow::GetWindowClassName() const
    {
        return L"Sample window Class";
    }
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   MainWindow::HandleMessage

      Summary:  Handles the messages

      Args:     UINT uMessage
                  Message code
                WPARAM wParam
                  Additional data the pertains to the message
                LPARAM lParam
                  Additional data the pertains to the message

      Returns:  LRESULT
                  Integer value that your program returns to Windows
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: MainWindow::HandleMessage definition (remove the comment)
    --------------------------------------------------------------------*/
    LRESULT MainWindow::HandleMessage(_In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
    {
        PAINTSTRUCT ps;
        HDC hdc;

        switch (uMsg)
        {
        case WM_PAINT:
            hdc = BeginPaint(m_hWnd, &ps);
            EndPaint(m_hWnd, &ps);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

            // Note that this tutorial does not handle resizing (WM_SIZE) requests,
            // so we created the window without the resize border.
        case WM_INPUT:
            UINT dataSize;
            GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, NULL, &dataSize, sizeof(RAWINPUTHEADER)); //Need to populate data size first
            //std::cout << GET_RAWINPUT_CODE_WPARAM(wParam) << " code thing\n";
            if (dataSize > 0)
            {
                std::vector<BYTE> rawdata(dataSize);

                if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, rawdata.data(), &dataSize, sizeof(RAWINPUTHEADER)) == dataSize)
                {
                    RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(rawdata.data());
                    if (raw->header.dwType == RIM_TYPEMOUSE)
                    {
                        m_mouseRelativeMovement.X = raw->data.mouse.lLastX;
                        m_mouseRelativeMovement.Y = raw->data.mouse.lLastY;
                    }
                }
            }
            RECT rc;
            POINT p1, p2;

            GetClientRect(m_hWnd, &rc);    // 클라이언트 크기

            // 클라이언트 크기를 좌표로 변환
            p1.x = rc.left;
            p1.y = rc.top;
            p2.x = rc.right;
            p2.y = rc.bottom;

            // 클라이언트 크기를 스크린 크기로 변환
            ClientToScreen(m_hWnd, &p1);
            ClientToScreen(m_hWnd, &p2);

            rc.left = p1.x;
            rc.top = p1.y;
            rc.right = p2.x;
            rc.bottom = p2.y;

            //해당 좌표를 기준으로 커서를 고정
            ClipCursor(&rc);
            break;
        case WM_KEYDOWN:
            if (wParam == 0x44)
                m_directions.bRight = true;
            if (wParam == 0x53)
                m_directions.bBack = true;
            if (wParam == 0x41)
                m_directions.bLeft = true;
            if (wParam == 0x57)
                m_directions.bFront = true;
            if (wParam == VK_SHIFT)
                m_directions.bDown = true;
            if (wParam == VK_SPACE)
                m_directions.bUp = true;
            break;

        case WM_KEYUP:
            if (wParam == 0x44)
                m_directions.bRight = false;
            if (wParam == 0x53)
                m_directions.bBack = false;
            if (wParam == 0x41)
                m_directions.bLeft = false;
            if (wParam == 0x57)
                m_directions.bFront = false;
            if (wParam == VK_SHIFT)
                m_directions.bDown = false;
            if (wParam == VK_SPACE)
                m_directions.bUp = false;
            break;

        default:
            return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
        }

        return 0;
    }
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   MainWindow::GetDirections

      Summary:  Returns the keyboard direction input

      Returns:  const DirectionsInput&
                  Keyboard direction input
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: MainWindow::GetDirections definition (remove the comment)
    --------------------------------------------------------------------*/
    const DirectionsInput& MainWindow::GetDirections() const
    {
        return m_directions;
    }
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   MainWindow::GetMouseRelativeMovement

      Summary:  Returns the mouse relative movement

      Returns:  const MouseRelativeMovement&
                  Mouse relative movement
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: MainWindow::GetMouseRelativeMovement definition (remove the comment)
    --------------------------------------------------------------------*/
    const MouseRelativeMovement& MainWindow::GetMouseRelativeMovement() const
    {
        return m_mouseRelativeMovement;
    }
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   MainWindow::ResetMouseMovement

      Summary:  Reset the mouse relative movement to zero
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: MainWindow::ResetMouseMovement definition (remove the comment)
    --------------------------------------------------------------------*/
    void MainWindow::ResetMouseMovement()
    {
        m_mouseRelativeMovement.X = 0;
        m_mouseRelativeMovement.Y = 0;
    }
}
