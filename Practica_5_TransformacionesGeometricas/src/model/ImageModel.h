#ifndef IMAGE_MODEL_H
#define IMAGE_MODEL_H

#include <gdkmm/pixbuf.h>
#include <glibmm/refptr.h>
#include <string>

class ImageModel {
public:
    ImageModel();
    ~ImageModel() = default;

    bool loadImage(const std::string& filepath);
    bool saveImage(const std::string& filepath);
    
    Glib::RefPtr<Gdk::Pixbuf> getOriginalImage() const { return original_image_; }
    Glib::RefPtr<Gdk::Pixbuf> getTransformedImage() const { return transformed_image_; }
    
    void setTransformedImage(Glib::RefPtr<Gdk::Pixbuf> image);
    void resetToOriginal();
    
    bool hasImage() const { return original_image_.operator bool(); }
    int getWidth() const;
    int getHeight() const;

private:
    Glib::RefPtr<Gdk::Pixbuf> original_image_;
    Glib::RefPtr<Gdk::Pixbuf> transformed_image_;
    std::string current_filepath_;
};

#endif
