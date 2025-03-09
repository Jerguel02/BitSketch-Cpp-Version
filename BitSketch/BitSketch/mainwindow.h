#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QPixmap>
#include <QVector>

class PixelArtDialog;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void openImage();
    void saveHex();
    void openPixelEditor();

private:
    void initUI();
    void convertImageToHex();

    QLabel* label;
    QPushButton* openButton;
    QPushButton* saveButton;
    QPushButton* pixelEditorButton;
    QPixmap* image;
    QVector<QVector<QString>> hexData;
    PixelArtDialog* pixelArtDialog;
};

#endif // MAINWINDOW_H
