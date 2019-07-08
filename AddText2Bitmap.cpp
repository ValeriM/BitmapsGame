// AddText2Bitmap.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

void AddText2Bitmap(char *sourceBmp, char *text, char *destBmp);
int ReplaceColor(char *, int oldColorRed = 255, int oldColorGreen = 0, int oldColorBlue = 0, int newColorRed = 0, int newColorGreen = 255, int newColorBlue = 0, char *destBmp = "dest.bmp");
int ReplaceColor2(char *, int oldColorRed = 255, int oldColorGreen = 0, int oldColorBlue = 0, int newColorRed = 0, int newColorGreen = 255, int newColorBlue = 0, char *destBmp = "dest.bmp");
int _tmain(int argc, _TCHAR* argv[])
{
	//AddText2Bitmap(argv[1], argv[2], argv[3]);
	ReplaceColor2(argv[1]);
	return 0;
}

HBITMAP         Create24BPPDIBSection(HDC hDC, int iWidth, int iHeight)
	{
	    BITMAPINFO      bmi;
	    HBITMAP         hbm;
	    void *          pBits;/*LPBYTE*/
	    // Initialize to 0s.       
		ZeroMemory(&bmi, sizeof(bmi));
	    // Initialize the header.
	    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	    bmi.bmiHeader.biWidth = iWidth;
	    bmi.bmiHeader.biHeight = iHeight;
	    bmi.bmiHeader.biPlanes = 1;
	    bmi.bmiHeader.biBitCount = 24;
	    bmi.bmiHeader.biCompression = BI_RGB;   // Create the surface.
	    hbm = CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
	    return (hbm);
	}
int CalcLen(HDC hdc, char *str)
{
  int l = 0, s = 0;
  while(*str)
  {
		GetCharWidth32(hdc, *str, *str, &l);
		s += l;
		str++;
  }
  return s;
}
void errhandler(const char *s, HWND h = NULL)
{
	MessageBox(h, s, "Error", 0);
}

PBITMAPINFO CreateBitmapInfoStruct(HWND hwnd, HBITMAP hBmp)
{
    BITMAP bmp;
    PBITMAPINFO pbmi;
    WORD    cClrBits;
	
    // Retrieve the bitmap's color format, width, and height.
    if (!GetObject(hBmp, sizeof(BITMAP), (LPSTR)&bmp))
        errhandler("GetObject", hwnd);

    // Convert the color format to a count of bits.
    cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel);
    if (cClrBits == 1)
        cClrBits = 1;
    else if (cClrBits <= 4)
        cClrBits = 4;
    else if (cClrBits <= 8)
        cClrBits = 8;
    else if (cClrBits <= 16)
        cClrBits = 16;
    else if (cClrBits <= 24)
        cClrBits = 24;
    else cClrBits = 32;

    // Allocate memory for the BITMAPINFO structure. (This structure
    // contains a BITMAPINFOHEADER structure and an array of RGBQUAD
    // data structures.)

     if (cClrBits != 24)
         pbmi = (PBITMAPINFO) LocalAlloc(LPTR,
                    sizeof(BITMAPINFOHEADER) +
                    sizeof(RGBQUAD) * (1<< cClrBits));

     // There is no RGBQUAD array for the 24-bit-per-pixel format.

     else
         pbmi = (PBITMAPINFO) LocalAlloc(LPTR,
                    sizeof(BITMAPINFOHEADER));

    // Initialize the fields in the BITMAPINFO structure.

    pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    pbmi->bmiHeader.biWidth = bmp.bmWidth;
    pbmi->bmiHeader.biHeight = bmp.bmHeight;
    pbmi->bmiHeader.biPlanes = bmp.bmPlanes;
    pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel;
    if (cClrBits < 24)
        pbmi->bmiHeader.biClrUsed = (1<<cClrBits);

    // If the bitmap is not compressed, set the BI_RGB flag.
    pbmi->bmiHeader.biCompression = BI_RGB;

    // Compute the number of bytes in the array of color
    // indices and store the result in biSizeImage.
    pbmi->bmiHeader.biSizeImage = (pbmi->bmiHeader.biWidth + 7) /8
                                  * pbmi->bmiHeader.biHeight * cClrBits;
	/*pbmi->bmiHeader.biSizeImage = ((pbmi->bmiHeader.biWidth * cClrBits + 31) & ~ 31) /8
                                  * pbmi->bmiHeader.biHeight;*/
// Set biClrImportant to 0, indicating that all of the
    // device colors are important.
     pbmi->bmiHeader.biClrImportant = 0;
     return pbmi;
}

void CreateBMPFile(HWND hwnd, LPTSTR pszFile, PBITMAPINFO pbi,
                  HBITMAP hBMP, HDC hDC)
{
    HANDLE hf;                  // file handle
    BITMAPFILEHEADER hdr;       // bitmap file-header
    PBITMAPINFOHEADER pbih;     // bitmap info-header
    LPBYTE lpBits;              // memory pointer
    DWORD dwTotal;              // total count of bytes
    DWORD cb;                   // incremental count of bytes
    BYTE *hp;                   // byte pointer
    DWORD dwTmp;

    pbih = (PBITMAPINFOHEADER) pbi;
    lpBits = (LPBYTE) GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);

    if (!lpBits)
         errhandler("GlobalAlloc", hwnd);

    // Retrieve the color table (RGBQUAD array) and the bits
    // (array of palette indices) from the DIB.
    if (!GetDIBits(hDC, hBMP, 0, (WORD) pbih->biHeight, lpBits, pbi,
        DIB_RGB_COLORS))
    {
        errhandler("GetDIBits", hwnd);
    }

    // Create the .BMP file.
    hf = CreateFile(pszFile,
                   GENERIC_READ | GENERIC_WRITE,
                   (DWORD) 0,
                    NULL,
                   CREATE_ALWAYS,
                   FILE_ATTRIBUTE_NORMAL,
                   (HANDLE) NULL);
    if (hf == INVALID_HANDLE_VALUE)
        errhandler("CreateFile", hwnd);
    hdr.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M"
    // Compute the size of the entire file.
    hdr.bfSize = (DWORD) (sizeof(BITMAPFILEHEADER) +
                 pbih->biSize + pbih->biClrUsed
                 * sizeof(RGBQUAD) + pbih->biSizeImage);
    hdr.bfReserved1 = 0;
    hdr.bfReserved2 = 0;

    // Compute the offset to the array of color indices.
    hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) +
                    pbih->biSize + pbih->biClrUsed
                    * sizeof (RGBQUAD);

    // Copy the BITMAPFILEHEADER into the .BMP file.
    if (!WriteFile(hf, (LPVOID) &hdr, sizeof(BITMAPFILEHEADER),
        (LPDWORD) &dwTmp,  NULL))
    {
       errhandler("WriteFile", hwnd);
    }

    // Copy the BITMAPINFOHEADER and RGBQUAD array into the file.
    if (!WriteFile(hf, (LPVOID) pbih, sizeof(BITMAPINFOHEADER)
                  + pbih->biClrUsed * sizeof (RGBQUAD),
                  (LPDWORD) &dwTmp,  NULL))
        errhandler("WriteFile", hwnd);

    // Copy the array of color indices into the .BMP file.
    dwTotal = cb = pbih->biSizeImage;
    hp = lpBits;
    if (!WriteFile(hf, (LPSTR) hp, (int) cb, (LPDWORD) &dwTmp,NULL))
           errhandler("WriteFile", hwnd);

    // Close the .BMP file.
     if (!CloseHandle(hf))
           errhandler("CloseHandle", hwnd);

    // Free memory.
    GlobalFree((HGLOBAL)lpBits);
}

void SaveBmp2File(HBITMAP hBmp, HDC hDC, char *fileName)
{
	PBITMAPINFO pbmi = CreateBitmapInfoStruct(NULL, hBmp);
    CreateBMPFile(NULL, fileName, pbmi, hBmp, hDC);
	LocalFree(pbmi);
}

int ReplaceColor(char *sourceBmp, int oldColorRed, int oldColorGreen, int oldColorBlue, int newColorRed, int newColorGreen, int newColorBlue, char *destBmp)
{
	COLORREF oldColor = RGB(oldColorRed, oldColorGreen, oldColorBlue);
	COLORREF newColor = RGB(newColorRed, newColorGreen, newColorBlue);
	HBITMAP hSourceBmp = (HBITMAP)LoadImage(
    NULL,   // handle to instance
    sourceBmp,  // name or identifier of the image .say"C:\\NewFolder\\1.bmp"
    IMAGE_BITMAP,        // image types
    0,     // desired width
    0,     // desired height
    LR_LOADFROMFILE);
	BITMAP bmp;
	GetObject(hSourceBmp, sizeof(BITMAP), (LPSTR)&bmp);
	int width = bmp.bmWidth;
	int height = bmp.bmHeight;
	int ret = 0;
	HDC hdcScreen = GetDC(NULL);
	HDC memHDC = CreateCompatibleDC(hdcScreen);
	HDC fileHDC = CreateCompatibleDC(hdcScreen);
	HBITMAP memBmp = Create24BPPDIBSection(hdcScreen, width, height);
	SelectObject(fileHDC, hSourceBmp);
	SelectObject(memHDC, memBmp);
	for (int w = 0; w < width; w++)
		for (int h = 0; h < height; h++)
		{
			if (GetPixel(fileHDC, w, h) == oldColor)
			{
				
				SetPixelV(fileHDC, w, h, newColor);
				ret = 1;
			}
  
		}
	BitBlt(memHDC, 0, 0, width, height, fileHDC, 0, 0, SRCCOPY);
	SaveBmp2File(memBmp, memHDC, destBmp);
	return ret;
}

int ReplaceColor2(char *sourceBmp, int oldColorRed, int oldColorGreen, int oldColorBlue, int newColorRed, int newColorGreen, int newColorBlue, char *destBmp)
{
	COLORREF oldColor = RGB(oldColorRed, oldColorGreen, oldColorBlue);
	COLORREF newColor = RGB(newColorRed, newColorGreen, newColorBlue);
	HBITMAP hSourceBmp = (HBITMAP)LoadImage(
    NULL,   // handle to instance
    sourceBmp,  // name or identifier of the image .say"C:\\NewFolder\\1.bmp"
    IMAGE_BITMAP,        // image types
    0,     // desired width
    0,     // desired height
    LR_LOADFROMFILE);
	BITMAP bmp;
	GetObject(hSourceBmp, sizeof(BITMAP), (LPSTR)&bmp);
	int width = bmp.bmWidth;
	int height = bmp.bmHeight;
	int ret = 0;
	HDC hdcScreen = GetDC(NULL);
	HDC memHDC = CreateCompatibleDC(hdcScreen);
	HDC fileHDC = CreateCompatibleDC(hdcScreen);
	HBITMAP memBmp = Create24BPPDIBSection(hdcScreen, width, height);
	SelectObject(fileHDC, hSourceBmp);
	SelectObject(memHDC, memBmp);
	for (int w = 0; w < width; w++)
		for (int h = 0; h < height; h++)
		{
			oldColor = GetPixel(fileHDC, w, h);
			BYTE red = GetRValue(oldColor);
			BYTE green = GetGValue(oldColor);
			BYTE blue = GetBValue(oldColor);
			BYTE swap = green;
			green = blue;
			blue = swap;
			SetPixelV(fileHDC, w, h, RGB(red, green, blue));
			ret = 1;
		}
	BitBlt(memHDC, 0, 0, width, height, fileHDC, 0, 0, SRCCOPY);
	SaveBmp2File(memBmp, memHDC, destBmp);
	return ret;
}

void AddText2Bitmap(char *sourceBmp, char *text, char *destBmp)
{
	HBITMAP hSourceBmp = (HBITMAP)LoadImage(
    NULL,   // handle to instance
    sourceBmp,  // name or identifier of the image .say"C:\\NewFolder\\1.bmp"
    IMAGE_BITMAP,        // image types
    0,     // desired width
    0,     // desired height
    LR_LOADFROMFILE);
	BITMAP bmp;
	GetObject(hSourceBmp, sizeof(BITMAP), (LPSTR)&bmp);
	
	TEXTMETRIC tm;
	
	HDC hdcScreen = GetDC(NULL)/*CreateDC("DISPLAY", NULL, NULL, NULL)*/; 
	GetTextMetrics(hdcScreen, &tm);
	char buf[200];
	/*sprintf(buf, "BITSPIXEL %d", GetDeviceCaps(
  hdcScreen,     
  BITSPIXEL   
));*/
	sprintf(buf, "BITSPIXEL %s %d", sourceBmp, bmp.bmBitsPixel);
	MessageBox(NULL, buf, "BP", 0);
	int width = CalcLen(hdcScreen, text);
	int height = tm.tmHeight;
	HDC txtDC = CreateCompatibleDC(hdcScreen);
	HDC maskDC = CreateCompatibleDC(hdcScreen);
	HBITMAP maskBmp = CreateCompatibleBitmap(hdcScreen, width, height);
	HBITMAP txtBmp = CreateCompatibleBitmap(hdcScreen, width, height);
	SelectObject(txtDC, txtBmp);
	SelectObject(maskDC, maskBmp);
	RECT r;
	r.left = 0;
	r.top = 0;
	r.right = width;
	r.bottom = height;
	SetTextColor(txtDC, RGB(0, 255, 0));
	SetBkColor(txtDC, RGB(0, 0, 0));
	DrawText(txtDC, text, strlen(text), &r, 0);

	SetTextColor(maskDC, RGB(0, 0, 0));
	SetBkColor(maskDC, RGB(255, 255, 255));
	DrawText(maskDC, text, strlen(text), &r, 0);
	
	if (bmp.bmWidth > width)
	{
  		width = bmp.bmWidth;
	}
	if (bmp.bmHeight > tm.tmHeight)
	{
		height = bmp.bmHeight;
	}
	HDC fileHDC = CreateCompatibleDC(hdcScreen);
	HDC memHDC = CreateCompatibleDC(hdcScreen);
	
	HBITMAP memBmp = Create24BPPDIBSection(hdcScreen, width, height);//CreateCompatibleBitmap(hdcScreen, width, height);
	SelectObject(fileHDC, hSourceBmp);
	SelectObject(memHDC, memBmp);
	HBRUSH hBrush = CreateSolidBrush(GetPixel(fileHDC, 0, 0));
	SelectObject(memHDC, hBrush);
	FloodFill(memHDC, 0, 0, GetPixel(fileHDC, 0, 0));
	BitBlt(memHDC, 0, 0, width, height, fileHDC, 0, 0, SRCCOPY);
	BitBlt(memHDC, 0, 0, r.right, r.bottom, maskDC, 0, 0, SRCAND);
	BitBlt(memHDC, 0, 0, r.right, r.bottom, txtDC, 0, 0, SRCINVERT);
	SaveBmp2File(memBmp, memHDC, destBmp);
	/*PBITMAPINFO pbmi = CreateBitmapInfoStruct(NULL, memBmp);
    CreateBMPFile(NULL, destBmp, pbmi, memBmp, memHDC);*/
}