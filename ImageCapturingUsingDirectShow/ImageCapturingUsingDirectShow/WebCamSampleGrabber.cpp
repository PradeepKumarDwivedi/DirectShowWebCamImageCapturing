#include"WebCamSampleGrabber.h"




WebCamSampleGrabber::WebCamSampleGrabber()
{
	CB = NULL;
	
}

WebCamSampleGrabber::~WebCamSampleGrabber()
{

}

void WebCamSampleGrabber::SaveFrame(BITMAPINFO bi, BYTE *data, unsigned long size)
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
	bih.biHeight = bi.bmiHeader.biHeight;
	bih.biPlanes = 1;
	bih.biBitCount = 32;

	char FileName[256];
	FILE * out;
	sprintf_s(FileName, "capture_%d.bmp", (int)GetTickCount());
	fopen_s(&out,FileName, "wb");
	if (out)
	{
		fwrite(&bfh, sizeof(bfh), 1, out);
		fwrite(&bih, sizeof(bih), 1, out);
		fwrite(data, bufsize, 1, out);
		fclose(out);
	}
}

HRESULT WebCamSampleGrabber::InitMonikersForObjectBindings()
{
	HRESULT hr = S_OK;

	ULONG cFetched;

	ICreateDevEnum *pCreateDevEnum;		// Интерфейс для обнаружения и перечисления всех доступных устройств
	hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void**)&pCreateDevEnum);
	if (FAILED(hr))
	{
		printf("Failed to enumerate all video and audio capture devices!  hr=0x%x\n", hr);
		return hr;
	}

	IEnumMoniker *pEnumMoniker;			// Интерфейс, который будет содержать список всех доступных устройств

										//	Обнаружение подключенный устройств видеозахвата
	hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnumMoniker, 0);
	if (FAILED(hr) || !pEnumMoniker)
	{
		printf("Failed to create ClassEnumerator!  hr=0x%x\n", hr);
		return -1;
	}

	// Выбор активного устройства в фильтр pMonikerVideo. Мы берем первое из списка для простоты
	hr = pEnumMoniker->Next(1, &pMonikerVideo, &cFetched);
	if (S_OK == hr)
	{
		// Соединяем устройство с нашим фильтром захвата видео
		hr = pMonikerVideo->BindToObject(0, 0, IID_IBaseFilter, (void**)&pVideoCaptureFilter);
		if (FAILED(hr))
		{
			printf("Couldn't bind moniker to filter object!  hr=0x%x\n", hr);
			return hr;
		}
	}
	pEnumMoniker->Release();
	return hr;

}
HRESULT WebCamSampleGrabber::GetAllInterfaces(void)
{
	HRESULT hr = S_OK;

	// Создание графа фильтров
	hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC,
		IID_IGraphBuilder, (void **)&pGraphBuilder);
	if (FAILED(hr))
		return hr;

	// Создание графа захвата видео
	hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC,
		IID_ICaptureGraphBuilder2, (void **)&pCaptureGraphBuilder2);
	if (FAILED(hr))
		return hr;

	// Получим интерфейсы для управления графом и окном
	hr = pGraphBuilder->QueryInterface(IID_IMediaControl, (LPVOID *)&pMediaControl);
	if (FAILED(hr))
		return hr;


	hr = pGraphBuilder->QueryInterface(IID_IVideoWindow, (LPVOID *)&pVideoWindow);
	if (FAILED(hr))
		return hr;

	hr = pGraphBuilder->QueryInterface(IID_IMediaEvent, (LPVOID *)&pMediaEvent);
	if (FAILED(hr))
		return hr;


	// ------------------------
	// Create the Sample Grabber.

	hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER,
		IID_IBaseFilter, (void**)&pGrabberF);
	if (FAILED(hr))
	{
		return hr;
		// Return an error.
	}

	hr = pGrabberF->QueryInterface(IID_ISampleGrabber, (void**)&pSampleGrabber);
	if (FAILED(hr)) std::cout << "\nERROR: QueryInterface";

	//----------------------

	return hr;
}
HRESULT WebCamSampleGrabber::CaptureVideo()
{
	return S_OK;
}
void WebCamSampleGrabber::StopPreview()
{

}
void WebCamSampleGrabber::CloseInterfaces(void)
{
	// Остановить видео
	if (pMediaControl)
		pMediaControl->StopWhenReady();
	//psCurrent = Stopped;

	// Закрытие окна
	if (pVideoWindow) pVideoWindow->put_Visible(OAFALSE);

	// Закрытие остальных интерфейсов
	pMediaControl->Release();
	pGraphBuilder->Release();
	pVideoWindow->Release();
	pCaptureGraphBuilder2->Release();
}

HRESULT WebCamSampleGrabber::ConnectFilters(IGraphBuilder *pGraph, IPin *pOut, IBaseFilter *pDest)
{
	if ((pGraph == NULL) || (pOut == NULL) || (pDest == NULL))
	{
		return E_POINTER;
	}
#ifdef debug
	PIN_DIRECTION PinDir;
	pOut->QueryDirection(&PinDir);
	_ASSERTE(PinDir == PINDIR_OUTPUT);
#endif

	// Find an input pin on the downstream filter.
	IPin *pIn = 0;
	HRESULT hr = GetUnconnectedPin(pDest, PINDIR_INPUT, &pIn);
	if (FAILED(hr))
	{
		return hr;
	}
	// Try to connect them.
	hr = pGraph->Connect(pOut, pIn);
	pIn->Release();
	return hr;
}
HRESULT WebCamSampleGrabber::GetUnconnectedPin(IBaseFilter *pFilter, PIN_DIRECTION PinDir, IPin **ppPin)
{
	*ppPin = 0;
	IEnumPins *pEnum = 0;
	IPin *pPin = 0;
	HRESULT hr = pFilter->EnumPins(&pEnum);
	if (FAILED(hr))
	{
		return hr;
	}
	while (pEnum->Next(1, &pPin, NULL) == S_OK)
	{
		PIN_DIRECTION ThisPinDir;
		pPin->QueryDirection(&ThisPinDir);
		if (ThisPinDir == PinDir)
		{
			IPin *pTmp = 0;
			hr = pPin->ConnectedTo(&pTmp);
			if (SUCCEEDED(hr))  // Already connected, not the pin we want.
			{
				pTmp->Release();
			}
			else  // Unconnected, this is the pin we want.
			{
				pEnum->Release();
				*ppPin = pPin;
				return S_OK;
			}
		}
		pPin->Release();
	}
	pEnum->Release();
	// Did not find a matching pin.
	return E_FAIL;

}

HRESULT WebCamSampleGrabber::ConnectFilters(IGraphBuilder *pGraph, IBaseFilter *pSrc, IBaseFilter *pDest)
{
	if ((pGraph == NULL) || (pSrc == NULL) || (pDest == NULL))
	{
		return E_POINTER;
	}

	// Find an output pin on the first filter.
	IPin *pOut = 0;
	HRESULT hr = GetUnconnectedPin(pSrc, PINDIR_OUTPUT, &pOut);
	if (FAILED(hr))
	{
		return hr;
	}
	hr = ConnectFilters(pGraph, pOut, pDest);
	pOut->Release();
	return hr;
}

HRESULT WebCamSampleGrabber::InitializeAndStartCapturing()
{
	HRESULT hr = CoInitialize(NULL);
	hr = GetAllInterfaces();
	if (FAILED(hr))
	{
		printf("Failed to get video interfaces!  hr=0x%x\n", hr);
		return hr;
	}

	hr = pCaptureGraphBuilder2->SetFiltergraph(pGraphBuilder);
	if (FAILED(hr))
	{
		printf("Failed to attach the filter graph to the capture graph!  hr=0x%x\n", hr);
		return hr;
	}

	

	hr = InitMonikersForObjectBindings();
	if (FAILED(hr))
	{
		//MessageBox(GetActiveWindow(), _T
		return hr;
	}

	hr = pGraphBuilder->AddFilter(pVideoCaptureFilter, L"Video Capture");
	if (FAILED(hr))
	{
		printf("Couldn't add video capture filter to graph!  hr=0x%x\n", hr);
		pVideoCaptureFilter->Release();
		return hr;
	}

	AM_MEDIA_TYPE mt;
	ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
	mt.majortype = MEDIATYPE_Video;
	mt.subtype = MEDIASUBTYPE_RGB32;
	hr = pSampleGrabber->SetMediaType(&mt);
	hr = pSampleGrabber->SetBufferSamples(TRUE);

	hr = pGraphBuilder->AddFilter(pGrabberF, L"Sample Grabber");
	if (FAILED(hr))
	{
		printf("Couldn't add sample grabber to graph!  hr=0x%x\n", hr);
		// Return an error.
	}
	hr = pCaptureGraphBuilder2->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, pVideoCaptureFilter, pGrabberF, 0);
	if (FAILED(hr))
	{
		printf("Couldn't render video capture stream. The device may already be in use.  hr=0x%x\n", hr);
		pVideoCaptureFilter->Release();
		return hr;
	}


	hr = pSampleGrabber->GetConnectedMediaType(&mt);
	if (FAILED(hr))
	{
		//GrabInfo(  TEXT("Unable to determine what we connected.") );
		return -1;
	}
	VIDEOINFOHEADER * vih = (VIDEOINFOHEADER*)mt.pbFormat;
	

	CB = new CSampleGrabberCB(vih);
	if (!FAILED(hr))
	{
		CB->Width = vih->bmiHeader.biWidth;
		CB->Height = vih->bmiHeader.biHeight;
		//FreeMediaType( mt );
	}
	hr = pSampleGrabber->SetCallback(CB, 1);
	

	pVideoCaptureFilter->Release();

	
	//SetupVideoWindow();

	
	hr = pMediaControl->Run();
	if (FAILED(hr))
	{
		printf("Couldn't run the graph!  hr=0x%x\n", hr);
		return hr;
	}
}