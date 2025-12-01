#pragma once
#include <gtkmm.h>
#include <vector>

class CanvasArea : public Gtk::DrawingArea {
public:
    CanvasArea();
    virtual ~CanvasArea() = default;

    void add_image(const Glib::RefPtr<Gdk::Pixbuf>& pix);
    Glib::RefPtr<Gdk::Pixbuf> get_active_image() const;
    void push_result_image(const Glib::RefPtr<Gdk::Pixbuf>& pix);

protected:
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
    bool on_button_press_event(GdkEventButton* event) override;
    bool on_button_release_event(GdkEventButton* event) override;
    bool on_motion_notify_event(GdkEventMotion* event) override;

private:
    struct Item {
        Glib::RefPtr<Gdk::Pixbuf> pix;
        double x = 0.0;
        double y = 0.0;
    };

    std::vector<Item> items_;
    int active_index_ = -1;
    bool dragging_ = false;
    double drag_off_x_ = 0.0;
    double drag_off_y_ = 0.0;

    int hit_test(double px, double py) const;
    void bring_to_front(int index);
    void show_context_menu(double x, double y, int index);
    void save_item_as(int index);
    void delete_item(int index);
};
