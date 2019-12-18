#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <gdiplus.h>
#include <cassert>
#include "resource.h"

static HINSTANCE s_hInst;

LPWSTR LoadStringDx(INT nID)
{
    static UINT s_index = 0;
    const UINT cchBuffMax = 1024;
    static WCHAR s_sz[4][cchBuffMax];

    WCHAR *pszBuff = s_sz[s_index];
    s_index = (s_index + 1) % _countof(s_sz);
    pszBuff[0] = 0;
    if (!::LoadStringW(NULL, nID, pszBuff, cchBuffMax))
        assert(0);
    return pszBuff;
}

HBITMAP
DoLoadBitmapFromRes(HINSTANCE hInst, LPCWSTR pszType, LPCWSTR pszName)
{
    HRSRC hRsrc = FindResourceW(hInst, pszName, pszType);
    if (!hRsrc)
        return NULL;

    DWORD cbRsrc = SizeofResource(hInst, hRsrc);
    if (!cbRsrc)
        return NULL;

    LPVOID pvRsrc = LockResource(LoadResource(hInst, hRsrc));
    if (!pvRsrc)
        return NULL;

    HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, cbRsrc);
    if (!hGlobal)
        return NULL;

    LPVOID pvGlobal = GlobalLock(hGlobal);
    if (!hGlobal)
        return NULL;

    CopyMemory(pvGlobal, pvRsrc, cbRsrc);
    GlobalUnlock(hGlobal);

    Gdiplus::Bitmap *pBitmap = NULL;
    IStream *pStream = NULL;
    HBITMAP hbm = NULL;
    HRESULT hr = CreateStreamOnHGlobal(hGlobal, FALSE, &pStream);
    if (hr == S_OK)
    {
        pBitmap = Gdiplus::Bitmap::FromStream(pStream);
        if (pBitmap)
        {
            Gdiplus::Color argb(0xFF, 0xFF, 0xFF, 0xFF);
            pBitmap->GetHBITMAP(argb, &hbm);
        }
        delete pBitmap;
    }
    if (pStream)
    {
        pStream->Release();
    }
    GlobalFree(hGlobal);

    return hbm;
}

BOOL InitInstance(HWND hwnd, HINSTANCE hInst)
{
    return TRUE;
}

void ExitInstance(void)
{
}

BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    InitInstance(hwnd, s_hInst);
    return TRUE;
}

void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    switch (id)
    {
    case IDOK:
    case IDCANCEL:
        EndDialog(hwnd, id);
        break;
    }
}

INT_PTR CALLBACK
DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        HANDLE_MSG(hwnd, WM_INITDIALOG, OnInitDialog);
        HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);
    }
    return 0;
}

INT WINAPI
WinMain(HINSTANCE   hInstance,
        HINSTANCE   hPrevInstance,
        LPSTR       lpCmdLine,
        INT         nCmdShow)
{
    HRESULT hr;
    ULONG_PTR gdiplusToken;
    HINSTANCE hinstSysLink;
    s_hInst = hInstance;

    hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);

    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    InitCommonControls();
    hinstSysLink = LoadLibraryW(L"SysLink");

    DialogBoxW(hInstance, MAKEINTRESOURCEW(IDD_MAIN), NULL, DialogProc);
    ExitInstance();

    Gdiplus::GdiplusShutdown(gdiplusToken);
    FreeLibrary(hinstSysLink);

    if (hr == S_OK)
        CoUninitialize();
    return 0;
}
