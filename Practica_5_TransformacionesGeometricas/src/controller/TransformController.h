#ifndef TRANSFORM_CONTROLLER_H
#define TRANSFORM_CONTROLLER_H

#include <memory>
#include <string>
#include "utils/ImageProcessor.h"

class ImageModel;
class MainWindow;

class TransformController {
public:
    TransformController(std::shared_ptr<ImageModel> model, MainWindow* view);
    ~TransformController() = default;

    void loadImage(const std::string& filepath);
    void saveImage(const std::string& filepath);
    void applyTranslation();
    void applyRotation();
    void applyScale();
    void resetImage();

private:
    void updateView();
    
    std::shared_ptr<ImageModel> model_;
    MainWindow* view_;
    std::unique_ptr<ImageProcessor> processor_;
};

#endif
