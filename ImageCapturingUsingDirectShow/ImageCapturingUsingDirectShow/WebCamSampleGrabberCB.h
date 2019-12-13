#pragma once

#include <windows.h>
#include <dshow.h> // «аголовочный файл DirectShow
#include "DirectShowIncludes\DX\qedit.h"
#include <iostream>
#include <objbase.h>
#include "DirectShowIncludes\BaseClasses\streams.h"
#include <strmif.h>
#include <list>
//#include <ws2tcpip.h>

#define WIN32_LEAN_AND_MEAN

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

typedef struct
{
	HWND hWnd;
	HDC hDoubleBuffer;
	HDC hDoubleBufferWorker;
	BITMAPINFO BitmapInfo;
	HBITMAP hCompatBitmap;
	HBITMAP hOldCompatBitmap;
	HGDIOBJ hOldBitMap;
	HBITMAP hDBBitMap;
	UCHAR *pBuffer;
	DWORD dwScreenX;
	DWORD dwScreenY;
	PALETTEENTRY VirtualPalette[256];
	DWORD dwOptimizedPaletteLookUp[256];
	CRITICAL_SECTION csRepaintBlock;
}GDI, *PGDI;



class CSampleGrabberCB : public ISampleGrabberCB
{
public:
	// Ёти параметры устанавливаютс€ главным потоком
	DWORD lastTime;
	long Width;
	long Height;
	PGDI m_pGDI;
	HANDLE FrameCaptured;
	//UCHAR *pBufferCopy;
	HANDLE BufferEvent;
	LONGLONG prev, step; // не используетс€

	CSampleGrabberCB(VIDEOINFOHEADER * VideoInfoheader);
	~CSampleGrabberCB();
	void SaveFrame(BITMAPINFO bi, BYTE *data, unsigned long size);

	// Fake out any COM ref counting
	STDMETHODIMP_(ULONG) AddRef() { return 2; }
	STDMETHODIMP_(ULONG) Release() { return 1; }

	
	STDMETHODIMP QueryInterface(REFIID riid, void ** ppv);
	STDMETHODIMP SampleCB(double SampleTime, IMediaSample * pSample);
	STDMETHODIMP BufferCB(double SampleTime, BYTE * pBuffer, long BufferSize);

	std::list<PGDI> m_GlobalImageList;
	std::list<int> m_GlobalImageHeightList;
	std::list<int> m_GlobalImageWidthList;



};
