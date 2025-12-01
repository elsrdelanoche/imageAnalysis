#include "CanvasArea.hpp"
#include <algorithm>

CanvasArea::CanvasArea() {
    add_events(Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK | Gdk::POINTER_MOTION_MASK);
    set_size_request(800, 600);
}

void CanvasArea::add_image(const Glib::RefPtr<Gdk::Pixbuf>& pix) {
    if (!pix) return;
    Gtk::Allocation alloc = get_allocation();
    int w = alloc.get_width();
    int h = alloc.get_height();
    int iw = pix->get_width();
    int ih = pix->get_height();

    double x = (w > 0) ? (w - iw) / 2.0 : 0.0;
    double y = (h > 0) ? (h - ih) / 2.0 : 0.0;

    Item item;
    item.pix = pix;
    item.x = x;
    item.y = y;
    items_.push_back(item);
    active_index_ = static_cast<int>(items_.size()) - 1;
    queue_draw();
}

Glib::RefPtr<Gdk::Pixbuf> CanvasArea::get_active_image() const {
    if (active_index_ < 0 || active_index_ >= static_cast<int>(items_.size()))
        return Glib::RefPtr<Gdk::Pixbuf>();
    return items_[active_index_].pix;
}

void CanvasArea::push_result_image(const Glib::RefPtr<Gdk::Pixbuf>& pix) {
    if (!pix) return;
    double x = 0.0, y = 0.0;
    if (active_index_ >= 0 && active_index_ < static_cast<int>(items_.size())) {
        x = items_[active_index_].x;
        y = items_[active_index_].y;
    }
    Item item;
    item.pix = pix;
    item.x = x;
    item.y = y;
    items_.push_back(item);
    active_index_ = static_cast<int>(items_.size()) - 1;
    queue_draw();
}

bool CanvasArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr) {
    Gtk::Allocation alloc = get_allocation();
    int w = alloc.get_width();
    int h = alloc.get_height();

    // Fondo gris claro
    cr->set_source_rgb(0.18, 0.18, 0.22);
    cr->rectangle(0, 0, w, h);
    cr->fill();

    // Dibujar imágenes en orden
    for (size_t i = 0; i < items_.size(); ++i) {
        const auto& item = items_[i];
        if (!item.pix) continue;
        Gdk::Cairo::set_source_pixbuf(cr, item.pix, item.x, item.y);
        cr->paint();

        if (static_cast<int>(i) == active_index_) {
            // Borde alrededor de la imagen activa
            cr->set_line_width(2.0);
            cr->set_source_rgba(1.0, 0.8, 0.2, 0.9);
            cr->rectangle(item.x, item.y,
                          item.pix->get_width(),
                          item.pix->get_height());
            cr->stroke();
        }
    }

    return true;
}

int CanvasArea::hit_test(double px, double py) const {
    // Buscar desde arriba hacia abajo
    for (int i = static_cast<int>(items_.size()) - 1; i >= 0; --i) {
        const auto& item = items_[i];
        if (!item.pix) continue;
        int iw = item.pix->get_width();
        int ih = item.pix->get_height();
        if (px >= item.x && px <= item.x + iw &&
            py >= item.y && py <= item.y + ih) {
            return i;
        }
    }
    return -1;
}

void CanvasArea::bring_to_front(int index) {
    if (index < 0 || index >= static_cast<int>(items_.size())) return;
    auto item = items_[index];
    items_.erase(items_.begin() + index);
    items_.push_back(item);
    active_index_ = static_cast<int>(items_.size()) - 1;
}

bool CanvasArea::on_button_press_event(GdkEventButton* event) {
    if (!event) return false;
    if (event->button == 1) { // click izquierdo
        int idx = hit_test(event->x, event->y);
        if (idx >= 0) {
            bring_to_front(idx);
            dragging_ = true;
            drag_off_x_ = event->x - items_[active_index_].x;
            drag_off_y_ = event->y - items_[active_index_].y;
            queue_draw();
            return true;
        }
    } else if (event->button == 3) { // click derecho
        int idx = hit_test(event->x, event->y);
        if (idx >= 0) {
            bring_to_front(idx);
            queue_draw();
            show_context_menu(event->x_root, event->y_root, active_index_);
            return true;
        }
    }
    return false;
}

bool CanvasArea::on_button_release_event(GdkEventButton* event) {
    if (!event) return false;
    if (event->button == 1 && dragging_) {
        dragging_ = false;
        return true;
    }
    return false;
}

bool CanvasArea::on_motion_notify_event(GdkEventMotion* event) {
    if (!event) return false;
    if (dragging_ && active_index_ >= 0 && active_index_ < static_cast<int>(items_.size())) {
        items_[active_index_].x = event->x - drag_off_x_;
        items_[active_index_].y = event->y - drag_off_y_;
        queue_draw();
        return true;
    }
    return false;
}

void CanvasArea::show_context_menu(double x, double y, int index) {
    if (index < 0 || index >= static_cast<int>(items_.size())) return;
    auto menu = Gtk::make_managed<Gtk::Menu>();

    auto item_save = Gtk::make_managed<Gtk::MenuItem>("Guardar imagen como...");
    item_save->signal_activate().connect(
        sigc::bind(sigc::mem_fun(*this, &CanvasArea::save_item_as), index));
    menu->append(*item_save);

    auto item_delete = Gtk::make_managed<Gtk::MenuItem>("Eliminar imagen");
    item_delete->signal_activate().connect(
        sigc::bind(sigc::mem_fun(*this, &CanvasArea::delete_item), index));
    menu->append(*item_delete);

    menu->show_all();
    menu->popup_at_pointer(nullptr);
}



void CanvasArea::delete_item(int index) {
    if (index < 0 || index >= static_cast<int>(items_.size())) return;
    items_.erase(items_.begin() + index);
    if (items_.empty()) {
        active_index_ = -1;
    } else {
        if (active_index_ >= index) {
            active_index_ = std::max(0, active_index_ - 1);
        }
        if (active_index_ >= static_cast<int>(items_.size())) {
            active_index_ = static_cast<int>(items_.size()) - 1;
        }
    }
    queue_draw();
}
void CanvasArea::save_item_as(int index) {
    if (index < 0 || index >= static_cast<int>(items_.size())) return;
    auto pix = items_[index].pix;
    if (!pix) return;

    auto* win = dynamic_cast<Gtk::Window*>(get_toplevel());
    if (!win) return;

    Gtk::FileChooserDialog dialog(*win, "Guardar imagen", Gtk::FILE_CHOOSER_ACTION_SAVE);
    dialog.add_button("_Cancelar", Gtk::RESPONSE_CANCEL);
    dialog.add_button("_Guardar", Gtk::RESPONSE_OK);
    dialog.set_do_overwrite_confirmation(true);

    auto filter_png = Gtk::FileFilter::create();
    filter_png->set_name("Imagen PNG");
    filter_png->add_pattern("*.png");
    dialog.add_filter(filter_png);

    if (dialog.run() == Gtk::RESPONSE_OK) {
        auto filename = dialog.get_filename();
        if (filename.find('.') == std::string::npos) {
            filename += ".png";
        }
        try {
            pix->save(filename, "png");
        } catch (const Glib::Error& ex) {
            // Silencioso por ahora, se podría mostrar un diálogo de error
        }
    }
}
