#ifndef WINDOW_CPP_DEFINED
#define WINDOW_CPP_DEFINED

#include "Window.h"

#include "../../D3D12/Base/D3D12Device.h"
#include "../../D3D12/Base/D3D12SwapChain.h"
#include "../../Common/Common.h"
#include "Device.h"
#include "SwapChain.h"

Window::Window(int _width, int _height, const WString& _title, bool _isFullScreen ) :
    m_width(_width), m_height(_height), m_title(_title),
    m_hInstance(GetModuleHandle(nullptr)), m_isFullScreen(_isFullScreen)
{
    
}

Window::~Window()
{
    if (m_hWnd)
        DestroyWindow(m_hWnd);
    
    delete m_pSwapChain;
    delete m_pDevice;
}

bool Window::Create(int _width, int _height, const WString& _title)
{
    m_width = _width;
    m_height = _height;
    m_title = _title;
    m_hInstance = GetModuleHandle(nullptr);
    
    if (!RegisterWindowClass())
        return false;

    DWORD windowStyle;
    int posX, posY, windowWidth, windowHeight;

    m_windowedRect = {0, 0, m_width, m_height};
    
    if (m_isFullScreen)
    {
        // Get the primary monitor's full resolution
        windowWidth  = GetSystemMetrics(SM_CXSCREEN);
        windowHeight = GetSystemMetrics(SM_CYSCREEN);
        posX = 0;
        posY = 0;
        windowStyle = WS_POPUP | WS_VISIBLE; // No borders/titlebar
    }
    else
    {
        AdjustWindowRect(&m_windowedRect, WS_OVERLAPPEDWINDOW, FALSE);
        windowWidth  = m_windowedRect.right - m_windowedRect.left;
        windowHeight = m_windowedRect.bottom - m_windowedRect.top;
        posX = CW_USEDEFAULT;
        posY = CW_USEDEFAULT;
        windowStyle = WS_OVERLAPPEDWINDOW;
    }

    m_hWnd = CreateWindowEx(
        m_isFullScreen ? WS_EX_APPWINDOW : 0,
        L"Window",
        m_title.c_str(),
        windowStyle,
        posX, posY,
        windowWidth, windowHeight,
        nullptr, nullptr,
        m_hInstance, this
    );

    if (m_hWnd == nullptr)
        return false;

    ShowWindow(m_hWnd, SW_SHOW);
    UpdateWindow(m_hWnd);

    m_isOpen = true;
    return true;
}

bool Window::InitD3D12()
{
    Create(m_width, m_height, m_title);
    
    D3D12Device* device = new D3D12Device();
    
    if (device->Init() == false)
        return false;
    
    m_pDevice = device;
    m_pDevice->SetViewport(m_width, m_height);
    D3D12SwapChain* newSwapChain = new D3D12SwapChain(m_width, m_height, &device->m_pContext, m_hWnd);
    m_pSwapChain = newSwapChain;

    device->InitBuffer();

    m_isOpen = true;
    
    return true;
}

void Window::ToggleFullScreen()
{
    m_isFullScreen = !m_isFullScreen;

    if (m_isFullScreen)
    {
        // Save windowed state to restore later
        GetWindowRect(m_hWnd, &m_windowedRect);

        SetWindowLong(m_hWnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
        SetWindowPos(m_hWnd, HWND_TOP, 0, 0,
            GetSystemMetrics(SM_CXSCREEN),
            GetSystemMetrics(SM_CYSCREEN),
            SWP_FRAMECHANGED);
    }
    else
    {
        SetWindowLong(m_hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
        SetWindowPos(m_hWnd, nullptr,
            m_windowedRect.left, m_windowedRect.top,
            m_windowedRect.right  - m_windowedRect.left,
            m_windowedRect.bottom - m_windowedRect.top,
            SWP_FRAMECHANGED);
        ShowWindow(m_hWnd, SW_RESTORE);
    }
}

void Window::Update()
{
    MSG msg;
    while (PeekMessageW(&msg, m_hWnd, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
        {
            
            break;
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}

Device* Window::Clear()
{
    assert(m_pDevice != nullptr && "Can't draw without device");
    assert(m_pSwapChain != nullptr && "Can't draw without swapchain");

    m_pDevice->BeginDraw(m_pSwapChain->GetBackBuffer(), m_pSwapChain->GetDepthStencil());
    // m_pDevice->BeginUIDraw();

    return m_pDevice;
}

Device* Window::Clear(XMFLOAT3 _color)
{
    assert(m_pDevice != nullptr && "Can't draw without device");
    assert(m_pSwapChain != nullptr && "Can't draw without swapchain");

    m_pDevice->SetClearColor(_color);
    m_pDevice->BeginDraw(m_pSwapChain->GetBackBuffer(), m_pSwapChain->GetDepthStencil());

    return m_pDevice;
}

void Window::Display()
{
    m_pDevice->EndDraw();
    // m_pDevice->EndUIDraw();

    m_pSwapChain->Present();
}

void Window::OnResize()
{
    if (m_pDevice != nullptr)
        m_pDevice->SetViewport(m_width, m_height);
    if (m_pSwapChain != nullptr)
        m_pSwapChain->Resize(m_width, m_height);
}

void Window::Close()
{
    m_isOpen = false;
}

void Window::SetSize(int _width, int _height)
{
    m_width = _width;
    m_height = _height;

    SetWindowPos(m_hWnd, nullptr, 0, 0, _width, _height, SWP_SHOWWINDOW);

    OnResize();
}

float Window::GetAspectRatio()
{
    return MathHelper::Min(10.0f, (float)m_width / (float)m_height);
}

LRESULT Window::HandleMessage(UINT _msg, WPARAM _wParam, LPARAM _lParam)
{
    switch (_msg) {
        case WM_DESTROY:
            m_isOpen = false;
            PostQuitMessage(0);
            return 0;
                
        case WM_CLOSE:
            DestroyWindow(m_hWnd);
            return 0;
        
        case WM_SIZE:
            if (_wParam != SIZE_MINIMIZED) {
                m_width = LOWORD(_lParam);
                m_height = HIWORD(_lParam);
            }
            return 0;

        case WM_EXITSIZEMOVE:
            OnResize();
            return 0;
        
        case WM_KEYDOWN:
            return 0;
    }
    
    return DefWindowProc(m_hWnd, _msg, _wParam, _lParam);
}

LRESULT CALLBACK Window::WindowProc(HWND _hwnd, UINT _msg, WPARAM _wParam, LPARAM _lParam)
{
    Window* pWindow = nullptr;

    if (_msg == WM_NCCREATE) {
        CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(_lParam);
        pWindow = reinterpret_cast<Window*>(pCreate->lpCreateParams);
            
        SetWindowLongPtr(_hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));
        pWindow->m_hWnd = _hwnd;
    }
    else {
        pWindow = reinterpret_cast<Window*>(GetWindowLongPtr(_hwnd, GWLP_USERDATA));
    }
        
    if (pWindow) {
        return pWindow->HandleMessage(_msg, _wParam, _lParam);
    }
        
    return DefWindowProc(_hwnd, _msg, _wParam, _lParam);
}

bool Window::RegisterWindowClass()
{
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = m_hInstance;
    wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = L"Window";
    wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

    if (!RegisterClassEx(&wc))
    {
        DWORD error = GetLastError();
        if (error != ERROR_CLASS_ALREADY_EXISTS)
        {
            return false;
        }
    }

    return true;
}


#endif
