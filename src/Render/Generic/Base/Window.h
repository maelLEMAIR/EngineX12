#ifndef WINDOW_H_DEFINED
#define WINDOW_H_DEFINED

#include "../../Common/Common.h"
#include <windows.h>

class SwapChain;
class Device;

class Window 
{
public:
    bool IsOpen() { return m_isOpen; }
    HWND GetHWND() { return m_hWnd; }
    void Close();
    
    HWND GetHandle()    { return m_hWnd; }
    
    float GetAspectRatio();
    int GetWidth()      { return m_width; }
    int GetHeight()     { return m_height; }
    void SetSize(int _width, int _height);
    void OnResize();
    
    bool Create(int _width, int _height, const WString& _title);
    bool InitD3D12();
    void ToggleFullScreen();
    void Update();

    Device* GetDevice() { return m_pDevice; }
    SwapChain* GetSwapChain() { return m_pSwapChain; }

    Device* Clear();
    Device* Clear(XMFLOAT3 _color);
    void Display();

    Window() = default;
    Window(int _width, int _height, const WString& _title, bool _isFullScreen = false);
    virtual ~Window();
protected:    
    HINSTANCE m_hInstance;
    HWND m_hWnd = nullptr;
    
    bool m_isOpen = false;

    int m_width;
    int m_height;

    Device* m_pDevice = nullptr;
    SwapChain* m_pSwapChain = nullptr;

    WString m_title;
    bool m_isFullScreen;
    tagRECT m_windowedRect;

    virtual LRESULT HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam);
    
private:
    static LRESULT CALLBACK WindowProc(HWND _hwnd, UINT _msg, WPARAM _wParam, LPARAM _lParam);
    bool RegisterWindowClass();
    
};
#endif
