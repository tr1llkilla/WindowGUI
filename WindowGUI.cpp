/*
Copyright (C) 2024 tr1llkilla
    This program comes with ABSOLUTELY NO WARRANTY.
    This is free software, and you are welcome to redistribute it
    under legal conditions.
Author:
tr1llkilla

Author's note:
This program is unfinished, but does the trick to get the job done manually
Feel free to credit this work in any future registry forensic or data analysis works!
*/

#include <Windows.h>
#include <string>
#include <bcrypt.h>
#include <vector>
#include <cmath>

#pragma comment(lib, "bcrypt.lib")

// Global variables
HINSTANCE hInst;
HWND hwndLoginButton, hwndCreateAccountButton, hwndUsername, hwndPassword;
HWND hwndRegisterUsername, hwndRegisterPassword, hwndRegisterButton;
HBITMAP hBitmap; // Declare hBitmap globally
std::wstring registeredUsername = L"username"; // Replace with your registered username
std::wstring registeredPassword = L"password"; // Replace with your registered password
int animationFrame = 0; // Global variable for animation


// Registration window variables
HWND registerHwnd;
WNDCLASS registerWc;
int registerBoxX, registerBoxY, registerBoxWidth, registerBoxHeight;

// Function declarations
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK RegisterWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
std::vector<BYTE> HashPassword(const std::wstring& password);

// Function to hash the password using bcrypt
std::vector<BYTE> HashPassword(const std::wstring& password);

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Store the instance handle
    hInst = hInstance;

    // Load custom icon
    HICON hIcon = LoadIcon(hInstance, IDI_APPLICATION);

    // Load custom cursor
    HCURSOR hCursor = LoadCursor(hInstance, IDC_ARROW);

    // Load image data from file
    hBitmap = (HBITMAP)LoadImage(hInstance, L"PATH\\TO\\FILE.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    if (hBitmap == NULL) {
        // Display an error message with additional information
        DWORD error = GetLastError();

        LPVOID errorMsg = nullptr;
        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
            NULL,
            error,
            0, // Default language
            (LPWSTR)&errorMsg,
            0,
            NULL
        );

        MessageBox(NULL, (L"Failed to load the image! Error code: " + std::to_wstring(error) + L"\n" + (LPCWSTR)errorMsg).c_str(), L"Error", MB_ICONERROR);

        // Free the allocated buffer
        LocalFree(errorMsg);

        return 1;
    }

    // Create a brush with the loaded bitmap
    HBRUSH hbrBackground = CreatePatternBrush(hBitmap);

    // Register the main window class
    WNDCLASS wc = {};
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = hIcon;
    wc.hCursor = hCursor;
    wc.hbrBackground = hbrBackground;
    wc.lpszClassName = L"MyWindowClass";
    RegisterClass(&wc);

    // Create the main window
    HWND hwnd = CreateWindowEx(0, L"MyWindowClass", L"My Window", WS_OVERLAPPEDWINDOW,
        0, 0, 1000, 1000, 0, 0, hInstance, 0);

    // Calculate center positions
    int windowWidth = 1000;
    int windowHeight = 1000;
    int loginBoxWidth = 300;
    int loginBoxHeight = 160;

    int loginBoxX = (windowWidth - loginBoxWidth) / 2;
    int loginBoxY = (windowHeight - loginBoxHeight) / 2;

    // Calculate center positions for login controls
    int usernameX = loginBoxX + 50;
    int usernameY = loginBoxY + 40;
    int passwordX = loginBoxX + 50;
    int passwordY = loginBoxY + 70;

    // Calculate center positions for buttons
    int loginButtonX = loginBoxX + (loginBoxWidth - 2 * 100 - 10) / 2;
    int loginButtonY = loginBoxY + 110;
    int createAccountButtonX = loginButtonX + 110;
    int createAccountButtonY = loginBoxY + 110;

    // Create "Log In" button
    hwndLoginButton = CreateWindowEx(0, L"BUTTON", L"Log In", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        loginButtonX, loginButtonY, 100, 30, hwnd, (HMENU)1, hInst, 0);

    // Create "Create Account" button
    hwndCreateAccountButton = CreateWindowEx(0, L"BUTTON", L"Create Account", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        createAccountButtonX, createAccountButtonY, 150, 30, hwnd, (HMENU)2, hInst, 0);

    // Create username and password controls
    hwndUsername = CreateWindowEx(0, L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER,
        usernameX, usernameY, 200, 20, hwnd, 0, hInst, 0);

    hwndPassword = CreateWindowEx(0, L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_PASSWORD,
        passwordX, passwordY, 200, 20, hwnd, 0, hInst, 0);

    // Show the main window
    ShowWindow(hwnd, nCmdShow);

    // Main message loop
    MSG msg = {};
    while (GetMessage(&msg, 0, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Cleanup
    DeleteObject(hBitmap);
    DeleteObject(hbrBackground);

    return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        if (LOWORD(wParam) == 1) // Log In button clicked
        {
            // Get the entered username and password
            int usernameLength = GetWindowTextLength(hwndUsername) + 1;
            int passwordLength = GetWindowTextLength(hwndPassword) + 1;
            std::wstring username(usernameLength, L'\0');
            std::wstring password(passwordLength, L'\0');
            GetWindowText(hwndUsername, &username[0], usernameLength);
            GetWindowText(hwndPassword, &password[0], passwordLength);

            // Compare the entered username and password with the stored values
            if (username == registeredUsername && password == registeredPassword)
            {
                // Hide login and register controls
                ShowWindow(hwndLoginButton, SW_HIDE);
                ShowWindow(hwndCreateAccountButton, SW_HIDE);
                ShowWindow(hwndUsername, SW_HIDE);
                ShowWindow(hwndPassword, SW_HIDE);

                // Show the .bmp image (modify the coordinates accordingly)
                hBitmap = (HBITMAP)LoadImage(hInst, L"A:\\Cadell Anderson\\Pictures\\Website Images\\spacepunk DJ_files\\basketballcourtprototype.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
                HBRUSH hbrBackground = CreatePatternBrush(hBitmap);
                SendMessage(hwnd, WM_CTLCOLORSTATIC, (WPARAM)GetDC(hwnd), (LPARAM)hbrBackground);
                ReleaseDC(hwnd, (HDC)GetDC(hwnd));
                SetTimer(hwnd, 1, 50, NULL); // Adjust the timer interval as needed
            }
            else
            {
                MessageBox(hwnd, L"Login failed. Please check your credentials.", L"Error", MB_OK | MB_ICONERROR);
            }
        }
        else if (LOWORD(wParam) == 2) // Create Account button clicked
        {
            // Open the registration window
            RegisterWndProc(NULL, WM_CREATE, 0, 0);
        }
        break;

    case WM_TIMER:
        if (wParam == 1)
        {
            // Handle animation here
            // For example, update animationFrame and redraw the image
            animationFrame++;
            RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
        }
        break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // Paint the image with animation
        // You can use animationFrame to adjust the displayed frame of the image
        RECT clientRect;
        GetClientRect(hwnd, &clientRect);

        // Create a compatible device context
        HDC hdcMem = CreateCompatibleDC(hdc);

        // Select the bitmap into the memory device context
        HGDIOBJ hOld = SelectObject(hdcMem, hBitmap);

        // Calculate the position and size of the image
        int imageWidth = 1000 /* specify the width of your image */;
        int imageHeight = 1000 /* specify the height of your image */;
        int imageX = (clientRect.right - imageWidth) / 2;
        int imageY = (clientRect.bottom - imageHeight) / 2;

        // Draw the image to the window
        BitBlt(hdc, imageX, imageY, imageWidth, imageHeight, hdcMem, 0, 0, SRCCOPY);

        // Clean up
        SelectObject(hdcMem, hOld);
        DeleteDC(hdcMem);

        EndPaint(hwnd, &ps);
    }
    break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK RegisterWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        // Initialize variables
        registerBoxWidth = 300;
        registerBoxHeight = 160;

        registerBoxX = (400 - registerBoxWidth) / 2;
        registerBoxY = (200 - registerBoxHeight) / 2;

        // Create the registration window class
        ZeroMemory(&registerWc, sizeof(WNDCLASS));
        registerWc.style = CS_HREDRAW | CS_VREDRAW;
        registerWc.lpfnWndProc = RegisterWndProc;
        registerWc.cbClsExtra = 0;
        registerWc.cbWndExtra = 0;
        registerWc.hInstance = hInst;
        registerWc.hIcon = LoadIcon(hInst, IDI_APPLICATION);
        registerWc.hCursor = LoadCursor(hInst, IDC_ARROW);
        registerWc.hbrBackground = CreatePatternBrush(hBitmap);
        registerWc.lpszClassName = L"RegisterWindowClass";
        RegisterClass(&registerWc);

        // Create the registration window
        registerHwnd = CreateWindowEx(0, L"RegisterWindowClass", L"Register Account", WS_OVERLAPPEDWINDOW,
            0, 0, 400, 200, 0, 0, hInst, 0);

        // Create username and password controls in the registration window
        hwndRegisterUsername = CreateWindowEx(0, L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER,
            registerBoxX + 50, registerBoxY + 40, 200, 20, registerHwnd, 0, hInst, 0);

        hwndRegisterPassword = CreateWindowEx(0, L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_PASSWORD,
            registerBoxX + 50, registerBoxY + 70, 200, 20, registerHwnd, 0, hInst, 0);

        hwndRegisterButton = CreateWindowEx(0, L"BUTTON", L"Register", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            registerBoxX + 100, registerBoxY + 110, 100, 30, registerHwnd, (HMENU)3, hInst, 0);

        // Show the registration window
        ShowWindow(registerHwnd, SW_SHOWNORMAL);
        UpdateWindow(registerHwnd);
        break;

    case WM_COMMAND:
        if (LOWORD(wParam) == 3) // Register button clicked
        {
            // Get the entered username and password
            int registerUsernameLength = GetWindowTextLength(hwndRegisterUsername) + 1;
            int registerPasswordLength = GetWindowTextLength(hwndRegisterPassword) + 1;
            std::wstring registerUsername(registerUsernameLength, L'\0');
            std::wstring registerPassword(registerPasswordLength, L'\0');
            GetWindowText(hwndRegisterUsername, &registerUsername[0], registerUsernameLength);
            GetWindowText(hwndRegisterPassword, &registerPassword[0], registerPasswordLength);

            // Store the registered username and password
            registeredUsername = registerUsername;
            registeredPassword = registerPassword;

            // Hash the password before storing it (as in the existing code)
            std::vector<BYTE> hashedPassword = HashPassword(registerPassword);

            // Simulate storing the hashed password in a database
            // In a real scenario, you should use a secure storage solution
            MessageBox(NULL, (L"User Account Created!\nUsername: " + registerUsername + L"\nPassword Hash: " + std::to_wstring(hashedPassword.size())).c_str(), L"Success", MB_OK);
        }
        break;

    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}

// Function to hash the password using bcrypt
std::vector<BYTE> HashPassword(const std::wstring& password)
{
    BCRYPT_ALG_HANDLE hAlg = NULL;
    BCRYPT_HASH_HANDLE hHash = NULL;
    NTSTATUS status;

    status = BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_SHA256_ALGORITHM, NULL, 0);
    if (BCRYPT_SUCCESS(status))
    {
        status = BCryptCreateHash(hAlg, &hHash, NULL, 0, NULL, 0, 0);
        if (BCRYPT_SUCCESS(status))
        {
            // Convert password to bytes
            const BYTE* passwordBytes = reinterpret_cast<const BYTE*>(password.c_str());

            // Calculate the length of the password in bytes
            ULONG passwordSize = static_cast<ULONG>(password.length() * sizeof(wchar_t));

            status = BCryptHashData(hHash, reinterpret_cast<PUCHAR>(const_cast<wchar_t*>(password.c_str())), passwordSize, 0);
            if (BCRYPT_SUCCESS(status))
            {
                // Get the hash value
                DWORD hashSize = 0;
                DWORD result;
                status = BCryptGetProperty(hAlg, BCRYPT_HASH_LENGTH, reinterpret_cast<BYTE*>(&hashSize), sizeof(hashSize), &result, 0);
                if (BCRYPT_SUCCESS(status))
                {
                    std::vector<BYTE> hash(hashSize);
                    status = BCryptFinishHash(hHash, hash.data(), hash.size(), 0);
                    if (BCRYPT_SUCCESS(status))
                    {
                        // Return the hashed password
                        return hash;
                    }
                }
            }

            BCryptDestroyHash(hHash);
        }

        BCryptCloseAlgorithmProvider(hAlg, 0);
    }

    // Return an empty vector in case of failure
    return std::vector<BYTE>();
}
