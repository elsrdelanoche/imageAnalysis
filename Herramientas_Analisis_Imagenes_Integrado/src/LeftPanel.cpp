#include "LeftPanel.hpp"

LeftPanel::LeftPanel()
: Gtk::Frame("Herramienta activa"),
  m_box(Gtk::ORIENTATION_VERTICAL),
  m_paramBox(Gtk::ORIENTATION_VERTICAL) {

    m_title.set_text("Actual:");
    m_title.set_xalign(0.0);

    m_tool.set_text("Ninguna");
    m_tool.set_xalign(0.0);

    set_shadow_type(Gtk::SHADOW_ETCHED_IN);

    m_box.set_margin_top(8);
    m_box.set_margin_bottom(8);
    m_box.set_margin_start(8);
    m_box.set_margin_end(8);
    m_box.set_spacing(6);

    m_paramBox.set_spacing(4);

    m_box.pack_start(m_title, Gtk::PACK_SHRINK);
    m_box.pack_start(m_tool, Gtk::PACK_SHRINK);
    m_box.pack_start(m_sep, Gtk::PACK_SHRINK);
    m_box.pack_start(m_paramBox, Gtk::PACK_EXPAND_WIDGET);

    add(m_box);
    show_all_children();
}

void LeftPanel::set_current_tool(const Glib::ustring& name) {
    m_tool.set_text(name);
}

void LeftPanel::set_param_widget(Gtk::Widget* widget) {
    // Elimina widgets anteriores
    auto children = m_paramBox.get_children();
    for (auto* child : children) {
        m_paramBox.remove(*child);
    }
    if (widget) {
        m_paramBox.pack_start(*widget, Gtk::PACK_EXPAND_WIDGET);
    }
    show_all_children();
}

void LeftPanel::clear_params() {
    auto children = m_paramBox.get_children();
    for (auto* child : children) {
        m_paramBox.remove(*child);
    }
    show_all_children();
}
