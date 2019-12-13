
#include "WebCamSampleGrabberCB.h"

CSampleGrabberCB::CSampleGrabberCB(VIDEOINFOHEADER * VideoInfoheader)
{
	lastTime = 0;
	m_pGDI = NULL;
	m_pGDI = (PGDI)LocalAlloc(LMEM_ZEROINIT, sizeof(GDI));
	m_pGDI ->pBuffer = (UCHAR *)malloc(4000000);//1228800 Original Size
	m_pGDI->dwScreenX = VideoInfoheader->bmiHeader.biWidth;
	m_pGDI->dwScreenY = VideoInfoheader->bmiHeader.biHeight;
	m_pGDI->BitmapInfo.bmiHeader.biSize = VideoInfoheader->bmiHeader.biSize;
	m_pGDI->BitmapInfo.bmiHeader.biBitCount = VideoInfoheader->bmiHeader.biBitCount;
	m_pGDI->BitmapInfo.bmiHeader.biHeight = VideoInfoheader->bmiHeader.biHeight;
	m_pGDI->BitmapInfo.bmiHeader.biWidth = VideoInfoheader->bmiHeader.biWidth;
	m_pGDI->BitmapInfo.bmiHeader.biCompression = BI_RGB;
	m_pGDI->BitmapInfo.bmiHeader.biPlanes = 1;
	//pBufferCopy = (UCHAR *)malloc(4000000);//1228800 Original Size

	FrameCaptured = CreateEvent(NULL, false, false, NULL);
	
}

CSampleGrabberCB::~CSampleGrabberCB()
{

}

void CSampleGrabberCB::SaveFrame(BITMAPINFO bi, BYTE *data, unsigned long size)
{
	DWORD bufsize = size;
	//m_bSaveToFile = false;
	BITMAPFILEHEADER bfh;
	memset(&bfh, 0, sizeof(bfh));
	bfh.bfType = 'MB';
	bfh.bfSize = sizeof(bfh) + bufsize + sizeof(BITMAPINFOHEADER);
	bfh.bfOffBits = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER);

	DWORD Written = 0;

	// Write the bitmap format			
	BITMAPINFOHEADER bih;
	memset(&bih, 0, sizeof(bih));
	bih.biSize = sizeof(bih);
	bih.biWidth = bi.bmiHeader.biWidth;
	bih.biHeight =  -bi.bmiHeader.biHeight;
	bih.biPlanes = 1;
	bih.biBitCount = 32;
	
	char FileName[256];
	FILE * out;
	sprintf_s(FileName, "CapturedImages\\Capture_%d.bmp", (int)GetTickCount());
	fopen_s(&out, FileName, "wb");
	if (out)
	{
		fwrite(&bfh, sizeof(bfh), 1, out);
		fwrite(&bih, sizeof(bih), 1, out);
		fwrite(data, bufsize, 1, out);
		fclose(out);
	}
}

// Fake out any COM QI'ing
STDMETHODIMP CSampleGrabberCB::QueryInterface(REFIID riid, void ** ppv)
{
	//CheckPointer(ppv, E_POINTER);

	if (riid == IID_ISampleGrabberCB || riid == IID_IUnknown)
	{
		*ppv = (void *) static_cast<ISampleGrabberCB*> (this);
		return NOERROR;
	}

	return E_NOINTERFACE;
}
// Не используется
//
STDMETHODIMP CSampleGrabberCB::SampleCB(double SampleTime, IMediaSample * pSample)
{
	return 0;
}

// Callback ф-ия вызываемая SampleGrabber-ом, в другом потоке
//
STDMETHODIMP CSampleGrabberCB::BufferCB(double SampleTime, BYTE * pBuffer, long BufferSize)
{


	BITMAPFILEHEADER bfh;
	memset(&bfh, 0, sizeof(bfh));
	bfh.bfType = 'MB';
	bfh.bfSize = sizeof(bfh) + BufferSize + sizeof(BITMAPINFOHEADER);
	bfh.bfOffBits = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER);

	DWORD Written = 0;

	// Write the bitmap format

	BITMAPINFOHEADER bih;
	memset(&bih, 0, sizeof(bih));
	bih.biSize = sizeof(bih);
	bih.biWidth = Width;
	bih.biHeight = Height;
	bih.biPlanes = 1;
	bih.biBitCount = 32;

	BITMAPINFO bi;

	bi.bmiHeader = bih;

	DWORD newTime = GetTickCount();


#pragma region This is the logic for making image upside down
	int j = 0;
	for (int i = BufferSize - 4; i > 0; i -= 4)
	{
		m_pGDI->pBuffer[j] = pBuffer[i];
		m_pGDI->pBuffer[j + 1] = pBuffer[i + 1];
		m_pGDI->pBuffer[j + 2] = pBuffer[i + 2];
		m_pGDI->pBuffer[j + 3] = pBuffer[i + 3];
		j += 4;
	}
#pragma endregion

	
	SaveFrame(bi, m_pGDI->pBuffer/*pBuffer*/, BufferSize);
	SetEvent(FrameCaptured);
#pragma region If We Want to Capture frames at some intervals
	/*if (newTime - lastTime > 1000)
	{
		lastTime = newTime;
		SaveFrame(bi, pBuffer, BufferSize);
	}*/
#pragma endregion


	return 0;
}

