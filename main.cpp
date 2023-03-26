// well-timed-spacebar.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Windows.h>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

const double THRESHOLD = 0.9; //threshold for "is this the skill check window?"

cv::Mat getMat(HWND hWND) {
    HDC deviceContext = GetDC(hWND);
    HDC memoryDeviceContext = CreateCompatibleDC(deviceContext);

    int height, width;
    HBITMAP bitmap;
    if (true) { //DBD window
        bool nineteentwenty = true; //Current resolution workaround without having to do a bunch more stuff
        if (nineteentwenty) {
            height = 285;
            width = 253;
            bitmap = CreateCompatibleBitmap(deviceContext, width, height);
            SelectObject(memoryDeviceContext, bitmap);
            //Copy data into bitmap
            BitBlt(memoryDeviceContext, 0, 0, width, height, deviceContext, 846, 405, SRCCOPY);
        }
        else {
            height = 300;
            width = 350;
            bitmap = CreateCompatibleBitmap(deviceContext, width, height);
            SelectObject(memoryDeviceContext, bitmap);
            BitBlt(memoryDeviceContext, 0, 0, width, height, deviceContext, 1194, 635, SRCCOPY);
        }
    }
    else { //Other debug window
        RECT windowRect; //Window rectangle, to be changed later according to resolution because we don't need to capture the whole screen (save processing time)
        GetClientRect(hWND, &windowRect);
        height = windowRect.bottom;
        width = windowRect.right;

        bitmap = CreateCompatibleBitmap(deviceContext, width, height);
        SelectObject(memoryDeviceContext, bitmap);
        //Copy data into bitmap
        BitBlt(memoryDeviceContext, 0, 0, width, height, deviceContext, 0, 0, SRCCOPY);
    }

    BITMAPINFOHEADER bi;
    bi.biSize = sizeof(BITMAPINFOHEADER);
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

    return mat;
}

int main()
{
    LPCWSTR windowTitle = L"DeadByDaylight  ";
    //LPCWSTR windowTitle = L"Window title";
    HWND hWND = FindWindow(NULL, windowTitle);

    bool init = false;
    while (!hWND) { //If window not found
        if (!init) {
            std::cout << "Start the game";
            init = true;
        }
        std::cout << ".";
        hWND = FindWindow(NULL, windowTitle);
        Sleep(333);
    }

    std::cout << "\nFound window\n";

    cv::Mat background;
    cv::namedWindow("output", cv::WINDOW_NORMAL); //Create the output window, debug purposes.

    std::string filePath = cv::samples::findFile("spacebar.png"); //"Needle" to find in the window
    cv::Mat needle = cv::imread(filePath, cv::IMREAD_UNCHANGED);
    cv::cvtColor(needle, needle, cv::COLOR_BGR2RGBA); //marchTemplate requires grayscale so we convert to gray
    cv::Mat img, copyofimg, result; //Make a copy to keep for later
    cv::Point minLoc, /*Minimum location*/ maxLoc; //Max location, which we will use as basis for "is this the skill check?"
    double minVal, maxVal; //Min and max values
    while (true) {
        img = getMat(hWND); //Get the window

        img.copyTo(copyofimg);
        cv::cvtColor(copyofimg, copyofimg, cv::COLOR_BGR2RGBA); //marchTemplate requires grayscale so we convert to gray

        cv::matchTemplate(copyofimg, needle, result, cv::TM_CCOEFF_NORMED, cv::Mat());
        //cv::normalize(result, result, 0, 1, cv::NORM_MINMAX, -1, cv::Mat()); //Normalize the data, might need to use?
        
        cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat()); //Find the min and max values of the result from matchTemplate 
        std::cout << minVal << " " << maxVal << "\n"; //DEBUG PURPOSES: To see the minimum value and maximum value. Used to fine tune the threshold
        if (maxVal >= THRESHOLD) { //"Found"
            std::cout << "Found\n";
            cv::rectangle(img, maxLoc, cv::Point(maxLoc.x + needle.cols, maxLoc.y + needle.rows), cv::Scalar(0, 255, 0), 2, 8, 0); //We draw a rectangle around the found image on original "img"
        }

        img.copyTo(background); //Copy to background
        cv::imshow("output", background); //Show the output
        cv::waitKey(33); //33ms means 30fps. 50ms would be 20fps. 100ms would be 10fps. 10ms would be 100fps
        //Note: Most likely limited by computing power, not this.
    }

    //You must either CTRL+C the console window or close the console window.

    //1920x1080
    //Vector3 CIRCLE_OFFSET = new Vec3(895, 473, 0);
    //CIRCLE_TREMBLE_RANGE = 50;
    //CIRCLE_DIAMETER = 133;

    //1440p

    //Fuq you you can get your own values
    //Vector3 CIRCLE_OFFSET = new Vec3(1194, 635, 0);
    //CIRCLE_DIAMETER = 164;
    cv::destroyWindow("output");

    return 0;
}
