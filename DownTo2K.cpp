#include <windows.h>
#include <windowsx.h>
#include <gdiplus.h>
#include "resource.h"

LPTSTR MZCAPI LoadStringDx(INT nID)
{
    static UINT s_index = 0;
    const UINT cchBuffMax = 1024;
    static TCHAR s_sz[4][cchBuffMax];

    TCHAR *pszBuff = s_sz[s_index];
    s_index = (s_index + 1) % _countof(s_sz);
    pszBuff[0] = 0;
    if (!::LoadString(NULL, nID, pszBuff, cchBuffMax))
        assert(0);
    return pszBuff;
}

HBITMAP
DoLoadBitmapFromRes(HINSTANCE hInst, LPCWSTR pszType, LPCWSTR pszName)
{
    HRSRC hRsrc = FindResource(hInst, pszName, pszType);
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
            Gdiplus::ARGB argb = Gdiplus::MakeARGB(0xFF, 0xFF, 0xFF, 0xFF);
            pBitmap->GetHBITMAP(Gdiplus::Color(argb, &hbm);
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

BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
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

BOOL InitInstance(HINSTANCE hInst)
{
}

void ExitInstance()
{
}

INT WINAPI
WinMain(HINSTANCE   hInstance,
        HINSTANCE   hPrevInstance,
        LPSTR       lpCmdLine,
        INT         nCmdShow)
{
    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);

    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    InitCommonControls();
    HINSTANCE hinstSysLink = LoadLibraryW(L"SysLink.dll");

    InitInstance(hInstance);
    {
        DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAIN), NULL, DialogProc);
    }
    ExitInstance();

    Gdiplus::GdiplusShutdown(gdiplusToken);
    FreeLibrary(hinstSysLink);

    if (hr == S_OK)
        CoUninitialize();
    return 0;
}
