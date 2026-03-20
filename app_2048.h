#pragma once
#include <windows.h>
#include <string>
#include <vector>

// Identyfikator dla opcji menu
#define ID_OPTIONS_PICKCOLOR 1001

class app_2048
{
private:
    HINSTANCE m_instance;
    HWND m_main;
    COLORREF m_selected_color; // Przechowuje wybrany kolor

    bool register_class();
    static std::wstring const s_class_name;

    static LRESULT CALLBACK window_proc_static(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
    LRESULT window_proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

    HWND create_window();
    void create_menu(HWND window);
    void draw_gradient(HWND window);

public:
    app_2048(HINSTANCE instance);
    int run(int show_command);
};