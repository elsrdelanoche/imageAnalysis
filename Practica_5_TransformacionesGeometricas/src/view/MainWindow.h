#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <gtkmm.h>
#include <memory>

class TransformController;

class MainWindow : public Gtk::Window {
public:
    MainWindow();
    virtual ~MainWindow() = default;

    void setController(std::shared_ptr<TransformController> controller);
    void displayImage(Glib::RefPtr<Gdk::Pixbuf> pixbuf);
    void updateStatus(const std::string& message);
    
    double getTranslationX() const;
    double getTranslationY() const;
    double getRotationAngle() const;
    double getScaleX() const;
    double getScaleY() const;
    int getInterpolationType() const;

protected:
    void onOpenImage();
    void onSaveImage();
    void onApplyTranslation();
    void onApplyRotation();
    void onApplyScale();
    void onResetImage();
    void onQuit();

private:
    void setupUI();
    void createMenuBar();
    void createToolbar();
    void createMainArea();
    void createTransformPanel();
    
    std::shared_ptr<TransformController> controller_;
    
    Gtk::Box main_box_;
    Gtk::Box content_box_;
    Gtk::ScrolledWindow scrolled_window_;
    Gtk::Image image_display_;
    Gtk::Statusbar statusbar_;
    
    Gtk::Box transform_panel_;
    Gtk::Frame translation_frame_;
    Gtk::Frame rotation_frame_;
    Gtk::Frame scale_frame_;
    
    Gtk::SpinButton spin_tx_;
    Gtk::SpinButton spin_ty_;
    Gtk::Button btn_apply_translation_;
    
    Gtk::SpinButton spin_angle_;
    Gtk::Button btn_apply_rotation_;
    
    Gtk::SpinButton spin_sx_;
    Gtk::SpinButton spin_sy_;
    Gtk::ComboBoxText combo_interpolation_;
    Gtk::Button btn_apply_scale_;
    
    Gtk::Button btn_reset_;
};

#endif
