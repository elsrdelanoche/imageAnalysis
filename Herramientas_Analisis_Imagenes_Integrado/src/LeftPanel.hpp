#pragma once
#include <gtkmm.h>

// Panel lateral izquierdo:
// muestra el nombre de la herramienta activa y, opcionalmente,
// un área dinámica de parámetros (sliders, botones, etc.).
class LeftPanel : public Gtk::Frame {
public:
    LeftPanel();

    void set_current_tool(const Glib::ustring& name);

    // Reemplaza el contenido de parámetros por el widget dado.
    // El widget debe estar gestionado (Gtk::manage / make_managed).
    void set_param_widget(Gtk::Widget* widget);
    void clear_params();

private:
    Gtk::Box  m_box;       // contenedor vertical
    Gtk::Label m_title;
    Gtk::Label m_tool;
    Gtk::Separator m_sep;
    Gtk::Box  m_paramBox;  // aquí se colocan los widgets de parámetros
};
