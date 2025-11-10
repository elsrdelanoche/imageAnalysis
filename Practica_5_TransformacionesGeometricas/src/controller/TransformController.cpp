#include "controller/TransformController.h"
#include "model/ImageModel.h"
#include "view/MainWindow.h"
#include "utils/ImageProcessor.h"

TransformController::TransformController(std::shared_ptr<ImageModel> model, MainWindow* view)
    : model_(model), view_(view), processor_(std::make_unique<ImageProcessor>()) {}

void TransformController::loadImage(const std::string& filepath) {
    if (model_->loadImage(filepath)) {
        updateView();
        view_->updateStatus("Imagen cargada: " + filepath);
    } else {
        view_->updateStatus("Error al cargar la imagen");
    }
}

void TransformController::saveImage(const std::string& filepath) {
    if (model_->saveImage(filepath)) {
        view_->updateStatus("Imagen guardada: " + filepath);
    } else {
        view_->updateStatus("Error al guardar la imagen");
    }
}

void TransformController::applyTranslation() {
    if (!model_->hasImage()) {
        view_->updateStatus("No hay imagen cargada");
        return;
    }
    
    double tx = view_->getTranslationX();
    double ty = view_->getTranslationY();
    
    auto result = processor_->translate(model_->getTransformedImage(), tx, ty);
    model_->setTransformedImage(result);
    updateView();
    
    view_->updateStatus("Traslación aplicada: tx=" + std::to_string((int)tx) + 
                       ", ty=" + std::to_string((int)ty));
}

void TransformController::applyRotation() {
    if (!model_->hasImage()) {
        view_->updateStatus("No hay imagen cargada");
        return;
    }
    
    double angle = view_->getRotationAngle();
    
    auto result = processor_->rotate(model_->getTransformedImage(), angle);
    model_->setTransformedImage(result);
    updateView();
    
    view_->updateStatus("Rotación aplicada: " + std::to_string((int)angle) + "°");
}

void TransformController::applyScale() {
    if (!model_->hasImage()) {
        view_->updateStatus("No hay imagen cargada");
        return;
    }
    
    double sx = view_->getScaleX();
    double sy = view_->getScaleY();
    int interp_type = view_->getInterpolationType();
    
    Gdk::InterpType interp;
    std::string interp_name;
    
    switch (interp_type) {
        case 0:
            interp = Gdk::INTERP_NEAREST;
            interp_name = "Vecino más cercano";
            break;
        case 1:
            interp = Gdk::INTERP_BILINEAR;
            interp_name = "Bilineal";
            break;
        case 2:
        default:
            interp = Gdk::INTERP_HYPER;
            interp_name = "Bicúbica";
            break;
    }
    
    auto result = processor_->scale(model_->getTransformedImage(), sx, sy, interp);
    model_->setTransformedImage(result);
    updateView();
    
    view_->updateStatus("Escalamiento aplicado: sx=" + std::to_string(sx) + 
                       ", sy=" + std::to_string(sy) + " (" + interp_name + ")");
}

void TransformController::resetImage() {
    if (!model_->hasImage()) {
        view_->updateStatus("No hay imagen cargada");
        return;
    }
    
    model_->resetToOriginal();
    updateView();
    view_->updateStatus("Imagen restaurada al original");
}

void TransformController::updateView() {
    view_->displayImage(model_->getTransformedImage());
}
