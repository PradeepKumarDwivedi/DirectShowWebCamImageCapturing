#pragma once


#include "WebCamSampleGrabberCB.h"
// ����� ����� ������ ������� ���� � ������������
#define DEFAULT_VIDEO_WIDTH     320
#define DEFAULT_VIDEO_HEIGHT    240

class WebCamSampleGrabber
{

private:
	// ���������� DirectShow
	//IMediaControl * pMediaControl = NULL;					// ���������� ������
	IMediaEvent *pMediaEvent = NULL;
	//IGraphBuilder *pGraphBuilder = NULL;					// ��� ���� ������
	//ICaptureGraphBuilder2 *pCaptureGraphBuilder2 = NULL;	// ���� ������� �����
	IVideoWindow *pVideoWindow = NULL;						// ����, � ������� ��������� ����� �����
	IMoniker *pMonikerVideo = NULL;							// ���������� ������������
	/*IBaseFilter *pVideoCaptureFilter = NULL;*/				// ������ ������������
	//IBaseFilter *pGrabberF = NULL;
	/*ISampleGrabber *pSampleGrabber = NULL;*/

public:
	CSampleGrabberCB * CB;
	IGraphBuilder * pGraphBuilder = NULL;					// ��� ���� ������
	ICaptureGraphBuilder2 *pCaptureGraphBuilder2 = NULL;	// ���� ������� �����
	IBaseFilter *pVideoCaptureFilter = NULL;
	ISampleGrabber *pSampleGrabber = NULL;
	IBaseFilter *pGrabberF = NULL;
	IMediaControl * pMediaControl = NULL;					// ���������� ������
	

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




