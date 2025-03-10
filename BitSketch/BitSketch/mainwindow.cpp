#include "mainwindow.h"
#include "pixelartdialog.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QTextStream>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), image(nullptr), pixelArtDialog(nullptr) {
    initUI();
    showMaximized();
}

MainWindow::~MainWindow() {
    delete image;
}

void MainWindow::initUI() {
    setWindowTitle("BitSketch");

    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout* layout = new QVBoxLayout(centralWidget);

    label = new QLabel("Select image to convert to hex code", this);
    openButton = new QPushButton("Select image", this);
    saveButton = new QPushButton("Save HEX code", this);
    pixelEditorButton = new QPushButton("Edit pixel", this);

    layout->addWidget(label);
    layout->addWidget(openButton);
    layout->addWidget(saveButton);
    layout->addWidget(pixelEditorButton);

    connect(openButton, &QPushButton::clicked, this, &MainWindow::openImage);
    connect(saveButton, &QPushButton::clicked, this, &MainWindow::saveHex);
    connect(pixelEditorButton, &QPushButton::clicked, this, &MainWindow::openPixelEditor);
}

void MainWindow::openImage() {
    QString fileName = QFileDialog::getOpenFileName(this, "Select image", "", "Image Files (*.png *.jpg *.bmp)");
    if (!fileName.isEmpty()) {
        delete image;
        image = new QPixmap(fileName);
        label->setPixmap(*image);
    }
}

void MainWindow::convertImageToHex() {
    if (image) {
        hexData.clear();
        QImage img = image->toImage();
        for (int y = 0; y < img.height(); ++y) {
            QVector<QString> row;
            for (int x = 0; x < img.width(); ++x) {
                QColor color = img.pixelColor(x, y);
                quint16 rgb565 = ((color.red() & 0xF8) << 8) | ((color.green() & 0xFC) << 3) | (color.blue() >> 3);
                row.append(QString("0x%1").arg(rgb565, 4, 16, QChar('0')));
            }
            hexData.append(row);
        }
        QMessageBox::information(this, "Notification!", "Image has been converted to hex code.");
    }
}

void MainWindow::saveHex() {
    convertImageToHex();
    if (hexData.isEmpty()) {
        QMessageBox::warning(this, "Warning!", "No hex code data to save.");
        return;
    }

    QString savePath = QFileDialog::getSaveFileName(this, "Save hex code", "", "Hex Files (*.txt)");
    if (!savePath.isEmpty()) {
        QFile file(savePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << "const uint16_t epd_bitmap_images [] PROGMEM = {\n";
            for (const auto& row : hexData) {
                QStringList rowList = QStringList(row.begin(), row.end());
                out << rowList.join(", ") << ",\n";
            }
            out << "};\n";
            file.close();
            QMessageBox::information(this, "Notification!", QString("Saved hex code: %1").arg(savePath));
        }
    }
}

void MainWindow::openPixelEditor() {
    pixelArtDialog = new PixelArtDialog(this);
    pixelArtDialog->show();
}
