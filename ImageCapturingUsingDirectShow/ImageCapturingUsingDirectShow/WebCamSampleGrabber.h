#pragma once


#include "WebCamSampleGrabberCB.h"
// Здесь можно задать размеры окна с изображением
#define DEFAULT_VIDEO_WIDTH     320
#define DEFAULT_VIDEO_HEIGHT    240

class WebCamSampleGrabber
{

private:
	// Интерфейсы DirectShow
	//IMediaControl * pMediaControl = NULL;					// Управление графом
	IMediaEvent *pMediaEvent = NULL;
	//IGraphBuilder *pGraphBuilder = NULL;					// Наш граф вцелом
	//ICaptureGraphBuilder2 *pCaptureGraphBuilder2 = NULL;	// Граф захвата видео
	IVideoWindow *pVideoWindow = NULL;						// Окно, в которое выводится видео поток
	IMoniker *pMonikerVideo = NULL;							// Устройство видеозахвата
	/*IBaseFilter *pVideoCaptureFilter = NULL;*/				// Фильтр видеозахвата
	//IBaseFilter *pGrabberF = NULL;
	/*ISampleGrabber *pSampleGrabber = NULL;*/

public:
	CSampleGrabberCB * CB;
	IGraphBuilder * pGraphBuilder = NULL;					// Наш граф вцелом
	ICaptureGraphBuilder2 *pCaptureGraphBuilder2 = NULL;	// Граф захвата видео
	IBaseFilter *pVideoCaptureFilter = NULL;
	ISampleGrabber *pSampleGrabber = NULL;
	IBaseFilter *pGrabberF = NULL;
	IMediaControl * pMediaControl = NULL;					// Управление графом
	

	WebCamSampleGrabber();
	~WebCamSampleGrabber();
	HRESULT InitializeAndStartCapturing();
	HRESULT InitMonikersForObjectBindings();
	HRESULT GetAllInterfaces(void);
	HRESULT GetUnconnectedPin(IBaseFilter *pFilter, PIN_DIRECTION PinDir, IPin **ppPin);
	HRESULT ConnectFilters(IGraphBuilder *pGraph, IPin *pOut, IBaseFilter *pDest);
	HRESULT ConnectFilters(IGraphBuilder *pGraph, IBaseFilter *pSrc, IBaseFilter *pDest);
	HRESULT CaptureVideo();
	void StopPreview();
	void CloseInterfaces(void);
	void SaveFrame(BITMAPINFO bi, BYTE *data, unsigned long size);


};




