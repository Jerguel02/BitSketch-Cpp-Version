#ifndef PREVIEWDIALOG_H
#define PREVIEWDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>
#include <QImage>
#include <QPixmap>
#include <QPoint>

class PreviewDialog : public QDialog {
    Q_OBJECT

public:
    explicit PreviewDialog(const QImage& image, QWidget* parent = nullptr);
    ~PreviewDialog();

protected:
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    void updateImageLabel();

    QImage image;
    qreal scaleFactor;
    QPoint lastMousePosition;
    bool isDragging;
    QLabel* imageLabel;
};

#endif // PREVIEWDIALOG_H
