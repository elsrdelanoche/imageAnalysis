#include "model/ImageModel.h"
#include <iostream>

ImageModel::ImageModel() 
    : original_image_(nullptr), transformed_image_(nullptr) {}

bool ImageModel::loadImage(const std::string& filepath) {
    try {
        original_image_ = Gdk::Pixbuf::create_from_file(filepath);
        transformed_image_ = original_image_->copy();
        current_filepath_ = filepath;
        return true;
    } catch (const Glib::Error& ex) {
        std::cerr << "Error al cargar imagen: " << ex.what() << std::endl;
        return false;
    }
}

bool ImageModel::saveImage(const std::string& filepath) {
    if (!transformed_image_) return false;
    
    try {
        std::string ext = filepath.substr(filepath.find_last_of(".") + 1);
        if (ext == "jpg" || ext == "jpeg") {
            transformed_image_->save(filepath, "jpeg");
        } else {
            transformed_image_->save(filepath, "png");
        }
        return true;
    } catch (const Glib::Error& ex) {
        std::cerr << "Error al guardar imagen: " << ex.what() << std::endl;
        return false;
    }
}

void ImageModel::setTransformedImage(Glib::RefPtr<Gdk::Pixbuf> image) {
    transformed_image_ = image;
}

void ImageModel::resetToOriginal() {
    if (original_image_) {
        transformed_image_ = original_image_->copy();
    }
}

int ImageModel::getWidth() const {
    return original_image_ ? original_image_->get_width() : 0;
}

int ImageModel::getHeight() const {
    return original_image_ ? original_image_->get_height() : 0;
}
