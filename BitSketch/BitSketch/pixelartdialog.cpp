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
    : QMainWindow(parent), pixelSize(10), gridWidth(50), gridHeight(50), isDrawing(false), selectedColor(Qt::red) {
    initUI();
    showMaximized();
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
    pixelImage = QImage(gridWidth, gridHeight, QImage::Format_RGB32);
    pixelImage.fill(Qt::white);
    pixelPixmap = QPixmap::fromImage(pixelImage.scaled(gridWidth * pixelSize, gridHeight * pixelSize));
    if (!pixelLabel) {
        pixelLabel = new QLabel(this);
        pixelLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    }
    pixelLabel->setPixmap(pixelPixmap);
    pixelLabel->move(0, 0);
    pixelLabel->setFixedSize(gridWidth * pixelSize, gridHeight * pixelSize);
    scrollArea->setWidget(pixelLabel);
    scrollArea->setAlignment(Qt::AlignTop | Qt::AlignLeft);
}

void PixelArtDialog::applySize() {
    const int maxSize = 500;
    gridWidth = qMin(widthInput->value(), maxSize);
    gridHeight = qMin(heightInput->value(), maxSize);
    widthInput->setValue(gridWidth);
    heightInput->setValue(gridHeight);
    qDebug() << "Applying grid size:" << gridWidth << "x" << gridHeight;
    createPixelGrid();
    updatePixelSizes();
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

    // Điều chỉnh kích thước lưới theo ảnh tải lên
    gridWidth = loadedImage.width();
    gridHeight = loadedImage.height();
    widthInput->setValue(gridWidth);
    heightInput->setValue(gridHeight);

    // Sao chép dữ liệu ảnh vào pixelImage
    pixelImage = loadedImage.convertToFormat(QImage::Format_RGB32);

    // Cập nhật hiển thị
    pixelPixmap = QPixmap::fromImage(pixelImage.scaled(gridWidth * pixelSize, gridHeight * pixelSize));
    pixelLabel->setPixmap(pixelPixmap);

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

        if (x >= 0 && x < gridWidth && y >= 0 && y < gridHeight) {
            if (event->type() == QEvent::MouseButtonPress) {
                saveStateToHistory(x, y, pixelImage.pixelColor(x, y));
                isDrawing = true;
            }
            if (mouseEvent->buttons() & Qt::LeftButton && !coordinateCheckbox->isChecked()) {
                pixelImage.setPixelColor(x, y, selectedColor);
            } else if (mouseEvent->buttons() & Qt::RightButton) {
                pixelImage.setPixelColor(x, y, Qt::white);
            }
            pixelPixmap = QPixmap::fromImage(pixelImage.scaled(gridWidth * pixelSize, gridHeight * pixelSize));
            pixelLabel->setPixmap(pixelPixmap);
            coordinatesLabel->setText(QString("Coordinates: (%1, %2)").arg(x).arg(y));
        } else {
            qDebug() << "Coordinate out of range:" << x << "," << y;
        }
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

void PixelArtDialog::redo() {
    if (!redoHistory.isEmpty()) {
        PixelChange change = redoHistory.takeLast();
        QColor currentColor = pixelImage.pixelColor(change.x, change.y);
        pixelImage.setPixelColor(change.x, change.y, change.color);  // Áp dụng màu từ Redo
        history.append({change.x, change.y, currentColor});         // Lưu màu hiện tại vào Undo
        pixelPixmap = QPixmap::fromImage(pixelImage.scaled(gridWidth * pixelSize, gridHeight * pixelSize));
        pixelLabel->setPixmap(pixelPixmap);
        qDebug() << "Redo to: (" << change.x << "," << change.y << ") with color" << change.color;
    } else {
        qDebug() << "No redo actions available";
    }
}
void PixelArtDialog::undo() {
    if (!history.isEmpty()) {
        PixelChange change = history.takeLast();
        QColor currentColor = pixelImage.pixelColor(change.x, change.y);
        pixelImage.setPixelColor(change.x, change.y, change.color);  // Khôi phục màu cũ
        redoHistory.append({change.x, change.y, currentColor});     // Lưu màu hiện tại vào Redo
        pixelPixmap = QPixmap::fromImage(pixelImage.scaled(gridWidth * pixelSize, gridHeight * pixelSize));
        pixelLabel->setPixmap(pixelPixmap);
        qDebug() << "Undo to: (" << change.x << "," << change.y << ") with color" << change.color;
    } else {
        qDebug() << "No undo actions available";
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
        // Lưu kích thước và vị trí trước khi zoom
        int oldPixelSize = pixelSize;
        pixelSize -= 2;
        qDebug() << "Zoom out, pixelSize:" << pixelSize;

        // Tính tỷ lệ zoom
        qreal zoomFactor = (qreal)pixelSize / oldPixelSize;

        // Lấy vị trí tâm zoom (chuột hoặc trung tâm nếu không có zoomPoint)
        QPoint center = zoomPoint.isNull() ? scrollArea->viewport()->rect().center() : zoomPoint;

        // Cập nhật kích thước lưới
        updatePixelSizes();

        // Điều chỉnh thanh cuộn để giữ tâm zoom cố định
        int newScrollX = (center.x() * zoomFactor) - (scrollArea->viewport()->width() / 2);
        int newScrollY = (center.y() * zoomFactor) - (scrollArea->viewport()->height() / 2);
        scrollArea->horizontalScrollBar()->setValue(newScrollX);
        scrollArea->verticalScrollBar()->setValue(newScrollY);
    }
}
void PixelArtDialog::updatePixelSizes() {
    qDebug() << "Updating pixel sizes with pixelSize:" << pixelSize;


    pixelPixmap = QPixmap::fromImage(pixelImage.scaled(gridWidth * pixelSize, gridHeight * pixelSize));
    pixelLabel->setPixmap(pixelPixmap);

    int whiteWidth = gridWidth * pixelSize;
    int whiteHeight = gridHeight * pixelSize;

    int viewWidth = scrollArea->viewport()->width();
    int viewHeight = scrollArea->viewport()->height();


    int offsetX = qMax(0, (viewWidth - whiteWidth) / 2);
    int offsetY = qMax(0, (viewHeight - whiteHeight) / 2);


    pixelLabel->setFixedSize(whiteWidth, whiteHeight);
    pixelLabel->move(offsetX, offsetY);

    qDebug() << "White region size:" << whiteWidth << "x" << whiteHeight << "Offset:" << offsetX << "," << offsetY;
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
