#include "stdafx.h"
#include "../include/visual/NativeWindow.h"
#include "../include/Application.h"
#include "GDI/GDIRenderCoreProvider.h"

using namespace MPF;
using namespace MPF::Visual;

DEFINE_TYPE(NativeWindow, MPF::Visual::NativeWindow)
const wchar_t MPFWindowClassName[] = L"MPF_Window_Wrapper";

NativeWindow::NativeWindow()
{
	CreateWindowClass();
}

NativeWindow::~NativeWindow()
{
	if (handle)
	{
		DestroyWindow((HWND)handle);
		handle = nullptr;
	}
}

void NativeWindow::CreateWindowClass()
{
	static bool isSucceed = false;
	if (!isSucceed)
	{
		WNDCLASSEX wndCls = { 0 };

		wndCls.cbSize = sizeof(wndCls);
		wndCls.hInstance = (HINSTANCE)Application::GetCurrent()->GetNativeHandle();
		wndCls.style = CS_HREDRAW | CS_VREDRAW;
		wndCls.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wndCls.lpszClassName = MPFWindowClassName;
		wndCls.lpfnWndProc = (WNDPROC)WindowProcWrapper;

		isSucceed = RegisterClassEx(&wndCls) != 0;
	}
	massert(isSucceed);
}

LRESULT WINAPI NativeWindow::WindowProcWrapper(handle_t hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static NativeWindow* window = nullptr;

	if (msg == WM_CREATE)
	{
		//��ȡ this
		auto createParam = (LPCREATESTRUCT)lParam;
		window = (NativeWindow*)createParam->lpCreateParams;
	}

	if (window)
	{
		return window->WindowProc(hWnd, msg, wParam, lParam);
	}
	return DefWindowProc((HWND)hWnd, msg, wParam, lParam);
}

LRESULT WINAPI NativeWindow::WindowProc(handle_t hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc((HWND)hWnd, msg, wParam, lParam);
}

void NativeWindow::Create()
{
	handle = CreateWindowEx(0, MPFWindowClassName, String::GetEmpty()->GetDataPointer(),
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
		nullptr, nullptr, (HINSTANCE)Application::GetCurrent()->GetNativeHandle(), this);

	massert(handle != nullptr);
}

void NativeWindow::Show() const
{
	ShowWindow((HWND)handle, SW_SHOW);
}

std::shared_ptr<String> NativeWindow::GetTitle() const
{
	auto len = GetWindowTextLength((HWND)handle);
	auto text = new wchar_t[len + 1];
	GetWindowText((HWND)handle, text, len);

	return std::make_shared<String>(text, true);
}

void NativeWindow::SetTitle(std::shared_ptr<String> title)
{
	SetWindowText((HWND)handle, title->GetDataPointer());
}

uint NativeWindow::GetWidth() const
{
	RECT rect;
	GetWindowRect((HWND)handle, &rect);

	return rect.right - rect.left;
}

void NativeWindow::SetWidth(uint width)
{
	RECT rect;
	GetWindowRect((HWND)handle, &rect);
	SetWindowPos((HWND)handle, nullptr, rect.left, rect.top, width, rect.bottom - rect.top, NULL);
}

uint NativeWindow::GetHeight() const
{
	RECT rect;
	GetWindowRect((HWND)handle, &rect);

	return rect.bottom - rect.top;
}

void NativeWindow::SetHeight(uint height)
{
	RECT rect;
	GetWindowRect((HWND)handle, &rect);
	SetWindowPos((HWND)handle, nullptr, rect.left, rect.top, rect.right - rect.left, height, NULL);
}

handle_t NativeWindow::GetNativeHandle() const
{
	return handle;
}

std::shared_ptr<RenderCoreProvider> NativeWindow::CreateRenderCoreProvider(RenderCoreProviders provider)
{
	std::shared_ptr<RenderCoreProvider> render = nullptr;

	switch (provider)
	{
	case MPF::Visual::RenderCoreProviders::GDI:
		render = std::make_shared<GDI::GDIRenderCoreProvider>(shared_from_this());
		break;
	default:
		break;
	}
	return render;
}

uint NativeWindow::GetClientWidth() const
{
	RECT rect;

	GetClientRect((HWND)handle, &rect);
	return rect.right - rect.left;
}

uint NativeWindow::GetClientHeight() const
{
	RECT rect;

	GetClientRect((HWND)handle, &rect);
	return rect.bottom - rect.top;
}