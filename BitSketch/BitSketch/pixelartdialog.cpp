#include "pixelartdialog.h"
#include "previewdialog.h"
#include <QGraphicsRectItem>
#include <QFileDialog>
#include <QMessageBox>
#include <QColorDialog>
#include <QShortcut>
#include <QPainter>
#include <QMouseEvent>
#include <QTextStream>
#include <QDebug>

PixelArtDialog::PixelArtDialog(QWidget* parent)
    : QDialog(parent), pixelSize(10), gridWidth(50), gridHeight(50), isDrawing(false), selectedColor(Qt::red) {
    initUI();
    showMaximized();
}

PixelArtDialog::~PixelArtDialog() {}

void PixelArtDialog::initUI() {
    setWindowTitle("Pixel Art Editor");
    setGeometry(100, 100, 500, 400);
    setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);

    QShortcut* undoShortcut = new QShortcut(QKeySequence("Ctrl+Z"), this);
    connect(undoShortcut, &QShortcut::activated, this, &PixelArtDialog::undo);

    widthInput = new QSpinBox(this);
    widthInput->setRange(1, 1000);
    widthInput->setValue(gridWidth);
    widthInput->setPrefix("Width: ");

    heightInput = new QSpinBox(this);
    heightInput->setRange(1, 1000);
    heightInput->setValue(gridHeight);
    heightInput->setPrefix("Height: ");

    applySizeButton = new QPushButton("Áp dụng kích thước", this);
    connect(applySizeButton, &QPushButton::clicked, this, &PixelArtDialog::applySize);

    openImageButton = new QPushButton("Mở hình ảnh", this);
    connect(openImageButton, &QPushButton::clicked, this, &PixelArtDialog::openImage);

    scene = new QGraphicsScene(this);
    createPixelGrid();

    view = new QGraphicsView(scene, this);
    view->setFixedSize(gridWidth * pixelSize + 1, gridHeight * pixelSize + 1);

    scrollArea = new QScrollArea(this);
    scrollArea->setWidget(view);
    scrollArea->setWidgetResizable(true);

    undoButton = new QPushButton("Undo", this);
    connect(undoButton, &QPushButton::clicked, this, &PixelArtDialog::undo);

    colorButton = new QPushButton("Chọn màu", this);
    connect(colorButton, &QPushButton::clicked, this, &PixelArtDialog::chooseColor);

    saveDesignButton = new QPushButton("Lưu thiết kế", this);
    connect(saveDesignButton, &QPushButton::clicked, this, &PixelArtDialog::savePixelDesign);

    zoomInButton = new QPushButton("Phóng to", this);
    connect(zoomInButton, &QPushButton::clicked, this, &PixelArtDialog::zoomIn);

    zoomOutButton = new QPushButton("Thu nhỏ", this);
    connect(zoomOutButton, &QPushButton::clicked, this, &PixelArtDialog::zoomOut);

    previewButton = new QPushButton("Xem trước", this);
    connect(previewButton, &QPushButton::clicked, this, &PixelArtDialog::previewImage);

    coordinateCheckbox = new QCheckBox("Xem tọa độ", this);
    coordinateCheckbox->setChecked(false);

    coordinatesLabel = new QLabel(this);
    coordinatesLabel->setAlignment(Qt::AlignBottom | Qt::AlignLeft);

    QHBoxLayout* buttonConfigLayout = new QHBoxLayout;
    buttonConfigLayout->addWidget(applySizeButton);
    buttonConfigLayout->addWidget(openImageButton);

    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(colorButton);
    buttonLayout->addWidget(undoButton);
    buttonLayout->addWidget(saveDesignButton);
    buttonLayout->addWidget(zoomInButton);
    buttonLayout->addWidget(zoomOutButton);
    buttonLayout->addWidget(previewButton);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(widthInput);
    layout->addWidget(heightInput);
    layout->addLayout(buttonConfigLayout);
    layout->addWidget(scrollArea);
    layout->addLayout(buttonLayout);
    layout->addWidget(coordinateCheckbox);
    layout->addWidget(coordinatesLabel);
    setLayout(layout);

    view->viewport()->installEventFilter(this);
}

void PixelArtDialog::createPixelGrid() {
    pixelMatrix.clear();
    scene->clear();
    for (int y = 0; y < gridHeight; ++y) {
        QVector<QGraphicsRectItem*> row;
        for (int x = 0; x < gridWidth; ++x) {
            QGraphicsRectItem* rect = new QGraphicsRectItem(x * pixelSize, y * pixelSize, pixelSize, pixelSize);
            rect->setBrush(QBrush(Qt::white));
            rect->setPen(QPen(Qt::black));
            rect->setFlag(QGraphicsItem::ItemIsSelectable);
            scene->addItem(rect);
            row.append(rect);
        }
        pixelMatrix.append(row);
    }
}
void PixelArtDialog::applySize() {
    gridWidth = widthInput->value();
    gridHeight = heightInput->value();
    view->setFixedSize(gridWidth * pixelSize + 2, gridHeight * pixelSize + 2);
    createPixelGrid();
}

void PixelArtDialog::openImage() {
    QString filePath = QFileDialog::getOpenFileName(this, "Chọn hình ảnh", "", "Images (*.png *.xpm *.jpg *.bmp)");
    if (!filePath.isEmpty()) {
        QImage image(filePath);
        if (image.isNull()) {
            QMessageBox::warning(this, "Lỗi", "Không thể mở hình ảnh.");
            return;
        }

        pixelSize = 1;
        gridWidth = image.width() / pixelSize;
        gridHeight = image.height() / pixelSize;
        widthInput->setValue(gridWidth);
        heightInput->setValue(gridHeight);
        applySize();

        for (int y = 0; y < qMin(gridHeight, image.height()); ++y) {
            for (int x = 0; x < qMin(gridWidth, image.width()); ++x) {
                QColor color = image.pixelColor(x * pixelSize, y * pixelSize);
                pixelMatrix[y][x]->setBrush(QBrush(color));
            }
        }
        view->update();
    }
}

void PixelArtDialog::chooseColor() {
    QColor color = QColorDialog::getColor();
    if (color.isValid()) {
        selectedColor = color;
    }
}

bool PixelArtDialog::eventFilter(QObject* obj, QEvent* event) {
    if (obj == view->viewport()) {
        if (event->type() == QEvent::Wheel) {
            QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);
            if (wheelEvent->modifiers() == Qt::ControlModifier) {
                if (wheelEvent->angleDelta().y() > 0) zoomIn();
                else zoomOut();
                return true;
            }
        } else if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            QPointF pos = view->mapToScene(mouseEvent->pos());
            saveStateToHistory();
            isDrawing = true;

            for (int y = 0; y < gridHeight; ++y) {
                for (int x = 0; x < gridWidth; ++x) {
                    if (pixelMatrix[y][x]->contains(pos)) {
                        if (mouseEvent->button() == Qt::LeftButton && !coordinateCheckbox->isChecked()) {
                            pixelMatrix[y][x]->setBrush(QBrush(selectedColor));
                        } else if (mouseEvent->button() == Qt::RightButton) {
                            pixelMatrix[y][x]->setBrush(QBrush(Qt::white));
                        }
                        if (coordinateCheckbox->isChecked()) {
                            coordinatesLabel->setText(QString("Tọa độ: (%1, %2)").arg(x).arg(y));
                        }
                    }
                }
            }
            return true;
        } else if (event->type() == QEvent::MouseMove && isDrawing) {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            QPointF pos = view->mapToScene(mouseEvent->pos());
            for (int y = 0; y < gridHeight; ++y) {
                for (int x = 0; x < gridWidth; ++x) {
                    if (pixelMatrix[y][x]->contains(pos)) {
                        if (mouseEvent->buttons() & Qt::LeftButton && !coordinateCheckbox->isChecked()) {
                            pixelMatrix[y][x]->setBrush(QBrush(selectedColor));
                        } else if (mouseEvent->buttons() & Qt::RightButton) {
                            pixelMatrix[y][x]->setBrush(QBrush(Qt::white));
                        }
                        if (coordinateCheckbox->isChecked()) {
                            coordinatesLabel->setText(QString("Tọa độ: (%1, %2)").arg(x).arg(y));
                        }
                    }
                }
            }
            return true;
        } else if (event->type() == QEvent::MouseButtonRelease) {
            isDrawing = false;
            return true;
        }
    }
    return QDialog::eventFilter(obj, event);
}

void PixelArtDialog::saveStateToHistory() {
    QVector<QColor> state;
    for (int y = 0; y < gridHeight; ++y) {
        for (int x = 0; x < gridWidth; ++x) {
            state.append(pixelMatrix[y][x]->brush().color());
        }
    }
    history.append(state);
}

void PixelArtDialog::undo() {
    if (!history.isEmpty()) {
        QVector<QColor> lastState = history.takeLast();
        int index = 0;
        for (int y = 0; y < gridHeight; ++y) {
            for (int x = 0; x < gridWidth; ++x) {
                pixelMatrix[y][x]->setBrush(QBrush(lastState[index++]));
            }
        }
        view->update();
    }
}

void PixelArtDialog::zoomIn() {
    pixelSize += 1;
    updatePixelSizes();
}

void PixelArtDialog::zoomOut() {
    if (pixelSize > 1) {
        pixelSize -= 1;
        updatePixelSizes();
    }
}

void PixelArtDialog::updatePixelSizes() {
    for (int y = 0; y < gridHeight; ++y) {
        for (int x = 0; x < gridWidth; ++x) {
            QGraphicsRectItem* rect = pixelMatrix[y][x];
            rect->setRect(x * pixelSize, y * pixelSize, pixelSize, pixelSize);
        }
    }
    view->setFixedSize(gridWidth * pixelSize + 1, gridHeight * pixelSize + 1);
}

void PixelArtDialog::savePixelDesign() {
    QFileDialog dialog(this);
    dialog.setWindowTitle("Lưu thiết kế pixel");
    dialog.setNameFilter("PNG Files (*.png);;Hex Files (*.txt)");
    dialog.setDefaultSuffix("png");
    dialog.setAcceptMode(QFileDialog::AcceptSave);

    if (dialog.exec() == QDialog::Accepted) {
        QString savePath = dialog.selectedFiles().first();
        qDebug() << "Save path:" << savePath;
        if (!savePath.isEmpty()) {
            if (savePath.endsWith(".png")) {
                saveAsImage(savePath);
            } else if (savePath.endsWith(".txt")) {
                saveAsHex(savePath);
            }
        } else {
            qDebug() << "No file selected!";
        }
    }
}
void PixelArtDialog::saveAsImage(const QString& path) {
    QImage image(gridWidth, gridHeight, QImage::Format_RGB32);
    QPainter painter(&image);
    for (int y = 0; y < gridHeight; ++y) {
        for (int x = 0; x < gridWidth; ++x) {
            QColor color = pixelMatrix[y][x]->brush().color();
            painter.fillRect(x, y, pixelSize, pixelSize, color);
        }
    }
    painter.end();
    image.save(path);
    QMessageBox::information(this, "Thông báo", QString("Thiết kế đã được lưu thành hình ảnh: %1").arg(path));
}

void PixelArtDialog::saveAsHex(const QString& path) {
    QFile file(path);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        for (int y = 0; y < gridHeight; ++y) {
            QStringList row;
            for (int x = 0; x < gridWidth; ++x) {
                QColor color = pixelMatrix[y][x]->brush().color();
                quint16 rgb565 = ((color.red() & 0xF8) << 8) | ((color.green() & 0xFC) << 3) | (color.blue() >> 3);
                row.append(QString("0x%1").arg(rgb565, 4, 16, QChar('0')));
            }
            out << row.join(", ") << ",\n";
        }
        file.close();
        QMessageBox::information(this, "Thông báo", QString("Thiết kế đã được lưu thành mã hex: %1").arg(path));
    }
}

void PixelArtDialog::previewImage() {
    QImage image(gridWidth, gridHeight, QImage::Format_RGB32);
    QPainter painter(&image);
    for (int y = 0; y < gridHeight; ++y) {
        for (int x = 0; x < gridWidth; ++x) {
            QColor color = pixelMatrix[y][x]->brush().color();
            painter.fillRect(x, y, pixelSize, pixelSize, color);
        }
    }
    painter.end();

    PreviewDialog* previewDialog = new PreviewDialog(image, this);
    previewDialog->exec();
    delete previewDialog;
}
