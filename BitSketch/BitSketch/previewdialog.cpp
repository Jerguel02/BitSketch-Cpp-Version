#include "previewdialog.h"
#include <QWheelEvent>
#include <QMouseEvent>

PreviewDialog::PreviewDialog(const QImage& image, QWidget* parent)
    : QDialog(parent), image(image), scaleFactor(1.0), isDragging(false) {
    setWindowTitle("Preview");
    setFixedSize(800, 600);

    imageLabel = new QLabel(this);
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setPixmap(QPixmap::fromImage(image));

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(imageLabel);
    setLayout(layout);
}

PreviewDialog::~PreviewDialog() {}

void PreviewDialog::wheelEvent(QWheelEvent* event) {
    if (event->angleDelta().y() > 0) {
        scaleFactor *= 1.1;
    } else {
        scaleFactor /= 1.1;
    }
    updateImageLabel();
}

void PreviewDialog::updateImageLabel() {
    QImage scaledImage = image.scaled(image.size() * scaleFactor, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    imageLabel->setPixmap(QPixmap::fromImage(scaledImage));
    imageLabel->resize(scaledImage.size());
}

void PreviewDialog::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        isDragging = true;
        lastMousePosition = event->pos();
    }
}

void PreviewDialog::mouseMoveEvent(QMouseEvent* event) {
    if (isDragging) {
        QPoint delta = event->pos() - lastMousePosition;
        imageLabel->move(imageLabel->pos() + delta);
        lastMousePosition = event->pos();
    }
}

void PreviewDialog::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        isDragging = false;
    }
}
