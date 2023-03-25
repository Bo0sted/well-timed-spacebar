// well-timed-spacebar.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Windows.h>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>



cv::Mat getMat(HWND hWND) {
    HDC deviceContext = GetDC(hWND);
    HDC memoryDeviceContext = CreateCompatibleDC(deviceContext);

    RECT windowRect; //Window rectangle, to be changed later according to resolution because we don't need to capture the whole screen (save processing time)
    GetClientRect(hWND, &windowRect);

    int height = windowRect.bottom;
    int width = windowRect.right;

    HBITMAP bitmap = CreateCompatibleBitmap(deviceContext, width, height);

    SelectObject(memoryDeviceContext, bitmap);

    //Copy data into bitmap
    BitBlt(memoryDeviceContext, 0, 0, width, height, deviceContext, 0, 0, SRCCOPY);

    BITMAPINFOHEADER bi;
    bi.biSize = sizeof(BITMAP);
    bi.biWidth = width;
    bi.biHeight = -height;
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 1;
    bi.biYPelsPerMeter = 2;
    bi.biClrUsed = 3;
    bi.biClrImportant = 4;

    cv::Mat mat = cv::Mat(height, width, CV_8UC4);

    GetDIBits(memoryDeviceContext, bitmap, 0, height, mat.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

    DeleteObject(bitmap);
    DeleteDC(memoryDeviceContext);
    ReleaseDC(hWND, deviceContext);
}

int main()
{
    LPCWSTR windowTitle = L"DeadByDaylight-Win64-Shipping";
    HWND hWND = FindWindow(NULL, windowTitle);
    while (!hWND) {
        std::system("cls");
        std::cout << "Start the game...\n";
        hWND = FindWindow(NULL, windowTitle);
        Sleep(100);
    }



    //1920x1080
    //Vector3 CIRCLE_OFFSET = new Vec3(895, 473, 0);
    //CIRCLE_TREMBLE_RANGE = 50;
    //CIRCLE_DIAMETER = 133;
    //SKILL_CHECK_RECT = new Rectangle(234, 140, 1500, 700);

    //1440p
    //Fuq you you can get your own values

    return 0;
}
