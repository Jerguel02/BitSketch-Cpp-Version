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

PixelArtDialog::PixelArtDialog(QWidget* parent) : QMainWindow(parent) {
    pixelSize = 10;
    gridWidth = 100;
    gridHeight = 100;
    isDrawing = false;
    selectedColor = Qt::black;

    initUI();  // Khởi tạo UI trước
    createPixelGrid();

    pixelImage = QImage(gridWidth, gridHeight, QImage::Format_RGB32);
    pixelImage.fill(Qt::white);
    pixelPixmap = QPixmap(gridWidth * pixelSize, gridHeight * pixelSize);
    if (pixelPixmap.isNull()) {
        qDebug() << "Error: Failed to create initial pixelPixmap";
        return;
    }
    pixelPixmap.fill(Qt::white);
    painter = new QPainter(&pixelPixmap);
    painter->drawImage(0, 0, pixelImage.scaled(gridWidth * pixelSize, gridHeight * pixelSize));
    pixelLabel->setPixmap(pixelPixmap);

    updateTimer = new QTimer(this);
    updateTimer->setSingleShot(true);
    connect(updateTimer, &QTimer::timeout, this, [this]() {
        if (painter && pixelLabel) {
            for (const PixelChange& change : pendingUpdates) {
                if (change.x >= 0 && change.x < gridWidth && change.y >= 0 && change.y < gridHeight) {
                    pixelImage.setPixelColor(change.x, change.y, change.color);
                    painter->fillRect(change.x * pixelSize, change.y * pixelSize, pixelSize, pixelSize, change.color);
                }
            }
            pixelLabel->setPixmap(pixelPixmap);
            pendingUpdates.clear();
        } else {
            qDebug() << "Error: painter or pixelLabel is null in updateTimer";
        }
    });
}
PixelArtDialog::~PixelArtDialog() {}

void PixelArtDialog::initUI() {
    setWindowTitle("Pixel Art Editor");

    QShortcut* undoShortcut = new QShortcut(QKeySequence("Ctrl+Z"), this);
    connect(undoShortcut, &QShortcut::activated, this, &PixelArtDialog::undo);

    QShortcut* redoShortcut = new QShortcut(QKeySequence("Ctrl+Y"), this);
    connect(redoShortcut, &QShortcut::activated, this, &PixelArtDialog::redo);

    widthInput = new QSpinBox(this);
    widthInput->setRange(1, 1000);
    widthInput->setValue(gridWidth);
    widthInput->setPrefix("Width: ");

    heightInput = new QSpinBox(this);
    heightInput->setRange(1, 1000);
    heightInput->setValue(gridHeight);
    heightInput->setPrefix("Height: ");

    applySizeButton = new QPushButton("Apply Size", this);
    connect(applySizeButton, &QPushButton::clicked, this, &PixelArtDialog::applySize);

    openImageButton = new QPushButton("Open Image", this);
    connect(openImageButton, &QPushButton::clicked, this, &PixelArtDialog::openImage);

    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);

    createPixelGrid();  // Khởi tạo lưới ngay trong initUI

    undoButton = new QPushButton("Undo", this);
    connect(undoButton, &QPushButton::clicked, this, &PixelArtDialog::undo);

    colorButton = new QPushButton("Choose Color", this);
    connect(colorButton, &QPushButton::clicked, this, &PixelArtDialog::chooseColor);

    saveDesignButton = new QPushButton("Save Design", this);
    connect(saveDesignButton, &QPushButton::clicked, this, &PixelArtDialog::savePixelDesign);

    zoomInButton = new QPushButton("Zoom In", this);
    connect(zoomInButton, &QPushButton::clicked, this, [this](bool) { zoomIn(); });

    zoomOutButton = new QPushButton("Zoom Out", this);
    connect(zoomOutButton, &QPushButton::clicked, this, [this](bool) { zoomOut(); });

    previewButton = new QPushButton("Preview", this);
    connect(previewButton, &QPushButton::clicked, this, &PixelArtDialog::previewImage);

    coordinateCheckbox = new QCheckBox("Show Coordinates Only", this);
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

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(widthInput);
    layout->addWidget(heightInput);
    layout->addLayout(buttonConfigLayout);
    layout->addWidget(scrollArea);
    layout->addLayout(buttonLayout);
    layout->addWidget(coordinateCheckbox);
    layout->addWidget(coordinatesLabel);

    QWidget* centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    pixelLabel->installEventFilter(this);
}

void PixelArtDialog::createPixelGrid() {

    if (!scrollArea) {
        qDebug() << "Error: scrollArea is null in createPixelGrid";
        return;
    }

    if (gridWidth <= 0 || gridHeight <= 0 || pixelSize <= 0) {
        qDebug() << "Error: Invalid grid size - gridWidth:" << gridWidth << ", gridHeight:" << gridHeight << ", pixelSize:" << pixelSize;
        return;
    }

    pixelImage = QImage(gridWidth, gridHeight, QImage::Format_RGB32);
    pixelImage.fill(Qt::white);

    pixelPixmap = QPixmap::fromImage(pixelImage.scaled(gridWidth * pixelSize, gridHeight * pixelSize));
    if (pixelPixmap.isNull()) {
        qDebug() << "Error: Failed to create pixelPixmap with size" << gridWidth * pixelSize << "x" << gridHeight * pixelSize;
        return;
    }

    if (scrollArea->widget()) {
        scrollArea->takeWidget()->deleteLater();
    }

    pixelLabel = new QLabel(this);
    pixelLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    pixelLabel->setPixmap(pixelPixmap);
    pixelLabel->setFixedSize(gridWidth * pixelSize, gridHeight * pixelSize);
    pixelLabel->move(0, 0);

    scrollArea->setWidget(pixelLabel);
    scrollArea->setAlignment(Qt::AlignTop | Qt::AlignLeft);


    pixelLabel->installEventFilter(this);
}




void PixelArtDialog::applySize() {
    int newWidth = widthInput->value();
    int newHeight = heightInput->value();


    if (newWidth <= 0 || newHeight <= 0 || newWidth * pixelSize > 32767 || newHeight * pixelSize > 32767) {
        qDebug() << "Error: Invalid grid size" << newWidth << "x" << newHeight;
        return;
    }


    QImage oldImage = pixelImage;


    gridWidth = newWidth;
    gridHeight = newHeight;
    pixelImage = QImage(gridWidth, gridHeight, QImage::Format_RGB32);
    pixelImage.fill(Qt::white);


    int copyWidth = qMin(oldImage.width(), gridWidth);
    int copyHeight = qMin(oldImage.height(), gridHeight);
    for (int x = 0; x < copyWidth; ++x) {
        for (int y = 0; y < copyHeight; ++y) {
            pixelImage.setPixelColor(x, y, oldImage.pixelColor(x, y));
        }
    }


    if (updateTimer->isActive()) {
        updateTimer->stop();
        pendingUpdates.clear();
    }


    if (painter) {
        delete painter;
        painter = nullptr;
    }
    pixelPixmap = QPixmap(gridWidth * pixelSize, gridHeight * pixelSize);
    if (pixelPixmap.isNull()) {
        qDebug() << "Error: Failed to create pixelPixmap with size" << gridWidth * pixelSize << "x" << gridHeight * pixelSize;
        return;
    }
    pixelPixmap.fill(Qt::white);
    painter = new QPainter(&pixelPixmap);
    painter->drawImage(0, 0, pixelImage.scaled(gridWidth * pixelSize, gridHeight * pixelSize));
    pixelLabel->setPixmap(pixelPixmap);
    pixelLabel->setFixedSize(gridWidth * pixelSize, gridHeight * pixelSize);
    pixelLabel->move(0, 0);
}

void PixelArtDialog::openImage() {
    QString filePath = QFileDialog::getOpenFileName(this, "Open Image", "", "Images (*.png *.jpg *.bmp)");
    if (filePath.isEmpty()) {
        return;
    }

    QImage loadedImage(filePath);
    if (loadedImage.isNull()) {
        QMessageBox::warning(this, "Error", "Failed to load image!");
        return;
    }

    gridWidth = loadedImage.width();
    gridHeight = loadedImage.height();
    widthInput->setValue(gridWidth);
    heightInput->setValue(gridHeight);


    if (gridWidth * pixelSize > 32767 || gridHeight * pixelSize > 32767) {
        QMessageBox::warning(this, "Error", "Image size too large for current pixelSize!");
        return;
    }

    pixelImage = loadedImage.convertToFormat(QImage::Format_RGB32);
    if (pixelImage.isNull()) {
        qDebug() << "Error: pixelImage is invalid after conversion!";
        return;
    }

    if (updateTimer->isActive()) {
        updateTimer->stop();
    }
    pendingUpdates.clear();


    if (painter) {
        delete painter;
        painter = nullptr;
    }
    pixelPixmap = QPixmap::fromImage(pixelImage.scaled(gridWidth * pixelSize, gridHeight * pixelSize));
    if (pixelPixmap.isNull()) {
        qDebug() << "Error: Failed to create pixelPixmap with size" << gridWidth * pixelSize << "x" << gridHeight * pixelSize;
        return;
    }
    painter = new QPainter(&pixelPixmap);

    if (!pixelLabel) {
        pixelLabel = new QLabel(this);
        pixelLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    }
    pixelLabel->setPixmap(pixelPixmap);
    pixelLabel->setFixedSize(gridWidth * pixelSize, gridHeight * pixelSize);
    pixelLabel->move(0, 0);


    if (!scrollArea) {
        qDebug() << "Error: scrollArea is null in openImage";
        return;
    }
    if (scrollArea->widget() != pixelLabel) {
        scrollArea->setWidget(pixelLabel);
    }


    history.clear();
    redoHistory.clear();
    for (int y = 0; y < gridHeight; ++y) {
        for (int x = 0; x < gridWidth; ++x) {
            QColor color = pixelImage.pixelColor(x, y);
            saveStateToHistory(x, y, color);
        }
    }

    qDebug() << "Opened image:" << filePath << "with size:" << gridWidth << "x" << gridHeight;
}


void PixelArtDialog::chooseColor() {
    QColor color = QColorDialog::getColor();
    if (color.isValid()) {
        selectedColor = color;
    }
}

bool PixelArtDialog::eventFilter(QObject* obj, QEvent* event) {
    if (obj != pixelLabel) {
        return QMainWindow::eventFilter(obj, event);
    }

    if (event->type() == QEvent::MouseButtonPress || (event->type() == QEvent::MouseMove && isDrawing)) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        QPoint pos = mouseEvent->pos();
        int x = pos.x() / pixelSize;
        int y = pos.y() / pixelSize;

        if (pixelImage.isNull()) {
            qDebug() << "Error: pixelImage is null!";
            return false;
        }

        if (x < 0 || x >= pixelImage.width() || y < 0 || y >= pixelImage.height()) {
            qDebug() << "Error: Out of bounds access at" << x << "," << y;
            return false;
        }

        if (event->type() == QEvent::MouseButtonPress) {
            saveStateToHistory(x, y, pixelImage.pixelColor(x, y));
            isDrawing = true;
        }

        QColor color = (mouseEvent->buttons() & Qt::LeftButton && !coordinateCheckbox->isChecked())
                       ? selectedColor
                       : (mouseEvent->buttons() & Qt::RightButton ? Qt::white : pixelImage.pixelColor(x, y));

        pendingUpdates.append({x, y, color});

        if (!updateTimer) {
            qDebug() << "Error: updateTimer is null!";
            return false;
        }

        if (!updateTimer->isActive()) {
            updateTimer->start(16);
        }

        coordinatesLabel->setText(QString("(%1, %2)").arg(x).arg(y));
        return true;
    }
    else if (event->type() == QEvent::MouseButtonRelease) {
        isDrawing = false;
        return true;
    }
    else if (event->type() == QEvent::Wheel) {
        QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);
        if (wheelEvent->modifiers() == Qt::ControlModifier) {
            if (wheelEvent->angleDelta().y() > 0) {
                zoomIn();
            } else {
                zoomOut();
            }
            return true;
        }
    }

    return QMainWindow::eventFilter(obj, event);
}


void PixelArtDialog::saveStateToHistory(int x, int y, QColor color) {
    history.append({x, y, color});
}

void PixelArtDialog::undo() {
    if (!history.isEmpty()) {
        PixelChange change = history.takeLast();
        QColor currentColor = pixelImage.pixelColor(change.x, change.y);
        pixelImage.setPixelColor(change.x, change.y, change.color);
        redoHistory.append({change.x, change.y, currentColor});
        painter->fillRect(change.x * pixelSize, change.y * pixelSize, pixelSize, pixelSize, change.color);
        pixelLabel->setPixmap(pixelPixmap);
    }
}

void PixelArtDialog::redo() {
    if (!redoHistory.isEmpty()) {
        PixelChange change = redoHistory.takeLast();
        QColor currentColor = pixelImage.pixelColor(change.x, change.y);
        pixelImage.setPixelColor(change.x, change.y, change.color);
        history.append({change.x, change.y, currentColor});
        painter->fillRect(change.x * pixelSize, change.y * pixelSize, pixelSize, pixelSize, change.color);
        pixelLabel->setPixmap(pixelPixmap);
    }
}

void PixelArtDialog::zoomIn(const QPoint& zoomPoint) {
    if (pixelSize < 50) {
        int oldPixelSize = pixelSize;
        pixelSize += 2;
        qDebug() << "Zoom in, pixelSize:" << pixelSize;

        qreal zoomFactor = (qreal)pixelSize / oldPixelSize;
        QPoint center = zoomPoint.isNull() ? scrollArea->viewport()->rect().center() : zoomPoint;

        updatePixelSizes();

        int newScrollX = (center.x() * zoomFactor) - (scrollArea->viewport()->width() / 2);
        int newScrollY = (center.y() * zoomFactor) - (scrollArea->viewport()->height() / 2);
        scrollArea->horizontalScrollBar()->setValue(newScrollX);
        scrollArea->verticalScrollBar()->setValue(newScrollY);
    }
}

void PixelArtDialog::zoomOut(const QPoint& zoomPoint) {
    if (pixelSize > 2) {
        int oldPixelSize = pixelSize;
        pixelSize -= 2;
        qDebug() << "Zoom out, pixelSize:" << pixelSize;

        qreal zoomFactor = (qreal)pixelSize / oldPixelSize;
        QPoint center = zoomPoint.isNull() ? scrollArea->viewport()->rect().center() : zoomPoint;

        updatePixelSizes();

        int newScrollX = (center.x() * zoomFactor) - (scrollArea->viewport()->width() / 2);
        int newScrollY = (center.y() * zoomFactor) - (scrollArea->viewport()->height() / 2);
        scrollArea->horizontalScrollBar()->setValue(newScrollX);
        scrollArea->verticalScrollBar()->setValue(newScrollY);
    }
}

void PixelArtDialog::updatePixelSizes() {
    delete painter;
    pixelPixmap = QPixmap(gridWidth * pixelSize, gridHeight * pixelSize);
    pixelPixmap.fill(Qt::white);
    painter = new QPainter(&pixelPixmap);
    painter->drawImage(0, 0, pixelImage.scaled(gridWidth * pixelSize, gridHeight * pixelSize));
    pixelLabel->setPixmap(pixelPixmap);
    pixelLabel->setFixedSize(gridWidth * pixelSize, gridHeight * pixelSize);
    pixelLabel->move(0, 0);
}

void PixelArtDialog::savePixelDesign() {
    QFileDialog dialog(this);
    dialog.setWindowTitle("Save pixel design");
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
    qDebug() << "Saving image to:" << path << "with size:" << gridWidth << "x" << gridHeight;
    QImage image(gridWidth, gridHeight, QImage::Format_RGB32);
    QPainter painter(&image);
    for (int y = 0; y < gridHeight; ++y) {
        for (int x = 0; x < gridWidth; ++x) {
            QColor color = pixelImage.pixelColor(x, y);  // Thay pixelMatrix bằng pixelImage
            painter.fillRect(x, y, 1, 1, color);         // Vẽ từng pixel
        }
    }
    painter.end();
    if (!image.save(path)) {
        QMessageBox::warning(this, "Error", "Failed to save image!");
    } else {
        QMessageBox::information(this, "Info", QString("Design saved as image: %1").arg(path));
    }
}

void PixelArtDialog::saveAsHex(const QString& path) {
    qDebug() << "Saving hex to:" << path << "with size:" << gridWidth << "x" << gridHeight;
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", QString("Cannot open file: %1").arg(file.errorString()));
        return;
    }
    QTextStream out(&file);
    out << "const uint16_t epd_bitmap_images [] PROGMEM = {\n";
    for (int y = 0; y < gridHeight; ++y) {
        QStringList row;
        for (int x = 0; x < gridWidth; ++x) {
            QColor color = pixelImage.pixelColor(x, y);  // Thay pixelMatrix bằng pixelImage
            quint16 rgb565 = ((color.red() & 0xF8) << 8) | ((color.green() & 0xFC) << 3) | (color.blue() >> 3);
            row.append(QString("0x%1").arg(rgb565, 4, 16, QChar('0')));
        }
        out << row.join(", ") << ",\n";
    }
    out << "};\n";
    file.close();
    QMessageBox::information(this, "Info", QString("Design saved as hex: %1").arg(path));
}

void PixelArtDialog::previewImage() {
    QDialog* previewDialog = new QDialog(this);
    previewDialog->setWindowTitle("Preview");
    QVBoxLayout* layout = new QVBoxLayout(previewDialog);
    QLabel* previewLabel = new QLabel(previewDialog);
    previewLabel->setPixmap(QPixmap::fromImage(pixelImage));  // Dùng pixelImage trực tiếp
    layout->addWidget(previewLabel);
    previewDialog->setLayout(layout);
    previewDialog->exec();
    qDebug() << "Preview image displayed with size:" << gridWidth << "x" << gridHeight;
}
