
/*
	The entry point of the application.
	TaskbarBlur++ (2019 version)

	This project is a new version of an
	old Win32 project of mine. It's only purpose is
	to make the taskbar have a blur behind it.

	The reason for re-writing this is simply to convert this to
	a command-line utility which is more lightweight, thus more
	suitable as a Windows startup program.

	I am unsure as to whether this would work without
	the AeroGlass (http://www.glass8.eu/) Windows modification.

	-- mikejzx, https://github.com/mikejzx, https://mikejzx.github.io
*/

#include "pch.h"

void cmd_args(int argc, char* argv[]);
void compute();
HRESULT SetWindowBlurStatus(BOOL, HWND);

bool enable_blur = true;
bool wait_flag = false;

// These structs are needed to set 
// composition attributes.
struct WINCOMPATTRDATA { DWORD nAttribute; PVOID pData; ULONG ulDataSize; };
struct ACCENTPOLICY {
	int accentState; int accentFlags; int colour; int animId;
};

// Load these methods to set composition attributes.
typedef BOOL(WINAPI*pSetWindowCompositionAttribute)(HWND, WINCOMPATTRDATA*);
static pSetWindowCompositionAttribute SetWindowCompositionAttribute = (pSetWindowCompositionAttribute)GetProcAddress(GetModuleHandle(TEXT("user32.dll")), "SetWindowCompositionAttribute");
typedef BOOL(WINAPI*pGetWindowCompositionAttribute)(HWND, WINCOMPATTRDATA*);
static pGetWindowCompositionAttribute GetWindowCompositionAttribute = (pGetWindowCompositionAttribute)GetProcAddress(GetModuleHandle(TEXT("user32.dll")), "GetWindowCompositionAttribute");

int main(int argc, char* argv[])
{
	cmd_args(argc, argv);

	compute();

	if (wait_flag)
	{
		std::cout << "Press any key to continue..." << std::endl;
		std::cin.get();
	}
	return 0;
}

void compute()
{
	// Find taskbar window.
	HWND hTaskbar = FindWindowA("Shell_TrayWnd", NULL);
	HWND hTaskbar_Secondary = FindWindowA("Shell_SecondaryTrayWnd", NULL);

	// For window composition attributes.
	ACCENTPOLICY settings = { 3, 2, 0xFFFFFF, 0 }; // 3 is ACCENT_ENABLE_BLURBEHIND

	// (Set second member to zero if disabling.)
	WINCOMPATTRDATA data = { 19, enable_blur ? &settings : 0x0, sizeof(ACCENTPOLICY) };

	// Set blur
	SetWindowBlurStatus(enable_blur ? TRUE : FALSE, hTaskbar);
	SetWindowCompositionAttribute(hTaskbar, &data);
		
	// Apply to second monitor if existant.
	if (hTaskbar_Secondary)
	{
		SetWindowBlurStatus(enable_blur ? TRUE : FALSE, hTaskbar_Secondary);
		SetWindowCompositionAttribute(hTaskbar_Secondary, &data);
	}
}

// Handle command-line arguments.
void cmd_args(int argc, char* argv[])
{
	// No arguments passed in
	if (argc == 1) { return; }

	for (int i = 1; i < argc; ++i)
	{
		if (strcmp(argv[i], "--disable") == 0)
		{
			std::cout << "Running with --disable flag..." << std::endl;
			enable_blur = false;
		}
		else if (strcmp(argv[i], "--wait-for-key") == 0)
		{
			wait_flag = true;
		}
		else if (strcmp(argv[i], "--help") == 0
			|| strcmp(argv[i], "/h") == 0
			|| strcmp(argv[i], "/H") == 0
			|| strcmp(argv[i], "/?") == 0)
		{
			std::cout << std::endl << "    TASKBAR BLUR++ (2019) HELP: " << std::endl;
			std::cout << R"(    Usage: taskbar-blur-2019 [OPTION]

    List of parameters:

        --disable: Instead enabling the blur like it does 
                   by default, the program will *disable* it.

        --wait-for-key: After changing the blur effect, the
                   program will wait for a key press.

        --help:    Displays this help information.

)" << std::endl;
		}
		else
		{
			std::cout << "Invalid parameter, \'" << argv[i] << "\'. Pass --help for list of options." << std::endl;
		}
	}
}

// Handle blurring
HRESULT SetWindowBlurStatus(BOOL enabled, HWND hwnd) {
	HRESULT hr = S_OK;

	DWM_BLURBEHIND bb = { 0 };
	bb.dwFlags = DWM_BB_ENABLE;
	bb.fEnable = enabled;
	bb.hRgnBlur = NULL;

	hr = DwmEnableBlurBehindWindow(hwnd, &bb);

	if (SUCCEEDED(hr)) {
		// Error
		//MessageBox(NULL, L"Error", L"Error", MB_OK);
	}

	return hr;
}