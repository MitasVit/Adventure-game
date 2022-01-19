// Game1.cpp : Definuje vstupní bod pro aplikaci.
//
#pragma warning(disable : 4996)// acess deprecated
#pragma warning(disable : 4244) // float to long/int
#include <windows.h>

#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <wchar.h>
#include <math.h>

#include <d2d1.h>
#include <d2d1helper.h>

#include <dwrite.h>
#include <wincodec.h>
#include <thread>
#include  <io.h>
#include  <stdio.h>
#include <string>
#include <fstream>

#pragma comment(lib, "d2d1.lib")

#include "Resource.h"
#include "Game1.h"

using namespace std;

template<class Interface>
inline void SafeRelease(
    Interface** ppInterfaceToRelease
)
{
    if (*ppInterfaceToRelease != NULL)
    {
        (*ppInterfaceToRelease)->Release();

        (*ppInterfaceToRelease) = NULL;
    }
}


#ifndef Assert
#if defined( DEBUG ) || defined( _DEBUG )
#define Assert(b) do {if (!(b)) {OutputDebugStringA("Assert: " #b "\n");}} while(0)
#else
#define Assert(b)
#endif //DEBUG || _DEBUG
#endif



#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

HWND shared;
int x=2, y=2;
int mom_postava = 1;
//y,x
bool zed[10][17] = { {true, true, true, true, false, false, false,false, false,false,false,false,false,false,false,false,false},
                {false, false, false, true,false, false, false,false, false,false,false,false,false,false,false,false,false},
                {false, false, false, true,false, false, false,false, false,false,false,false,false,false,false,false,false},
                {false, false, false, false,false, false, false,false, false,false,false,false,false,false,false,false,false},
                {false, false, false, false,false, false, false,false, false,false,false,false,false,false,false,false,false},
                {false, false, false, true,false, false, false,false, false,false,false,false,false,false,false,false,false},
                {false, false, false, true,false, false, false,false, false,false,false,false,false,false,false,false,false},
                {false, false, false, true,false, false, false,false, false,false,false,false,false,false,false,false,false},
                {false, false, false, false,false, false, false,false, false,false,false,false,false,false,false,false,false},
                {false, false, false, false,false, false, false,false, false,false,false,false,false,false,false,false,false}, };
enum typ_podlahy {
    trava = 1,
    kamen = 2,
    nic = 3
};
//y, x
typ_podlahy podlaha[10][17] = { {nic,nic,nic,nic,trava,trava,kamen,kamen,kamen,kamen, kamen,kamen,kamen,kamen,kamen,kamen,kamen},
                {trava, trava,trava,nic, trava, trava, kamen, kamen, kamen,trava, kamen,kamen,kamen,kamen,kamen,kamen,kamen},
                {trava, trava, kamen, nic, trava, trava, kamen, kamen, trava, trava, kamen,kamen,kamen,kamen,kamen,kamen,kamen},
                {trava, kamen, trava, trava, trava, kamen, kamen,trava,trava,trava, kamen,kamen,kamen,kamen,kamen,kamen,kamen},
                {trava, trava, kamen, trava, trava, trava, kamen, kamen, trava, trava, kamen,kamen,kamen,kamen,kamen,kamen,kamen},
                {trava, trava, kamen, nic, trava, trava, kamen, kamen, kamen, kamen, kamen,kamen,kamen,kamen,kamen,kamen,kamen},
                {trava, trava, kamen, nic, trava, trava, kamen, trava, kamen, kamen, kamen,kamen,kamen,kamen,kamen,kamen,kamen},
                {trava, kamen, kamen, nic, kamen, kamen, trava, trava, trava, kamen, kamen,kamen,kamen,kamen,kamen,kamen,kamen},
                {kamen, kamen, kamen, kamen, kamen, kamen, kamen, trava, kamen, kamen, kamen,kamen,kamen,kamen,kamen,kamen,kamen},
                {kamen, kamen, kamen, kamen, kamen, kamen, kamen, kamen, kamen, kamen, kamen,kamen,kamen,kamen,kamen,kamen,kamen} };

bool Collision(D2D1_RECT_F sprite1, D2D1_RECT_F sprite2, int width, int height)
{
    RECT rect1;
    rect1.left = sprite1.left + 1;
    rect1.top = sprite1.top + 1;
    rect1.right = sprite1.left + width - 1;
    rect1.bottom = sprite1.top + height - 1;

    RECT rect2;
    rect2.left = sprite2.left + 1;
    rect2.top = sprite2.top + 1;
    rect2.right = sprite2.left + width - 1;
    rect2.bottom = sprite2.top + height - 1;

    RECT dest;
    return IntersectRect(&dest, &rect1, &rect2);
}

inline bool exist_read_file(const std::string& name) {
    return (access(name.c_str(), 4) != -1);
}


class DemoApp
{
public:
    DemoApp();
    ~DemoApp();

    // Register the window class and call methods for instantiating drawing resources
    HRESULT Initialize();

    // Process and dispatch messages
    void RunMessageLoop();

private:
    // Initialize device-independent resources.
    HRESULT CreateDeviceIndependentResources();

    // Initialize device-dependent resources.
    HRESULT CreateDeviceResources();

    // Release device-dependent resource.
    void DiscardDeviceResources();

    // Draw content.
    HRESULT OnRender();

    // Resize the render target.
    void OnResize(
        UINT width,
        UINT height
    );

    HRESULT LoadResourceBitmap(
        ID2D1RenderTarget* pRenderTarget,
        IWICImagingFactory* pIWICFactory,
        PCWSTR resourceName,
        PCWSTR resourceType,
        UINT destinationWidth,
        UINT destinationHeight,
        ID2D1Bitmap** ppBitmap
    );

    HRESULT LoadBitmapFromFile(
        ID2D1RenderTarget* pRenderTarget,
        IWICImagingFactory* pIWICFactory,
        PCWSTR uri,
        UINT destinationWidth,
        UINT destinationHeight,
        ID2D1Bitmap** ppBitmap
    );

    // The windows procedure.
    static LRESULT CALLBACK WndProc(
        HWND hWnd,
        UINT message,
        WPARAM wParam,
        LPARAM lParam
    );
private:
    HWND m_hwnd;
    ID2D1Factory* m_pDirect2dFactory;
    ID2D1HwndRenderTarget* m_pRenderTarget;
    ID2D1SolidColorBrush* m_pLightSlateGrayBrush;
    ID2D1SolidColorBrush* m_pCornflowerBlueBrush;
    IWICImagingFactory* m_pWICFactory;
    ID2D1SolidColorBrush* m_pBrush1;

    ID2D1Bitmap* postava, *podlaha_kamen, *zed_nah/*nahore*/;
    ID2D1Bitmap* pos1, * pos2, * pos3, * pos4, *kamen;//postavy
    ID2D1Bitmap* trava;
};

DemoApp::DemoApp() :
    m_hwnd(NULL),
    m_pDirect2dFactory(NULL),
    m_pRenderTarget(NULL),
    m_pLightSlateGrayBrush(NULL),
    m_pCornflowerBlueBrush(NULL),
    m_pBrush1(NULL),
    postava(NULL),
    m_pWICFactory(NULL),
    podlaha_kamen(NULL),
    zed_nah(NULL),
    pos1(NULL), 
    pos2(NULL), 
    pos3(NULL), 
    pos4(NULL),
    kamen(NULL),
    trava(NULL)
{
}


DemoApp::~DemoApp()
{
    SafeRelease(&m_pDirect2dFactory);
    SafeRelease(&m_pRenderTarget);
    SafeRelease(&m_pLightSlateGrayBrush);
    SafeRelease(&m_pCornflowerBlueBrush);
    SafeRelease(&m_pBrush1);
    SafeRelease(&postava);
    SafeRelease(&m_pWICFactory);
    SafeRelease(&podlaha_kamen);
    SafeRelease(&zed_nah);
    SafeRelease(&pos1);
    SafeRelease(&pos2);
    SafeRelease(&pos3);
    SafeRelease(&pos4);
    SafeRelease(&kamen);
    SafeRelease(&trava);
}


HRESULT DemoApp::LoadResourceBitmap(
    ID2D1RenderTarget* pRenderTarget,
    IWICImagingFactory* pIWICFactory,
    PCWSTR resourceName,
    PCWSTR resourceType,
    UINT destinationWidth,
    UINT destinationHeight,
    ID2D1Bitmap** ppBitmap
)
{
    HRESULT hr = S_OK;
    IWICBitmapDecoder* pDecoder = NULL;
    IWICBitmapFrameDecode* pSource = NULL;
    IWICStream* pStream = NULL;
    IWICFormatConverter* pConverter = NULL;
    IWICBitmapScaler* pScaler = NULL;

    HRSRC imageResHandle = NULL;
    HGLOBAL imageResDataHandle = NULL;
    void* pImageFile = NULL;
    DWORD imageFileSize = 0;

    // Locate the resource.
    imageResHandle = FindResourceW(HINST_THISCOMPONENT, resourceName, resourceType);

    hr = imageResHandle ? S_OK : E_FAIL;
    if (SUCCEEDED(hr))
    {
        // Load the resource.
        imageResDataHandle = LoadResource(HINST_THISCOMPONENT, imageResHandle);

        hr = imageResDataHandle ? S_OK : E_FAIL;
    }
    if (SUCCEEDED(hr))
    {
        // Lock it to get a system memory pointer.
        pImageFile = LockResource(imageResDataHandle);

        hr = pImageFile ? S_OK : E_FAIL;
    }
    if (SUCCEEDED(hr))
    {
        // Calculate the size.
        imageFileSize = SizeofResource(HINST_THISCOMPONENT, imageResHandle);

        hr = imageFileSize ? S_OK : E_FAIL;
    }
    if (SUCCEEDED(hr))
    {
        // Create a WIC stream to map onto the memory.
        hr = pIWICFactory->CreateStream(&pStream);
    }
    if (SUCCEEDED(hr))
    {
        // Initialize the stream with the memory pointer and size.
        hr = pStream->InitializeFromMemory(
            reinterpret_cast<BYTE*>(pImageFile),
            imageFileSize
        );
    }
    if (SUCCEEDED(hr))
    {
        // Create a decoder for the stream.
        hr = pIWICFactory->CreateDecoderFromStream(
            pStream,
            NULL,
            WICDecodeMetadataCacheOnLoad,
            &pDecoder
        );
    }
    if (SUCCEEDED(hr))
    {
        // Create the initial frame.
        hr = pDecoder->GetFrame(0, &pSource);
    }
    if (SUCCEEDED(hr))
    {
        // Convert the image format to 32bppPBGRA
        // (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
        hr = pIWICFactory->CreateFormatConverter(&pConverter);
    }
    if (SUCCEEDED(hr))
    {
        // If a new width or height was specified, create an
        // IWICBitmapScaler and use it to resize the image.
        if (destinationWidth != 0 || destinationHeight != 0)
        {
            UINT originalWidth, originalHeight;
            hr = pSource->GetSize(&originalWidth, &originalHeight);
            if (SUCCEEDED(hr))
            {
                if (destinationWidth == 0)
                {
                    FLOAT scalar = static_cast<FLOAT>(destinationHeight) / static_cast<FLOAT>(originalHeight);
                    destinationWidth = static_cast<UINT>(scalar * static_cast<FLOAT>(originalWidth));
                }
                else if (destinationHeight == 0)
                {
                    FLOAT scalar = static_cast<FLOAT>(destinationWidth) / static_cast<FLOAT>(originalWidth);
                    destinationHeight = static_cast<UINT>(scalar * static_cast<FLOAT>(originalHeight));
                }

                hr = pIWICFactory->CreateBitmapScaler(&pScaler);
                if (SUCCEEDED(hr))
                {
                    hr = pScaler->Initialize(
                        pSource,
                        destinationWidth,
                        destinationHeight,
                        WICBitmapInterpolationModeCubic
                    );
                    if (SUCCEEDED(hr))
                    {
                        hr = pConverter->Initialize(
                            pScaler,
                            GUID_WICPixelFormat32bppPBGRA,
                            WICBitmapDitherTypeNone,
                            NULL,
                            0.f,
                            WICBitmapPaletteTypeMedianCut
                        );
                    }
                }
            }
        }
        else
        {
            hr = pConverter->Initialize(
                pSource,
                GUID_WICPixelFormat32bppPBGRA,
                WICBitmapDitherTypeNone,
                NULL,
                0.f,
                WICBitmapPaletteTypeMedianCut
            );
        }
    }
    if (SUCCEEDED(hr))
    {
        //create a Direct2D bitmap from the WIC bitmap.
        hr = pRenderTarget->CreateBitmapFromWicBitmap(
            pConverter,
            NULL,
            ppBitmap
        );
    }



    SafeRelease(&pDecoder);
    SafeRelease(&pSource);
    SafeRelease(&pStream);
    SafeRelease(&pConverter);
    SafeRelease(&pScaler);

    return hr;
}

//
// Creates a Direct2D bitmap from the specified
// file name.
//
HRESULT DemoApp::LoadBitmapFromFile(
    ID2D1RenderTarget* pRenderTarget,
    IWICImagingFactory* pIWICFactory,
    PCWSTR uri,
    UINT destinationWidth,
    UINT destinationHeight,
    ID2D1Bitmap** ppBitmap
)
{
    HRESULT hr = S_OK;

    IWICBitmapDecoder* pDecoder = NULL;
    IWICBitmapFrameDecode* pSource = NULL;
    IWICStream* pStream = NULL;
    IWICFormatConverter* pConverter = NULL;
    IWICBitmapScaler* pScaler = NULL;

    hr = pIWICFactory->CreateDecoderFromFilename(
        uri,
        NULL,
        GENERIC_READ,
        WICDecodeMetadataCacheOnLoad,
        &pDecoder
    );
    if (SUCCEEDED(hr))
    {

        // Create the initial frame.
        hr = pDecoder->GetFrame(0, &pSource);
    }

    if (SUCCEEDED(hr))
    {
        // Convert the image format to 32bppPBGRA
        // (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
        hr = pIWICFactory->CreateFormatConverter(&pConverter);
    }
    if (SUCCEEDED(hr))
    {
        // If a new width or height was specified, create an
        // IWICBitmapScaler and use it to resize the image.
        if (destinationWidth != 0 || destinationHeight != 0)
        {
            UINT originalWidth, originalHeight;
            hr = pSource->GetSize(&originalWidth, &originalHeight);
            if (SUCCEEDED(hr))
            {
                if (destinationWidth == 0)
                {
                    FLOAT scalar = static_cast<FLOAT>(destinationHeight) / static_cast<FLOAT>(originalHeight);
                    destinationWidth = static_cast<UINT>(scalar * static_cast<FLOAT>(originalWidth));
                }
                else if (destinationHeight == 0)
                {
                    FLOAT scalar = static_cast<FLOAT>(destinationWidth) / static_cast<FLOAT>(originalWidth);
                    destinationHeight = static_cast<UINT>(scalar * static_cast<FLOAT>(originalHeight));
                }

                hr = pIWICFactory->CreateBitmapScaler(&pScaler);
                if (SUCCEEDED(hr))
                {
                    hr = pScaler->Initialize(
                        pSource,
                        destinationWidth,
                        destinationHeight,
                        WICBitmapInterpolationModeCubic
                    );
                }
                if (SUCCEEDED(hr))
                {
                    hr = pConverter->Initialize(
                        pScaler,
                        GUID_WICPixelFormat32bppPBGRA,
                        WICBitmapDitherTypeNone,
                        NULL,
                        0.f,
                        WICBitmapPaletteTypeMedianCut
                    );
                }
            }
        }
        else // Don't scale the image.
        {
            hr = pConverter->Initialize(
                pSource,
                GUID_WICPixelFormat32bppPBGRA,
                WICBitmapDitherTypeNone,
                NULL,
                0.f,
                WICBitmapPaletteTypeMedianCut
            );
        }
    }
    if (SUCCEEDED(hr))
    {
        // Create a Direct2D bitmap from the WIC bitmap.
        hr = pRenderTarget->CreateBitmapFromWicBitmap(
            pConverter,
            NULL,
            ppBitmap
        );
    }

    SafeRelease(&pDecoder);
    SafeRelease(&pSource);
    SafeRelease(&pStream);
    SafeRelease(&pConverter);
    SafeRelease(&pScaler);

    return hr;
}

void DemoApp::RunMessageLoop()
{
    MSG msg;

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

HRESULT DemoApp::Initialize()
{
    HRESULT hr;

    // Initialize device-indpendent resources, such
    // as the Direct2D factory.
    hr = CreateDeviceIndependentResources();

    if (SUCCEEDED(hr))
    {
        // Register the window class.
        WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = DemoApp::WndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = sizeof(LONG_PTR);
        wcex.hInstance = HINST_THISCOMPONENT;
        wcex.hbrBackground = NULL;
        wcex.lpszMenuName = NULL;
        wcex.hCursor = LoadCursor(NULL, IDI_APPLICATION);
        wcex.hIconSm = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_GAME1));
        wcex.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_SMALL));
        wcex.lpszClassName = L"D2DDemoApp";//IDI_SMALL

        RegisterClassEx(&wcex);

        FLOAT dpiX, dpiY;

        dpiX = (FLOAT)GetDpiForWindow(GetDesktopWindow());
        dpiY = dpiX;
      


        // Create the window.
        m_hwnd = CreateWindow(
            L"D2DDemoApp",
            L"Direct2D Demo App",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            static_cast<UINT>(ceil(640.f * dpiX / 96.f)),
            static_cast<UINT>(ceil(480.f * dpiY / 96.f)),
            NULL,
            NULL,
            HINST_THISCOMPONENT,
            this
        );
        hr = m_hwnd ? S_OK : E_FAIL;
        if (SUCCEEDED(hr))
        {
            shared = m_hwnd;
            ShowWindow(m_hwnd, SW_SHOWNORMAL);
            UpdateWindow(m_hwnd);
        }
    }

    return hr;
}

int WINAPI WinMain(
    HINSTANCE /* hInstance */,
    HINSTANCE /* hPrevInstance */,
    LPSTR /* lpCmdLine */,
    int /* nCmdShow */
)
{
    HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

    if (SUCCEEDED(CoInitialize(NULL)))
    {
        {
            DemoApp app;

            if (SUCCEEDED(app.Initialize()))
            {
                app.RunMessageLoop();
            }
        }
        CoUninitialize();
    }

    return 0;
}
HRESULT DemoApp::CreateDeviceIndependentResources()
{
    HRESULT hr = S_OK;

    // Create a Direct2D factory.
    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pDirect2dFactory);

    return hr;
}

HRESULT DemoApp::CreateDeviceResources()
{
    HRESULT hr = S_OK;

    if (!m_pRenderTarget)
    {
        RECT rc;
        GetClientRect(m_hwnd, &rc);

        D2D1_SIZE_U size = D2D1::SizeU(
            rc.right - rc.left,
            rc.bottom - rc.top
        );

        // Create a Direct2D render target.
        hr = m_pDirect2dFactory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(m_hwnd, size),
            &m_pRenderTarget
        );


        if (SUCCEEDED(hr))
        {
            // Create a gray brush.
            hr = m_pRenderTarget->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF::LightSlateGray),
                &m_pLightSlateGrayBrush
            );
        }
        if (SUCCEEDED(hr))
        {
            // Create a blue brush.
            hr = m_pRenderTarget->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF::CornflowerBlue),
                &m_pCornflowerBlueBrush
            );
        }
        if (SUCCEEDED(hr))
        {
            // Create a blue brush.
            hr = m_pRenderTarget->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF::Black),
                &m_pBrush1
            );
        }
        if (SUCCEEDED(hr))
        {
            // Create WIC factory.
            hr = CoCreateInstance(
                CLSID_WICImagingFactory,
                NULL,
                CLSCTX_INPROC_SERVER,
                IID_IWICImagingFactory,
                reinterpret_cast<void**>(&m_pWICFactory)
            );
        }
        if (SUCCEEDED(hr))
        {
            // Create a bitmap by loading it from a file.
            hr = LoadResourceBitmap(
                m_pRenderTarget,
                m_pWICFactory,
                L"postava",
                L"Image",
                550,
                660,
                &postava
            );
        }
        if (SUCCEEDED(hr))
        {
            // Create a bitmap by loading it from a file.
            hr = LoadResourceBitmap(
                m_pRenderTarget,
                m_pWICFactory,
                L"podlahakamen",
                L"Image",
                500,
                500,
                &podlaha_kamen
            );
        }
        if (SUCCEEDED(hr))
        {
            // Create a bitmap by loading it from a file.
            hr = LoadResourceBitmap(
                m_pRenderTarget,
                m_pWICFactory,
                L"zed1",
                L"Image",
                500,
                500,
                &zed_nah
            );
        }
        if (SUCCEEDED(hr))
        {
            // Create a bitmap by loading it from a file.
            hr = LoadResourceBitmap(
                m_pRenderTarget,
                m_pWICFactory,
                L"po1",
                L"Image",
                500,
                500,
                &pos1
            );
        }
        if (SUCCEEDED(hr))
        {
            // Create a bitmap by loading it from a file.
            hr = LoadResourceBitmap(
                m_pRenderTarget,
                m_pWICFactory,
                L"po2",
                L"Image",
                500,
                500,
                &pos2
            );
        }
        if (SUCCEEDED(hr))
        {
            // Create a bitmap by loading it from a file.
            hr = LoadResourceBitmap(
                m_pRenderTarget,
                m_pWICFactory,
                L"po3",
                L"Image",
                500,
                500,
                &pos3
            );
        }
        if (SUCCEEDED(hr))
        {
            // Create a bitmap by loading it from a file.
            hr = LoadResourceBitmap(
                m_pRenderTarget,
                m_pWICFactory,
                L"po4",
                L"Image",
                500,
                500,
                &pos4
            );
        }
        if (SUCCEEDED(hr))
        {
            // Create a bitmap by loading it from a file.
            hr = LoadResourceBitmap(
                m_pRenderTarget,
                m_pWICFactory,
                L"kamen",
                L"Image",
                500,
                500,
                &kamen
            );
        }
        if (SUCCEEDED(hr))
        {
            // Create a bitmap by loading it from a file.
            hr = LoadResourceBitmap(
                m_pRenderTarget,
                m_pWICFactory,
                L"trava",
                L"Image",
                500,
                500,
                &trava
            );
        }

    }

    return hr;
}

void DemoApp::DiscardDeviceResources()
{
    SafeRelease(&m_pRenderTarget);
    SafeRelease(&m_pLightSlateGrayBrush);
    SafeRelease(&m_pCornflowerBlueBrush);
}

LRESULT CALLBACK DemoApp::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;

    if (message == WM_CREATE)
    {
        LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
        DemoApp* pDemoApp = (DemoApp*)pcs->lpCreateParams;

        ::SetWindowLongPtrW(
            hwnd,
            GWLP_USERDATA,
            reinterpret_cast<LONG_PTR>(pDemoApp)
        );

        result = 1;

        //nacteni mapy
        int startposx, startposy, width, height;
        if (exist_read_file("emy mapa.txt")) {
            ifstream in("emy mapa.txt");
            in >> width;
            in >> height;
            in >> startposx;
            in >> startposy;
            for (int _y = 0; _y < 10; _y++) {
                for (int _x = 0; _x < 17; _x++) {
                    int _t;
                    in >> _t;
                    if (_t == 1) {
                        zed[_y][_x] = true;
                    }
                    else {
                        zed[_y][_x] = false;
                    }
                }
            }
            for (int _y = 0; _y < 10; _y++) {
                for (int _x = 0; _x < 17; _x++) {
                    int t;
                    in >> t;
                    if (t == 1) {
                        podlaha[_y][_x] = typ_podlahy::trava;
                    }
                    else if (t == 2) {
                        podlaha[_y][_x] = typ_podlahy::kamen;
                    }
                    else {
                        podlaha[_y][_x] = typ_podlahy::nic;
                    }
                }
            }
            in.close();
        }
        SendMessage(hwnd, WM_PAINT, 0, 0);
    }
    else
    {
        DemoApp* pDemoApp = reinterpret_cast<DemoApp*>(static_cast<LONG_PTR>(
            ::GetWindowLongPtrW(
                hwnd,
                GWLP_USERDATA
            )));

        bool wasHandled = false;

        if (pDemoApp)
        {
            switch (message)
            {
            case WM_SIZE:
            {
                UINT width = LOWORD(lParam);
                UINT height = HIWORD(lParam);
                pDemoApp->OnResize(width, height);
            }
            result = 0;
            wasHandled = true;
            break;
            case WM_LBUTTONDBLCLK:
            {
                MessageBox(NULL, L"", L"", MB_OK);
            }break;
            case WM_KEYDOWN:
            {
                if (GetAsyncKeyState(0x70) & 0x8000) {
                    //key f1
                    std::wstring _1 = L"x: " + std::to_wstring(x) + L" y: " + std::to_wstring(y);
                    MessageBox(NULL, _1.c_str(), _1.c_str(),  MB_OK | MB_ICONINFORMATION);
                }
                else if ((GetAsyncKeyState(0x27) & 0x8000) || (GetAsyncKeyState(0x44) & 0x8000)) {
                    //key-right pressed | key d 
                    if (!zed[y][x+1]) {
                        x++;
                        mom_postava = 3;
                    }
                    SendMessage(hwnd, WM_PAINT, 0, 0);
                }
                else if ((GetAsyncKeyState(0x25) & 0x8000) || (GetAsyncKeyState(0x41) & 0x8000)) {
                    //key-left pressed | key a
                    if (!zed[y][x-1]) {
                        x--;
                    }
                    mom_postava = 4;
                    SendMessage(hwnd, WM_PAINT, 0, 0);
                } 
                else if ((GetAsyncKeyState(0x28) & 0x8000) || (GetAsyncKeyState(0x53) & 0x8000)){
                    //key-down pressed | key s 
                    if (!zed[y+1][x]) {
                        y++;
                    }
                    mom_postava = 1;
                    SendMessage(hwnd, WM_PAINT, 0, 0);
                }
                else if ((GetAsyncKeyState(0x26) & 0x8000) || (GetAsyncKeyState(0x57) & 0x8000)) {
                     //key-up pressed | key w 
                    if (!zed[y-1][x]) {
                        y--;
                    }
                    mom_postava = 2;
                     SendMessage(hwnd, WM_PAINT, 0, 0);
                }
                else if (GetAsyncKeyState(0x4F) & 0x8000) {
                    //o pressed - open mapa
                    OPENFILENAMEA ofn;
                    char fileName[MAX_PATH] = "";
                    ZeroMemory(&ofn, sizeof(ofn));
                    ofn.lStructSize = sizeof(OPENFILENAME);
                    ofn.hwndOwner = shared;
                    ofn.lpstrFilter = "Map files(*.txt)\0*.txt";
                    ofn.lpstrFile = fileName;
                    ofn.nMaxFile = MAX_PATH;
                    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
                    ofn.lpstrDefExt = "";

                    string fileNameStr;
                    if (GetOpenFileNameA(&ofn)) {
                        fileNameStr = fileName;
                    }
                    if (fileNameStr != "") {
                        int startposx, startposy, width, height;
                        if (exist_read_file(fileNameStr.c_str())) {
                            ifstream in(fileNameStr.c_str());
                            in >> width;
                            in >> height;
                            in >> startposx;
                            in >> startposy;
                            for (int _y = 0; _y < 10; _y++) {
                                for (int _x = 0; _x < 17; _x++) {
                                    int _t;
                                    in >> _t;
                                    if (_t == 1) {
                                        zed[_y][_x] = true;
                                    }
                                    else {
                                        zed[_y][_x] = false;
                                    }
                                }
                            }
                            for (int _y = 0; _y < 10; _y++) {
                                for (int _x = 0; _x < 17; _x++) {
                                    int t;
                                    in >> t;
                                    if (t == 1) {
                                        podlaha[_y][_x] = typ_podlahy::trava;
                                    }
                                    else if (t == 2) {
                                        podlaha[_y][_x] = typ_podlahy::kamen;
                                    }
                                    else {
                                        podlaha[_y][_x] = typ_podlahy::nic;
                                    }
                                }
                            }
                            in.close();
                        }
                        SendMessage(hwnd, WM_PAINT, 0, 0);
                    }
                }
            }break;
            case WM_DISPLAYCHANGE:
            {
                InvalidateRect(hwnd, NULL, FALSE);
            }
            result = 0;
            wasHandled = true;
            break;

            case WM_PAINT:
            {
                pDemoApp->OnRender();
                ValidateRect(hwnd, NULL);
            }
            result = 0;
            wasHandled = true;
            break;

            case WM_DESTROY:
            {
                PostQuitMessage(0);
            }
            result = 1;
            wasHandled = true;
            break;
            }
        }

        if (!wasHandled)
        {
            result = DefWindowProc(hwnd, message, wParam, lParam);
        }
    }

    return result;
}

HRESULT DemoApp::OnRender()
{
    HRESULT hr = S_OK;

    hr = CreateDeviceResources();
    if (SUCCEEDED(hr))
    {
        m_pRenderTarget->BeginDraw();

        m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

        m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
        D2D1_SIZE_F rtSize = m_pRenderTarget->GetSize();
        // Draw a grid background.
        int width = static_cast<int>(rtSize.width);
        int height = static_cast<int>(rtSize.height);

        /*for (int x = 0; x < width; x += 40)
        {
            m_pRenderTarget->DrawLine(
                D2D1::Point2F(static_cast<FLOAT>(x), 0.0f),
                D2D1::Point2F(static_cast<FLOAT>(x), rtSize.height),
                m_pLightSlateGrayBrush,
                0.5f
            );
        }

        for (int y = 0; y < height; y += 40)
        {
            m_pRenderTarget->DrawLine(
                D2D1::Point2F(0.0f, static_cast<FLOAT>(y)),
                D2D1::Point2F(rtSize.width, static_cast<FLOAT>(y)),
                m_pLightSlateGrayBrush,
                0.5f
            );
        }*/
        
        /*for (int _x = 0; _x < width; _x++) {
            for (int _y = 0; _y < height; _y++){
                D2D1_RECT_F podlaha_pos = { _x * 80,_y * 80,(_x * 80) + 80,(_y * 80) + 80 };
                m_pRenderTarget->DrawBitmap(podlaha_kamen, podlaha_pos, 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
            }
        }*/
        
       /* for (int i = 1; i < 8; i++) {
            D2D1_RECT_F pos = {400,i*80, 480, (i*80)+80};
            m_pRenderTarget->DrawBitmap(kamen, pos, 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
        }*/


        for (int x = 0; x < 17; x++) {
            for (int y = 0; y < 10; y++) {
                if (zed[y][x] == true) {
                    m_pRenderTarget->DrawBitmap(kamen, SRect(x * 80, y * 80, (x * 80) + 80, (y * 80) + 80), 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                }
                else if (podlaha[y][x] == typ_podlahy::trava) {
                    m_pRenderTarget->DrawBitmap(trava, SRect(x * 80, y * 80, (x * 80) + 80, (y * 80) + 80), 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                }
                else if (podlaha[y][x] == typ_podlahy::kamen) {
                    m_pRenderTarget->DrawBitmap(podlaha_kamen, SRect(x  * 80, y * 80, (x * 80) + 80, (y * 80) + 80), 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                }
            }
        }


        D2D1_RECT_F postava_pos = D2D1::RectF(x * 80, y * 80, (x * 80) + 80, (y * 80) + 80);
        if (mom_postava == 1) {
            m_pRenderTarget->DrawBitmap(pos1, postava_pos, 1.0);
        }
        else if (mom_postava == 2) {
            m_pRenderTarget->DrawBitmap(pos2, postava_pos, 1.0);
        }
        else if (mom_postava == 3) {
            m_pRenderTarget->DrawBitmap(pos3, postava_pos, 1.0);
        }
        else if (mom_postava == 4) {
            m_pRenderTarget->DrawBitmap(pos4, postava_pos, 1.0);
        }
       /* m_pRenderTarget->DrawBitmap(postava, postava_pos, 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
        */


        hr = m_pRenderTarget->EndDraw();
    }
    if (hr == D2DERR_RECREATE_TARGET)
    {
        hr = S_OK;
        DiscardDeviceResources();
    }
    return hr;
}

void DemoApp::OnResize(UINT width, UINT height)
{
    if (m_pRenderTarget)
    {
        // Note: This method can fail, but it's okay to ignore the
        // error here, because the error will be returned again
        // the next time EndDraw is called.
        m_pRenderTarget->Resize(D2D1::SizeU(width, height));
    }
}