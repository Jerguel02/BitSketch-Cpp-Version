#ifndef PIXELARTDIALOG_H
#define PIXELARTDIALOG_H

#include <QDialog>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPushButton>
#include <QSpinBox>
#include <QScrollArea>
#include <QCheckBox>
#include <QLabel>
#include <QColor>
#include <QVector>

class QGraphicsRectItem;

class PixelArtDialog : public QDialog {
    Q_OBJECT

public:
    explicit PixelArtDialog(QWidget* parent = nullptr);
    ~PixelArtDialog();

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private slots:
    void applySize();
    void openImage();
    void chooseColor();
    void undo();
    void zoomIn();
    void zoomOut();
    void savePixelDesign();
    void previewImage();

private:
    void initUI();
    void createPixelGrid();
    void updatePixelSizes();
    void saveStateToHistory();
    void saveAsImage(const QString& path);
    void saveAsHex(const QString& path);

    int pixelSize;
    int gridWidth;
    int gridHeight;
    bool isDrawing;
    QColor selectedColor;
    QVector<QVector<QGraphicsRectItem*>> pixelMatrix;
    QVector<QVector<QColor>> history;
    QGraphicsScene* scene;
    QGraphicsView* view;
    QScrollArea* scrollArea;
    QSpinBox* widthInput;
    QSpinBox* heightInput;
    QPushButton* applySizeButton;
    QPushButton* openImageButton;
    QPushButton* colorButton;
    QPushButton* undoButton;
    QPushButton* saveDesignButton;
    QPushButton* zoomInButton;
    QPushButton* zoomOutButton;
    QPushButton* previewButton;
    QCheckBox* coordinateCheckbox;
    QLabel* coordinatesLabel;
};

#endif // PIXELARTDIALOG_H
