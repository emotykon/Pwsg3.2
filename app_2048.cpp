#include "app_2048.h"
#include <stdexcept>
#pragma comment(lib, "Msimg32.lib") // Wymagane dla GradientFill

std::wstring const app_2048::s_class_name{ L"2048 Window" };

app_2048::app_2048(HINSTANCE instance)
    : m_instance{ instance }, m_main{}, m_selected_color{ RGB(0, 128, 255) } // Domyœlny kolor
{
    register_class();
    m_main = create_window();
    create_menu(m_main);
}

bool app_2048::register_class()
{
    WNDCLASSEXW desc{ .cbSize = sizeof(WNDCLASSEXW) };
    if (GetClassInfoExW(m_instance, s_class_name.c_str(), &desc) != 0) return true;

    desc = {
        .cbSize = sizeof(WNDCLASSEXW),
        .lpfnWndProc = window_proc_static,
        .hInstance = m_instance,
        .hCursor = LoadCursorW(nullptr, L"IDC_ARROW"),
        .hbrBackground = (HBRUSH)(COLOR_WINDOW + 1),
        .lpszClassName = s_class_name.c_str()
    };
    return RegisterClassExW(&desc) != 0;
}

HWND app_2048::create_window()
{
    return CreateWindowExW(0, s_class_name.c_str(), L"2048 Gradient",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, 600, 400,
        nullptr, nullptr, m_instance, this);
}

void app_2048::create_menu(HWND window)
{
    HMENU h_menu = CreateMenu();
    HMENU h_sub_menu = CreatePopupMenu();
    AppendMenuW(h_sub_menu, MF_STRING, ID_OPTIONS_PICKCOLOR, L"Pick Color");
    AppendMenuW(h_menu, MF_POPUP, (UINT_PTR)h_sub_menu, L"Options");
    SetMenu(window, h_menu);
}

void app_2048::draw_gradient(HWND window)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(window, &ps);

    RECT rect;
    GetClientRect(window, &rect);

    TRIVERTEX vertex[2];

    // Lewy wierzcho³ek (kolor wybrany)
    vertex[0].x = 0;
    vertex[0].y = 0;
    vertex[0].Red = GetRValue(m_selected_color) << 8;
    vertex[0].Green = GetGValue(m_selected_color) << 8;
    vertex[0].Blue = GetBValue(m_selected_color) << 8;
    vertex[0].Alpha = 0x0000;

    // Prawy wierzcho³ek (np. kolor czarny lub inny)
    vertex[1].x = rect.right;
    vertex[1].y = rect.bottom;
    vertex[1].Red = 0x0000; // Mo¿esz tu ustawiæ inny kolor koñcowy
    vertex[1].Green = 0x0000;
    vertex[1].Blue = 0x0000;
    vertex[1].Alpha = 0x0000;

    GRADIENT_RECT g_rect{ 0, 1 };

    // Zmiana na GRADIENT_FILL_RECT_H dla kierunku poziomego
    GradientFill(hdc, vertex, 2, &g_rect, 1, GRADIENT_FILL_RECT_H);

    EndPaint(window, &ps);
}

LRESULT app_2048::window_proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
    switch (message)
    {
    case WM_COMMAND:
        if (LOWORD(wparam) == ID_OPTIONS_PICKCOLOR)
        {
            CHOOSECOLORW cc{ sizeof(cc) };
            static COLORREF cust_colors[16];
            cc.hwndOwner = window;
            cc.lpCustColors = cust_colors;
            cc.rgbResult = m_selected_color;
            cc.Flags = CC_FULLOPEN | CC_RGBINIT;

            if (ChooseColorW(&cc))
            {
                m_selected_color = cc.rgbResult;
                InvalidateRect(window, nullptr, TRUE); // Odœwie¿ okno
            }
        }
        return 0;

    case WM_PAINT:
        draw_gradient(window);
        return 0;

    case WM_SIZE:
        InvalidateRect(window, nullptr, TRUE);
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(window, message, wparam, lparam);
}

LRESULT app_2048::window_proc_static(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
    app_2048* app = nullptr;
    if (message == WM_NCCREATE)
    {
        app = static_cast<app_2048*>(reinterpret_cast<LPCREATESTRUCTW>(lparam)->lpCreateParams);
        SetWindowLongPtrW(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(app));
    }
    else
    {
        app = reinterpret_cast<app_2048*>(GetWindowLongPtrW(window, GWLP_USERDATA));
    }
    return app ? app->window_proc(window, message, wparam, lparam) : DefWindowProcW(window, message, wparam, lparam);
}

int app_2048::run(int show_command)
{
    ShowWindow(m_main, show_command);
    MSG msg{};
    while (GetMessageW(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return (int)msg.wParam;
}