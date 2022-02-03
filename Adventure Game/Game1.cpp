// Game1.cpp : Definuje vstupní bod pro aplikaci.
//
#pragma warning(disable : 4996)// acess deprecated
#pragma warning(disable : 4244) // float to long/int
#pragma warning(disable : 28251) // nekonzistentni poznamka u WinMain
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

#include <shellapi.h>
#include <atlstr.h>

#pragma comment(lib, "Shell32.lib")
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


//#define POSOUVANI_KAMERY

//sebr_"prvni pismeno barvy""[k]krystal [o]orb"
int sebr_mk = 0, sebr_ck = 0, sebr_ok = 0, sebr_zk = 0, sebr_zlk = 0;
int sebr_mo = 0, sebr_co = 0, sebr_oo = 0, sebr_zo = 0, sebr_zlo = 0;

HWND shared;
int x=2, y=2;
int mom_postava = 1;

enum typ_zdi {
    kamen_=1,
    kamen_o=2,
    nic__ = 3
};
enum typ_podlahy {
    trava = 1,
    kamen = 2,
    nic = 3
};
enum typ_predmetu {
    krystal_m = 1,/*modry*/
    krystal_c = 2,/*cerveny*/
    krystal_z = 4,/*zeleny*/
    krystal_o = 5, /*oranzovy*/
    krystal_zl = 6,
    //orby
    orb_m = 7,/*modry*/
    orb_c = 8,/*cerveny*/
    orb_z = 9,/*zeleny*/
    orb_o = 10, /*oranzovy*/
    orb_zl = 11,/*zluty*/
    nic_ = 3
};

//y,x
typ_zdi zed[10][17] = { {nic__, nic__, nic__, nic__, nic__, nic__, nic__,nic__, nic__,nic__,nic__,nic__,nic__,nic__,nic__,nic__,nic__},
               {nic__, nic__, nic__, nic__, nic__, nic__, nic__,nic__, nic__,nic__,nic__,nic__,nic__,nic__,nic__,nic__,nic__},
               {nic__, nic__, nic__, nic__, nic__, nic__, nic__,nic__, nic__,nic__,nic__,nic__,nic__,nic__,nic__,nic__,nic__},
                {nic__, nic__, nic__, nic__, nic__, nic__, nic__,nic__, nic__,nic__,nic__,nic__,nic__,nic__,nic__,nic__,nic__},
               {nic__, nic__, nic__, nic__, nic__, nic__, nic__,nic__, nic__,nic__,nic__,nic__,nic__,nic__,nic__,nic__,nic__},
              {nic__, nic__, nic__, nic__, nic__, nic__, nic__,nic__, nic__,nic__,nic__,nic__,nic__,nic__,nic__,nic__,nic__},
               {nic__, nic__, nic__, nic__, nic__, nic__, nic__,nic__, nic__,nic__,nic__,nic__,nic__,nic__,nic__,nic__,nic__},
                {nic__, nic__, nic__, nic__, nic__, nic__, nic__,nic__, nic__,nic__,nic__,nic__,nic__,nic__,nic__,nic__,nic__}, 
                {nic__, nic__, nic__, nic__, nic__, nic__, nic__,nic__, nic__,nic__,nic__,nic__,nic__,nic__,nic__,nic__,nic__}, };

//y, x
typ_podlahy podlaha[10][17] = { {nic,nic,nic,nic,trava,trava,kamen,kamen,kamen,kamen, kamen,kamen,kamen,kamen,kamen,kamen,kamen},
                {trava, trava,trava,nic, trava, trava, kamen, kamen, kamen,trava, kamen,kamen,kamen,kamen,kamen,kamen,kamen},
                {trava, trava, kamen, nic, trava, trava, kamen, kamen, trava, trava, kamen,kamen,kamen,kamen,kamen,kamen,kamen},
                {trava, kamen, trava, trava, trava, kamen, kamen,trava,trava,trava, kamen,kamen,kamen,kamen,kamen,kamen,kamen},
                {trava, trava, kamen, trava, trava, trava, kamen, kamen, trava, trava, kamen,kamen,kamen,kamen,kamen,kamen,kamen},
                {trava, trava, kamen, nic, trava, kamen, kamen, kamen, kamen, kamen, kamen,kamen,kamen,kamen,kamen,kamen,kamen},
                {trava, trava, kamen, nic, trava, trava, kamen, trava, kamen, kamen, kamen,kamen,kamen,kamen,kamen,kamen,kamen},
                {trava, kamen, kamen, nic, kamen, kamen, trava, trava, trava, kamen, kamen,kamen,kamen,kamen,kamen,kamen,kamen},
                {kamen, kamen, kamen, kamen, kamen, kamen, kamen, trava, kamen, kamen, kamen,kamen,kamen,kamen,kamen,kamen,kamen},
                {kamen, kamen, kamen, kamen, kamen, kamen, kamen, kamen, kamen, kamen, kamen,kamen,kamen,kamen,kamen,kamen,kamen} };
typ_predmetu predmety[10][17] = { {nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_, nic_,nic_,nic_,nic_,nic_,nic_,nic_},
{nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_, nic_,nic_,nic_,nic_,nic_,nic_,nic_},
{nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_, nic_,nic_,nic_,nic_,nic_,nic_,nic_},
{nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_, nic_,nic_,nic_,nic_,nic_,nic_,nic_}, 
{nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_, nic_,nic_,nic_,nic_,nic_,nic_,nic_}, 
{nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_, nic_,nic_,nic_,nic_,nic_,nic_,nic_}, 
{nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_, nic_,nic_,nic_,nic_,nic_,nic_,nic_}, 
{nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_, nic_,nic_,nic_,nic_,nic_,nic_,nic_}, 
{nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_, nic_,nic_,nic_,nic_,nic_,nic_,nic_}, 
{nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_, nic_,nic_,nic_,nic_,nic_,nic_,nic_}};

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
    ID2D1Bitmap* pos1, * pos2, * pos3, * pos4, *kamen, *kamen2;//postavy
    ID2D1Bitmap* trava, * krystal_m, * krystal_c, * krystal_z, * krystal_o, *krystal_zl;
    ID2D1Bitmap* orb_m, * orb_c, * orb_z, * orb_o, * orb_zl;//orby
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
    trava(NULL),
    krystal_m(NULL),
    krystal_c(NULL),
    krystal_o(NULL),
    krystal_z(NULL),
    krystal_zl(NULL),
    kamen2(NULL),
    orb_m(NULL),
    orb_c(NULL),
    orb_o(NULL),
    orb_z(NULL),
    orb_zl(NULL)
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
    SafeRelease(&krystal_m);
    SafeRelease(&krystal_c);
    SafeRelease(&krystal_o);
    SafeRelease(&krystal_z);
    SafeRelease(&krystal_zl);
    SafeRelease(&kamen2);
    SafeRelease(&orb_m);
    SafeRelease(&orb_c);
    SafeRelease(&orb_z);
    SafeRelease(&orb_o);
    SafeRelease(&orb_zl);
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
            L"Adventure Game",
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
        
            if (SUCCEEDED(hr))
            {
                hr = LoadResourceBitmap(
                    m_pRenderTarget,
                    m_pWICFactory,
                    L"mkrystal",
                    L"Image",
                    500,
                    500,
                    &krystal_m
                );
            }
            if (SUCCEEDED(hr))
            {
                hr = LoadResourceBitmap(
                    m_pRenderTarget,
                    m_pWICFactory,
                    L"ckrystal",
                    L"Image",
                    500,
                    500,
                    &krystal_c
                );
            }
            if (SUCCEEDED(hr))
            {
                hr = LoadResourceBitmap(
                    m_pRenderTarget,
                    m_pWICFactory,
                    L"okrystal",
                    L"Image",
                    500,
                    500,
                    &krystal_o
                );
            }
            if (SUCCEEDED(hr))
            {
                hr = LoadResourceBitmap(
                    m_pRenderTarget,
                    m_pWICFactory,
                    L"zkrystal",
                    L"Image",
                    500,
                    500,
                    &krystal_z
                );
            }
            if (SUCCEEDED(hr))
            {
                hr = LoadResourceBitmap(
                    m_pRenderTarget,
                    m_pWICFactory,
                    L"zlkrystal",
                    L"Image",
                    500,
                    500,
                    &krystal_zl
                );
            }
            if (SUCCEEDED(hr))
            {
                hr = LoadResourceBitmap(
                    m_pRenderTarget,
                    m_pWICFactory,
                    L"zed2",
                    L"Image",
                    500,
                    500,
                    &kamen2
                );
            }
            if (SUCCEEDED(hr))
            {
                hr = LoadResourceBitmap(
                    m_pRenderTarget,
                    m_pWICFactory,
                    L"morb",
                    L"Image",
                    500,
                    500,
                    &orb_m
                );
            }
            if (SUCCEEDED(hr))
            {
                hr = LoadResourceBitmap(
                    m_pRenderTarget,
                    m_pWICFactory,
                    L"corb",
                    L"Image",
                    500,
                    500,
                    &orb_c
                );
            }
            if (SUCCEEDED(hr))
            {
                hr = LoadResourceBitmap(
                    m_pRenderTarget,
                    m_pWICFactory,
                    L"oorb",
                    L"Image",
                    500,
                    500,
                    &orb_o
                );
            }
            if (SUCCEEDED(hr))
            {
                hr = LoadResourceBitmap(
                    m_pRenderTarget,
                    m_pWICFactory,
                    L"zorb",
                    L"Image",
                    500,
                    500,
                    &orb_z
                );
            }
            if (SUCCEEDED(hr))
            {
                hr = LoadResourceBitmap(
                    m_pRenderTarget,
                    m_pWICFactory,
                    L"zlorb",
                    L"Image",
                    500,
                    500,
                    &orb_zl
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

        //load if command line argument: Game1 path *map*
        //*map* replace with map full path
        string open_file_name="lets_go.txt";
        LPWSTR* szArglist;
        int nArgs;

        szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
        if (NULL == szArglist)
        {
            MessageBox(NULL, L"CommandLineToArgvW failed\n", L"info", MB_OK);
        }
        else {
            if (szArglist[1] != L"path") {
                open_file_name = "lets_go.txt";
            }
            else {
                wstring ws(szArglist[2]);
                string tmp_name = string(ws.begin(), ws.end());
                open_file_name = tmp_name;
            }
        }
        LocalFree(szArglist);

        //nacteni mapy
        int startposx, startposy, width, height;
        if (exist_read_file(open_file_name.c_str())) {
            ifstream in(open_file_name.c_str());
            in >> width;
            in >> height;
            in >> startposx;
            in >> startposy;
            for (int _y = 0; _y < 10; _y++) {
                for (int _x = 0; _x < 17; _x++) {
                    int _t;
                    in >> _t;
                    if (_t == 1) {
                        zed[_y][_x] = typ_zdi::kamen_;
                    }else if (_t == 2) {
                        zed[_y][_x] = typ_zdi::kamen_o;
                    }
                    else {
                        zed[_y][_x] = typ_zdi::nic__;
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
            for (int _y = 0; _y < 10; _y++) {
                for (int _x = 0; _x < 17; _x++) {
                    int t;
                    in >> t;
                    if (t == 0) {
                        predmety[_y][_x] = typ_predmetu::nic_;
                    }
                    else if (t == 1) {
                        predmety[_y][_x] = typ_predmetu::krystal_m;
                    }
                    else if (t == 2) {
                        predmety[_y][_x] = typ_predmetu::krystal_c;
                    }
                    else if (t == 3) {
                        predmety[_y][_x] = typ_predmetu::nic_;
                    }
                    else if (t == 4) {
                        predmety[_y][_x] = typ_predmetu::krystal_z;
                    }
                    else if (t == 5) {
                        predmety[_y][_x] = typ_predmetu::krystal_o;
                    }
                    else if (t == 6) {
                        predmety[_y][_x] = typ_predmetu::krystal_zl;
                    }
                    else if (t == 7) {
                        predmety[_y][_x] = typ_predmetu::orb_m;
                    }
                    else if (t == 8) {
                        predmety[_y][_x] = typ_predmetu::orb_c;
                    }
                    else if (t == 9) {
                        predmety[_y][_x] = typ_predmetu::orb_z;
                    }
                    else if (t == 10) {
                        predmety[_y][_x] = typ_predmetu::orb_o;
                    }
                    else if (t == 11) {
                        predmety[_y][_x] = typ_predmetu::orb_zl;
                    }
                    else {
                        predmety[_y][_x] = typ_predmetu::nic_;
                    }
                }
            }
            in.close();
        }
        else {
            MessageBox(NULL, L"Defaultni mapa nebo soubor otevreny pres prikaz -path neexistuje.", L"info", MB_OK | MB_ICONERROR);
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
                    if ((zed[y][x+1] != typ_zdi::kamen_) && (zed[y][x + 1] != typ_zdi::kamen_o)) {
                        x++;
                        mom_postava = 3;
                    }
                    if (predmety[y][x] == typ_predmetu::krystal_m) {
                        sebr_mk++;
                        predmety[y][x] = typ_predmetu::nic_;
                    }
                    else if (predmety[y][x] == typ_predmetu::krystal_c) {
                        sebr_ck++;
                        predmety[y][x] = typ_predmetu::nic_;
                    }
                    else if (predmety[y][x] == typ_predmetu::krystal_o) {
                        sebr_ok++;
                        predmety[y][x] = typ_predmetu::nic_;
                    }
                    else if (predmety[y][x] == typ_predmetu::krystal_z) {
                        sebr_zk++;
                        predmety[y][x] = typ_predmetu::nic_;
                    }
                    else if (predmety[y][x] == typ_predmetu::krystal_zl) {
                        sebr_zlk++;
                        predmety[y][x] = typ_predmetu::nic_;
                    }
                    else if (predmety[y][x] == typ_predmetu::orb_m) {
                        sebr_mo++;
                        predmety[y][x] = typ_predmetu::nic_;
                    }
                    else if (predmety[y][x] == typ_predmetu::orb_c) {
                        sebr_co++;
                        predmety[y][x] = typ_predmetu::nic_;
                    }
                    else if (predmety[y][x] == typ_predmetu::orb_o) {
                        sebr_oo++;
                        predmety[y][x] = typ_predmetu::nic_;
                    }
                    else if (predmety[y][x] == typ_predmetu::orb_z) {
                        sebr_zo++;
                        predmety[y][x] = typ_predmetu::nic_;
                    }
                    else if (predmety[y][x] == typ_predmetu::orb_zl) {
                        sebr_zlo++;
                        predmety[y][x] = typ_predmetu::nic_;
                    }
                    
                    SendMessage(hwnd, WM_PAINT, 0, 0);
                }
                else if ((GetAsyncKeyState(0x25) & 0x8000) || (GetAsyncKeyState(0x41) & 0x8000)) {
                    //key-left pressed | key a
                    if ((zed[y][x - 1] != typ_zdi::kamen_) && (zed[y][x - 1] != typ_zdi::kamen_o)) {
                        x--;
                        mom_postava = 4;
                    }
                    if (predmety[y][x] == typ_predmetu::krystal_m) {
                        sebr_mk++;
                        predmety[y][x] = typ_predmetu::nic_;
                    }
                    else if (predmety[y][x] == typ_predmetu::krystal_c) {
                        sebr_ck++;
                        predmety[y][x] = typ_predmetu::nic_;
                    }
                    else if (predmety[y][x] == typ_predmetu::krystal_o) {
                        sebr_ok++;
                        predmety[y][x] = typ_predmetu::nic_;
                    }
                    else if (predmety[y][x] == typ_predmetu::krystal_z) {
                        sebr_zk++;
                        predmety[y][x] = typ_predmetu::nic_;
                    }
                    else if (predmety[y][x] == typ_predmetu::krystal_zl) {
                        sebr_zlk++;
                        predmety[y][x] = typ_predmetu::nic_;
                    }
                    else if (predmety[y][x] == typ_predmetu::orb_m) {
                        sebr_mo++;
                        predmety[y][x] = typ_predmetu::nic_;
                    }
                    else if (predmety[y][x] == typ_predmetu::orb_c) {
                        sebr_co++;
                        predmety[y][x] = typ_predmetu::nic_;
                    }
                    else if (predmety[y][x] == typ_predmetu::orb_o) {
                        sebr_oo++;
                        predmety[y][x] = typ_predmetu::nic_;
                    }
                    else if (predmety[y][x] == typ_predmetu::orb_z) {
                        sebr_zo++;
                        predmety[y][x] = typ_predmetu::nic_;
                    }
                    else if (predmety[y][x] == typ_predmetu::orb_zl) {
                        sebr_zlo++;
                        predmety[y][x] = typ_predmetu::nic_;
                    }
                    SendMessage(hwnd, WM_PAINT, 0, 0);
                } 
                else if ((GetAsyncKeyState(0x28) & 0x8000) || (GetAsyncKeyState(0x53) & 0x8000)){
                    //key-down pressed | key s 
                    if ((zed[y+1][x] != typ_zdi::kamen_) && (zed[y+1][x] != typ_zdi::kamen_o)) {
                        y++;
                        mom_postava = 1;
                    }
                    if (predmety[y][x] == typ_predmetu::krystal_m) {
                        sebr_mk++;
                        predmety[y][x] = typ_predmetu::nic_;
                    }
                    else if (predmety[y][x] == typ_predmetu::krystal_c) {
                        sebr_ck++;
                        predmety[y][x] = typ_predmetu::nic_;
                    }
                    else if (predmety[y][x] == typ_predmetu::krystal_o) {
                        sebr_ok++;
                        predmety[y][x] = typ_predmetu::nic_;
                    }
                    else if (predmety[y][x] == typ_predmetu::krystal_z) {
                        sebr_zk++;
                        predmety[y][x] = typ_predmetu::nic_;
                    }
                    else if (predmety[y][x] == typ_predmetu::krystal_zl) {
                        sebr_zlk++;
                        predmety[y][x] = typ_predmetu::nic_;
                    }
                    else if (predmety[y][x] == typ_predmetu::orb_m) {
                        sebr_mo++;
                        predmety[y][x] = typ_predmetu::nic_;
                    }
                    else if (predmety[y][x] == typ_predmetu::orb_c) {
                        sebr_co++;
                        predmety[y][x] = typ_predmetu::nic_;
                    }
                    else if (predmety[y][x] == typ_predmetu::orb_o) {
                        sebr_oo++;
                        predmety[y][x] = typ_predmetu::nic_;
                    }
                    else if (predmety[y][x] == typ_predmetu::orb_z) {
                        sebr_zo++;
                        predmety[y][x] = typ_predmetu::nic_;
                    }
                    else if (predmety[y][x] == typ_predmetu::orb_zl) {
                        sebr_zlo++;
                        predmety[y][x] = typ_predmetu::nic_;
                    }
                    SendMessage(hwnd, WM_PAINT, 0, 0);
                }
                else if ((GetAsyncKeyState(0x26) & 0x8000) || (GetAsyncKeyState(0x57) & 0x8000)) {
                     //key-up pressed | key w 
                    if ((zed[y- 1][x] != typ_zdi::kamen_) && (zed[y - 1][x] != typ_zdi::kamen_o)) {
                        y--;
                        mom_postava = 2;
                    }
                    if (predmety[y][x] == typ_predmetu::krystal_m) {
                        sebr_mk++;
                        predmety[y][x] = typ_predmetu::nic_;
                    }
                    else if (predmety[y][x] == typ_predmetu::krystal_c) {
                        sebr_ck++;
                        predmety[y][x] = typ_predmetu::nic_;
                    }
                    else if (predmety[y][x] == typ_predmetu::krystal_o) {
                        sebr_ok++;
                        predmety[y][x] = typ_predmetu::nic_;
                    }
                    else if (predmety[y][x] == typ_predmetu::krystal_z) {
                        sebr_zk++;
                        predmety[y][x] = typ_predmetu::nic_;
                    }
                    else if (predmety[y][x] == typ_predmetu::krystal_zl) {
                        sebr_zlk++;
                        predmety[y][x] = typ_predmetu::nic_;
                    }
                    else if (predmety[y][x] == typ_predmetu::orb_m) {
                        sebr_mo++;
                        predmety[y][x] = typ_predmetu::nic_;
                    }
                    else if (predmety[y][x] == typ_predmetu::orb_c) {
                        sebr_co++;
                        predmety[y][x] = typ_predmetu::nic_;
                    }
                    else if (predmety[y][x] == typ_predmetu::orb_o) {
                        sebr_oo++;
                        predmety[y][x] = typ_predmetu::nic_;
                    }
                    else if (predmety[y][x] == typ_predmetu::orb_z) {
                        sebr_zo++;
                        predmety[y][x] = typ_predmetu::nic_;
                    }
                    else if (predmety[y][x] == typ_predmetu::orb_zl) {
                        sebr_zlo++;
                        predmety[y][x] = typ_predmetu::nic_;
                    }
                     SendMessage(hwnd, WM_PAINT, 0, 0);
                }
                else if (GetAsyncKeyState(VK_F2) & 0x8000) {
                    //key f2
                    string str = "Pocet sebranych krystalu - modre: " + to_string(sebr_mk) + ", cervene: " + to_string(sebr_ck) + ", zelene: " + to_string(sebr_zk) + ", oranzove: " + to_string(sebr_ok) + ", zlute: " + to_string(sebr_zlk) + "\n" + "Pocet sebranych orbu - modre: " + to_string(sebr_mo) + ", cervene: " + to_string(sebr_co) + ", zelene: " + to_string(sebr_zo) + ", oranzove: " + to_string(sebr_oo) + ", zlute: " + to_string(sebr_zlo);
                    MessageBoxA(NULL, str.c_str(), "Info", MB_OK | MB_ICONINFORMATION);
                }
                else if (GetAsyncKeyState(VK_F3) & 0x8000) {
                    //key f2
                    MessageBox(NULL, L"Leze, leze po železe, až dá pokoj tak tam vleze. Co je to?", L"Hálo tady implusovy", MB_OK);
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
                            sebr_ck = 0; sebr_mk = 0; sebr_ok = 0; sebr_zk = 0;
                            ifstream in(fileNameStr.c_str());
                            in >> width;
                            in >> height;
                            in >> startposx;
                            in >> startposy;
                            x = startposx;
                            y = startposy;
                            for (int _y = 0; _y < 10; _y++) {
                                for (int _x = 0; _x < 17; _x++) {
                                    int _t;
                                    in >> _t;
                                    if (_t == 1) {
                                        zed[_y][_x] = typ_zdi::kamen_;
                                    }
                                    else if (_t == 2) {
                                        zed[_y][_x] = typ_zdi::kamen_o;
                                    }
                                    else {
                                        zed[_y][_x] = typ_zdi::nic__;
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
                            for (int _y = 0; _y < 10; _y++) {
                                for (int _x = 0; _x < 17; _x++) {
                                    int t;
                                    in >> t;
                                    if (t == 0) {
                                        predmety[_y][_x] = typ_predmetu::nic_;
                                    }
                                    else if (t == 1) {
                                        predmety[_y][_x] = typ_predmetu::krystal_m;
                                    }
                                    else if (t == 2) {
                                        predmety[_y][_x] = typ_predmetu::krystal_c;
                                    }
                                    else if (t == 3) {
                                        predmety[_y][_x] = typ_predmetu::nic_;
                                    }
                                    else if (t == 4) {
                                        predmety[_y][_x] = typ_predmetu::krystal_z;
                                    }
                                    else if (t == 5) {
                                        predmety[_y][_x] = typ_predmetu::krystal_o;
                                    }
                                    else if (t == 6) {
                                        predmety[_y][_x] = typ_predmetu::krystal_zl;
                                    }
                                    else if (t == 7) {
                                        predmety[_y][_x] = typ_predmetu::orb_m;
                                    }
                                    else if (t == 8) {
                                        predmety[_y][_x] = typ_predmetu::orb_c;
                                    }
                                    else if (t == 9) {
                                        predmety[_y][_x] = typ_predmetu::orb_z;
                                    }
                                    else if (t == 10) {
                                        predmety[_y][_x] = typ_predmetu::orb_o;
                                    }
                                    else if (t == 11) {
                                        predmety[_y][_x] = typ_predmetu::orb_zl;
                                    }
                                    else {
                                        predmety[_y][_x] = typ_predmetu::nic_;
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

void IntInfo(int i) {
    string t = to_string(i);
    MessageBoxA(NULL, t.c_str(), t.c_str(), MB_OK);
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

        //pocet zobrazenych ctvercu - pro vetsi rychlost
        /*
        int zobraz_sirka = round(width / 80)+1;
        int zobraz_vyska = round(height / 80)+1;
        */

#ifdef POSOUVANI_KAMERY
        int hrac_x = round(((width - 80) / 2) / 80) * 80;
        int hrac_y = round(((height - 80) / 2) / 80) * 80;

        int hrac_x2 = (hrac_x / 80) - x;
        int hrac_y2 = (hrac_y / 80) - y;

        for (int _x = 0; _x < 17; _x++) {
            for (int _y = 0; _y < 10; _y++) {
                if (zed[_y + y-4][_x + x-7] == true) {
                    m_pRenderTarget->DrawBitmap(kamen, SRect(_x * 80, _y * 80, (_x * 80) + 80, (_y * 80) + 80), 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                }
                else if (podlaha[_y + y-4][_x + x-7] == typ_podlahy::trava) {
                    m_pRenderTarget->DrawBitmap(trava, SRect(_x * 80, _y * 80, (_x * 80) + 80, (_y * 80) + 80), 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                }
                else if (podlaha[_y + y-4][_x + x-7] == typ_podlahy::kamen) {
                    m_pRenderTarget->DrawBitmap(podlaha_kamen, SRect(_x * 80, _y * 80, (_x * 80) + 80, (_y * 80) + 80), 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                }

                if (predmety[_y + y-4][_x + x-7] == typ_predmetu::krystal_m) {
                    m_pRenderTarget->DrawBitmap(krystal_m, SRect(_x * 80, _y * 80, (_x * 80) + 80, (_y * 80) + 80), 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                }
                else if (predmety[_y + y-4][_x + x-7] == typ_predmetu::krystal_c) {
                    m_pRenderTarget->DrawBitmap(krystal_c, SRect(_x * 80, _y * 80, (_x * 80) + 80, (_y * 80) + 80), 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                }
                else if (predmety[_y + y-4][_x + x-7] == typ_predmetu::krystal_z) {
                    m_pRenderTarget->DrawBitmap(krystal_z, SRect(_x * 80, _y * 80, (_x * 80) + 80, (_y * 80) + 80), 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                }
                else if (predmety[_y - y][_x + x-7] == typ_predmetu::krystal_o) {
                    m_pRenderTarget->DrawBitmap(krystal_o, SRect(_x * 80, _y * 80, (_x * 80) + 80, (_y * 80) + 80), 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                }
                else if (predmety[_y - y][_x + x - 7] == typ_predmetu::krystal_zl) {
                    m_pRenderTarget->DrawBitmap(krystal_zl, SRect(_x * 80, _y * 80, (_x * 80) + 80, (_y * 80) + 80), 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                }
                else {

                }
            }
        }
        D2D1_RECT_F postava_pos = SRect(hrac_x, hrac_y, hrac_x + 80, hrac_y + 80);
#else
        for (int _x = 0; _x < 17; _x++) {
            for (int _y = 0; _y < 10; _y++) {

                if (podlaha[_y][_x] == typ_podlahy::trava) {
                    m_pRenderTarget->DrawBitmap(trava, SRect(_x * 80, _y * 80, (_x * 80) + 80, (_y * 80) + 80), 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                }
                else if (podlaha[_y][_x] == typ_podlahy::kamen) {
                    m_pRenderTarget->DrawBitmap(podlaha_kamen, SRect(_x * 80, _y * 80, (_x * 80) + 80, (_y * 80) + 80), 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                }

                //musi byt zvlast jinak by tam bylo bilo - nema pozadi obrazek
                if (zed[_y][_x] == typ_zdi::kamen_o) {
                    m_pRenderTarget->DrawBitmap(kamen2, SRect(_x * 80, _y * 80, (_x * 80) + 80, (_y * 80) + 80), 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                }
                else if (zed[_y][_x] == typ_zdi::kamen_) {
                    m_pRenderTarget->DrawBitmap(kamen, SRect(_x * 80, _y * 80, (_x * 80) + 80, (_y * 80) + 80), 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                }

                if (predmety[_y][_x] == typ_predmetu::krystal_m) {
                    m_pRenderTarget->DrawBitmap(krystal_m, SRect(_x * 80, _y * 80, (_x * 80) + 80, (_y * 80) + 80), 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                }
                else if (predmety[_y][_x] == typ_predmetu::krystal_c) {
                    m_pRenderTarget->DrawBitmap(krystal_c, SRect(_x * 80, _y * 80, (_x * 80) + 80, (_y * 80) + 80), 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                }
                else if (predmety[_y][_x] == typ_predmetu::krystal_z) {
                    m_pRenderTarget->DrawBitmap(krystal_z, SRect(_x * 80, _y * 80, (_x * 80) + 80, (_y * 80) + 80), 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                }
                else if (predmety[_y][_x] == typ_predmetu::krystal_o) {
                    m_pRenderTarget->DrawBitmap(krystal_o, SRect(_x * 80, _y * 80, (_x * 80) + 80, (_y * 80) + 80), 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                }
                else if (predmety[_y][_x] == typ_predmetu::orb_m) {
                    m_pRenderTarget->DrawBitmap(orb_m, SRect(_x * 80, _y * 80, (_x * 80) + 80, (_y * 80) + 80), 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                }
                else if (predmety[_y][_x] == typ_predmetu::orb_c) {
                    m_pRenderTarget->DrawBitmap(orb_c, SRect(_x * 80, _y * 80, (_x * 80) + 80, (_y * 80) + 80), 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                }
                else if (predmety[_y][_x] == typ_predmetu::orb_zl) {
                    m_pRenderTarget->DrawBitmap(orb_zl, SRect(_x * 80, _y * 80, (_x * 80) + 80, (_y * 80) + 80), 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                }
                else if (predmety[_y][_x] == typ_predmetu::orb_z) {
                    m_pRenderTarget->DrawBitmap(orb_z, SRect(_x * 80, _y * 80, (_x * 80) + 80, (_y * 80) + 80), 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                }
                else if (predmety[_y][_x] == typ_predmetu::orb_o) {
                    m_pRenderTarget->DrawBitmap(orb_o, SRect(_x * 80, _y * 80, (_x * 80) + 80, (_y * 80) + 80), 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                }
                else {

                }
            }
        }

        D2D1_RECT_F postava_pos = D2D1::RectF(x * 80, y * 80, (x * 80) + 80, (y * 80) + 80);
#endif

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