#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib,"winmm")

#include <windows.h>

#define SRATE	8000	//標本化周波数(1秒間のサンプル数)
#define F		800		//周波数(1秒間の波形数)
#define SPAN	200		//長点か短点か判定(ミリ秒)

TCHAR szClassName[] = TEXT("Window");
HWND hEdit;
WNDPROC ButtonWndProc;
HWAVEOUT hWaveOut;
WAVEHDR whdr;

struct map {
	TCHAR c;
	LPCTSTR pMorseCode;
};

struct map pattern[] = {
	{ TEXT('あ'), TEXT("－－・－－") },
	{ TEXT('い'), TEXT("・－") },
	{ TEXT('う'), TEXT("・・－") },
	{ TEXT('え'), TEXT("－・－－－") },
	{ TEXT('お'), TEXT("・－・・・") },
	{ TEXT('か'), TEXT("・－・・") },
	{ TEXT('き'), TEXT("－・－・・") },
	{ TEXT('く'), TEXT("・・・－") },
	{ TEXT('け'), TEXT("－・－－") },
	{ TEXT('こ'), TEXT("－－－－") },
	{ TEXT('さ'), TEXT("－・－・－") },
	{ TEXT('し'), TEXT("－－・－・") },
	{ TEXT('す'), TEXT("－－－・－") },
	{ TEXT('せ'), TEXT("・－－－・") },
	{ TEXT('そ'), TEXT("－－－・") },
	{ TEXT('た'), TEXT("－・") },
	{ TEXT('ち'), TEXT("・・－・") },
	{ TEXT('つ'), TEXT("・－－・") },
	{ TEXT('て'), TEXT("・－・－－") },
	{ TEXT('と'), TEXT("・・－・・") },
	{ TEXT('な'), TEXT("・－・") },
	{ TEXT('に'), TEXT("－・－・") },
	{ TEXT('ぬ'), TEXT("・・・・") },
	{ TEXT('ね'), TEXT("－－・－") },
	{ TEXT('の'), TEXT("・・－－") },
	{ TEXT('は'), TEXT("－・・・") },
	{ TEXT('ひ'), TEXT("－－・・－") },
	{ TEXT('ふ'), TEXT("－－・・") },
	{ TEXT('へ'), TEXT("・") },
	{ TEXT('ほ'), TEXT("－・・") },
	{ TEXT('ま'), TEXT("－・・－") },
	{ TEXT('み'), TEXT("・・－・－") },
	{ TEXT('む'), TEXT("－") },
	{ TEXT('め'), TEXT("－・・・－") },
	{ TEXT('も'), TEXT("－・・－・") },
	{ TEXT('や'), TEXT("・－－") },
	{ TEXT('ゆ'), TEXT("－・・－－") },
	{ TEXT('よ'), TEXT("－－") },
	{ TEXT('ら'), TEXT("・・・") },
	{ TEXT('り'), TEXT("－－・") },
	{ TEXT('る'), TEXT("－・－－・") },
	{ TEXT('れ'), TEXT("－－－") },
	{ TEXT('ろ'), TEXT("・－・－") },
	{ TEXT('わ'), TEXT("－・－") },
	{ TEXT('ゐ'), TEXT("・－・・－") },
	{ TEXT('ゑ'), TEXT("・－－・・") },
	{ TEXT('を'), TEXT("・－－－") },
	{ TEXT('ん'), TEXT("・－・－・") },
	{ TEXT('ー'), TEXT("・－－・－") },
	{ TEXT('゛'), TEXT("・・") },
	{ TEXT('゜'), TEXT("・・－－・") },
	{ TEXT('、'), TEXT("・－・－・－") },
	{ TEXT('１'), TEXT("・－－－－") },
	{ TEXT('２'), TEXT("・・－－－") },
	{ TEXT('３'), TEXT("・・・－－") },
	{ TEXT('４'), TEXT("・・・・－") },
	{ TEXT('５'), TEXT("・・・・・") },
	{ TEXT('６'), TEXT("－・・・・") },
	{ TEXT('７'), TEXT("－－・・・") },
	{ TEXT('８'), TEXT("－－－・・") },
	{ TEXT('９'), TEXT("－－－－・") },
	{ TEXT('０'), TEXT("－－－－－") },
	{ TEXT('\b'), TEXT("・・・－・") },
};

LRESULT CALLBACK ButtonProc1(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static BOOL bInputFlag;
	static DWORD dwCount;
	static DWORD_PTR dwStartPos;
	static DWORD_PTR dwEndPos;
	switch (msg)
	{
	case WM_KEYDOWN:
		if (wParam != VK_SPACE)break;
		if (HIWORD(lParam)&KF_REPEAT)break;
	case WM_LBUTTONDOWN:
		if (bInputFlag == 0)
		{
			dwStartPos = dwEndPos = SendMessage(hEdit, WM_GETTEXTLENGTH, 0, 0);
			bInputFlag = 1;
		}
		KillTimer(hWnd, 0x1234);
		waveOutWrite(hWaveOut, &whdr, sizeof(WAVEHDR));
		dwCount = GetTickCount();
		break;
	case WM_KEYUP:
		if (wParam != VK_SPACE)break;
	case WM_LBUTTONUP:
		if (!bInputFlag)break;
		waveOutReset(hWaveOut);
		dwEndPos = SendMessage(hEdit, WM_GETTEXTLENGTH, 0, 0);
		SendMessage(hEdit, EM_SETSEL, (WPARAM)dwEndPos, (LPARAM)dwEndPos);
		SendMessage(hEdit, EM_REPLACESEL,
			0,
			(LPARAM)((GetTickCount() - dwCount > SPAN) ? TEXT("－") : TEXT("・")));
		dwEndPos++;
		SendMessage(hEdit, EM_SETSEL, (WPARAM)dwStartPos, (LPARAM)dwEndPos);
		SetTimer(hWnd, 0x1234, 500, 0);
		break;
	case WM_TIMER:
		KillTimer(hWnd, 0x1234);
		{
			DWORD dwSize = GetWindowTextLength(hEdit);
			if (dwSize)
			{
				TCHAR szInput[2] = { 0 };
				LPTSTR lpszMorseCode = (LPTSTR)GlobalAlloc(
					0, sizeof(TCHAR)*(dwSize + 1));
				GetWindowText(hEdit, lpszMorseCode, dwSize + 1);
				for (int i = 0; i < sizeof(pattern) / sizeof(map); i++)
				{
					if (CSTR_EQUAL == CompareString(0, 0,
						lpszMorseCode + dwStartPos
						, lstrlen(lpszMorseCode + dwStartPos),
						pattern[i].pMorseCode,
						lstrlen(pattern[i].pMorseCode)))
					{
						szInput[0] = pattern[i].c;
						break;
					}
				}
				if (dwStartPos&&szInput[0] == TEXT('゛'))
				{
					switch (lpszMorseCode[dwStartPos - 1])
					{
					case TEXT('か'):
					case TEXT('き'):
					case TEXT('く'):
					case TEXT('け'):
					case TEXT('こ'):
					case TEXT('さ'):
					case TEXT('し'):
					case TEXT('す'):
					case TEXT('せ'):
					case TEXT('そ'):
					case TEXT('た'):
					case TEXT('ち'):
					case TEXT('つ'):
					case TEXT('て'):
					case TEXT('と'):
					case TEXT('は'):
					case TEXT('ひ'):
					case TEXT('ふ'):
					case TEXT('へ'):
					case TEXT('ほ'):
						szInput[0] = lpszMorseCode[--dwStartPos] + 1;
						break;
					}
				}
				else if (dwStartPos&&szInput[0] == TEXT('゜'))
				{
					switch (lpszMorseCode[dwStartPos - 1])
					{
					case TEXT('は'):
					case TEXT('ひ'):
					case TEXT('ふ'):
					case TEXT('へ'):
					case TEXT('ほ'):
						szInput[0] = lpszMorseCode[--dwStartPos] + 2;
						break;
					}
				}
				else if (szInput[0] == TEXT('\b'))
				{
					if (dwStartPos)dwStartPos--;
					szInput[0] = 0;
				}
				SendMessage(hEdit, EM_SETSEL, dwStartPos, dwEndPos);
				SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szInput);
				GlobalFree(lpszMorseCode);
			}
			bInputFlag = 0;
		}
		break;
	case WM_DESTROY:
		KillTimer(hWnd, 0x1234);
		break;
	default:
		break;
	}
	return CallWindowProc(ButtonWndProc, hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	WAVEFORMATEX wfe;
	int i, len;
	static HWND hButton;
	static LPBYTE lpWave;
	switch (msg)
	{
	case WM_CREATE:
		wfe.wFormatTag = WAVE_FORMAT_PCM;
		wfe.nChannels = 1;
		wfe.wBitsPerSample = 8;
		wfe.nBlockAlign = wfe.nChannels*wfe.wBitsPerSample / 8;
		wfe.nSamplesPerSec = SRATE;
		wfe.nAvgBytesPerSec = wfe.nSamplesPerSec*wfe.nBlockAlign;
		waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfe, 0, 0, CALLBACK_NULL);
		lpWave = (LPBYTE)GlobalAlloc(GMEM_ZEROINIT, wfe.nAvgBytesPerSec * 2);
		len = SRATE / F;
		for (i = 0; i < SRATE * 2; i++) {
			if (i%len < len / 2)lpWave[i] = 128 + 64;
			else lpWave[i] = 128 - 64;
		}
		whdr.lpData = (LPSTR)lpWave;
		whdr.dwBufferLength = wfe.nAvgBytesPerSec * 2;
		whdr.dwFlags = WHDR_BEGINLOOP | WHDR_ENDLOOP;
		whdr.dwLoops = -1;
		waveOutPrepareHeader(hWaveOut, &whdr, sizeof(WAVEHDR));
		waveOutSetVolume(hWaveOut, 0x6FFF6FFF);
		hEdit = CreateWindowEx(
			WS_EX_CLIENTEDGE,
			TEXT("EDIT"),
			0,
			WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL | ES_READONLY,
			10, 10, 512, 64,
			hWnd,
			(HMENU)100,
			((LPCREATESTRUCT)lParam)->hInstance,
			0);
		hButton = CreateWindow(
			TEXT("BUTTON"),
			TEXT("モールス"),
			WS_VISIBLE | WS_CHILD,
			10, 84, 128, 32,
			hWnd,
			(HMENU)100,
			((LPCREATESTRUCT)lParam)->hInstance,
			0);
		SetClassLongPtr(hButton,
			GCL_STYLE,
			GetClassLong(hButton,
				GCL_STYLE) & ~CS_DBLCLKS);
		ButtonWndProc = (WNDPROC)SetWindowLongPtr(
			hButton, GWLP_WNDPROC, (LONG_PTR)ButtonProc1);
		break;
	case WM_SETFOCUS:
		SetFocus(hButton);
		break;
	case WM_DESTROY:
		waveOutReset(hWaveOut);
		waveOutUnprepareHeader(hWaveOut, &whdr, sizeof(WAVEHDR));
		waveOutClose(hWaveOut);
		GlobalFree(lpWave);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR pCmdLine, int nCmdShow)
{
	MSG msg;
	WNDCLASS wndclass = {
		CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0,
		0,
		hInstance,
		0,
		LoadCursor(0,IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		0,
		szClassName
	};
	RegisterClass(&wndclass);
	HWND hWnd = CreateWindow(
		szClassName,
		TEXT("モールス信号で日本語入力"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
		0,
		0,
		hInstance,
		0
	);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}
