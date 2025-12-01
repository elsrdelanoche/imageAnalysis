#pragma once
#include <gtkmm.h>
#include "CanvasArea.hpp"
#include "LeftPanel.hpp"
#include "AppController.hpp"

class AppWindow : public Gtk::Window {
public:
    AppWindow();

private:
    Gtk::Box   m_vbox;
    Gtk::MenuBar m_menubar;
    Gtk::Paned m_paned;

    LeftPanel  m_leftPanel;
    CanvasArea m_canvas;

    AppController m_controller;

    void build_menus();
};
