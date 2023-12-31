﻿// SampleMultiWindow.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include <pch.h>
#include <MyApp.h>

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    
    auto ml = NativeWindows::MyApp::GetInstance();
    ml->Start();
    delete ml;

    return 0;
}


