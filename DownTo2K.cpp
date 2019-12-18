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

HBITMAP CreateResizedBitmapForCtrl(HWND hwndCtrl, HBITMAP hbm)
{
    RECT rc;
    GetClientRect(hwndCtrl, &rc);

    SIZE siz;
    siz.cx = rc.right - rc.left;
    siz.cy = rc.bottom - rc.top;

    return (HBITMAP)CopyImage(hbm, IMAGE_BITMAP, siz.cx, siz.cy, LR_CREATEDIBSECTION);
}

static HBITMAP s_hbmCheckmark;
static HBITMAP s_hbmCheckmarkResized;
static HBITMAP s_hbmMicrosoft;
static HBITMAP s_hbmWin2KPro;
static HBITMAP s_hbmWin2KProResized;
static HBITMAP s_hbmWinFlag;
static HFONT s_hBigFont;
BOOL InitInstance(HWND hwnd, HINSTANCE hInst)
{
    s_hbmCheckmark = DoLoadBitmapFromRes(hInst, L"PNG", MAKEINTRESOURCEW(IDR_PNG_CHECKMARK));

    HWND hStc5 = GetDlgItem(hwnd, stc5);
    s_hbmCheckmarkResized = CreateResizedBitmapForCtrl(hStc5, s_hbmCheckmark);

    s_hbmMicrosoft = DoLoadBitmapFromRes(hInst, L"PNG", MAKEINTRESOURCEW(IDR_PNG_MICROSOFT));

    s_hbmWin2KPro = DoLoadBitmapFromRes(hInst, L"PNG", MAKEINTRESOURCEW(IDR_PNG_WIN2KPRO));

    HWND hStc10 = GetDlgItem(hwnd, stc10);
    s_hbmWin2KProResized = CreateResizedBitmapForCtrl(hStc10, s_hbmWin2KPro);

    s_hbmWinFlag = DoLoadBitmapFromRes(hInst, L"PNG", MAKEINTRESOURCEW(IDR_PNG_WINFLAG));

    HWND hStc11 = GetDlgItem(hwnd, stc11);
    HFONT hFont = GetWindowFont(hwnd);
    LOGFONTW lf;
    GetObjectW(hFont, sizeof(lf), &lf);

    lf.lfHeight *= 4;
    lf.lfHeight /= 3;
    lf.lfWeight = FW_BOLD;
    s_hBigFont = CreateFontIndirectW(&lf);

    SetWindowFont(hStc11, s_hBigFont, TRUE);

    return TRUE;
}

void ExitInstance(void)
{
    DeleteObject(s_hbmCheckmark);
    s_hbmCheckmark = NULL;

    DeleteObject(s_hbmCheckmarkResized);
    s_hbmCheckmarkResized = NULL;

    DeleteObject(s_hbmWin2KPro);
    s_hbmWin2KPro = NULL;

    DeleteObject(s_hbmWin2KProResized);
    s_hbmWin2KProResized = NULL;

    DeleteObject(s_hBigFont);
    s_hBigFont = NULL;
}

BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    InitInstance(hwnd, s_hInst);

    SendDlgItemMessageW(hwnd, stc5, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)s_hbmCheckmarkResized);
    SendDlgItemMessageW(hwnd, stc6, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)s_hbmCheckmarkResized);
    SendDlgItemMessageW(hwnd, stc7, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)s_hbmCheckmarkResized);
    SendDlgItemMessageW(hwnd, stc8, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)s_hbmCheckmarkResized);

    SendDlgItemMessageW(hwnd, stc9, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)s_hbmMicrosoft);

    SendDlgItemMessageW(hwnd, stc10, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)s_hbmWin2KProResized);
    SendDlgItemMessageW(hwnd, stc3, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)s_hbmWinFlag);

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

HBRUSH OnCtlColor(HWND hwnd, HDC hdc, HWND hwndChild, int type)
{
    return GetStockBrush(WHITE_BRUSH);
}

INT_PTR CALLBACK
DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        HANDLE_MSG(hwnd, WM_INITDIALOG, OnInitDialog);
        HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);
        HANDLE_MSG(hwnd, WM_CTLCOLORSTATIC, OnCtlColor);
        HANDLE_MSG(hwnd, WM_CTLCOLORDLG, OnCtlColor);
        HANDLE_MSG(hwnd, WM_CTLCOLORBTN, OnCtlColor);
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
