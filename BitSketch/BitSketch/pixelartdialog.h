#ifndef PIXELARTDIALOG_H
#define PIXELARTDIALOG_H

#include <QMainWindow>
#include <QPushButton>
#include <QSpinBox>
#include <QScrollArea>
#include <QScrollBar>
#include <QCheckBox>
#include <QLabel>
#include <QColor>
#include <QImage>
#include <QPixmap>
#include <QPainter>
#include <QTimer>


class PixelArtDialog : public QMainWindow {
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
    void redo();
    void zoomIn(const QPoint& zoomPoint = QPoint());
    void zoomOut(const QPoint& zoomPoint = QPoint());
    void savePixelDesign();
    void previewImage();

private:
    struct PixelChange {
        int x;
        int y;
        QColor color;
    };

    void initUI();
    void createPixelGrid();
    void updatePixelSizes();
    void saveStateToHistory(int x, int y, QColor color);
    void saveAsImage(const QString& path);
    void saveAsHex(const QString& path);

    int pixelSize;
    int gridWidth;
    int gridHeight;
    bool isDrawing;
    QColor selectedColor;
    QImage pixelImage;
    QPixmap pixelPixmap;
    QLabel* pixelLabel;
    QVector<PixelChange> history;
    QVector<PixelChange> redoHistory;
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
    QPainter* painter;
    QTimer* updateTimer;
    QVector<PixelChange> pendingUpdates;
};

#endif
