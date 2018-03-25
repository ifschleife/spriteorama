#include "main_window.hpp"

#include "viewport.hpp"

#include <QAction>
#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <QGuiApplication>
#include <QMessageBox>
#include <QScreen>
#include <QSettings>


MainWindow::MainWindow()
    : QMainWindow{nullptr}
    , m_viewport(new Viewport(this))
{
    const QSize screen_size = currentScreenResolution();
    const int window_x_offset = qMin(200, static_cast<int>(screen_size.width() * 0.1f));
    const int window_y_offset = qMin(200, static_cast<int>(screen_size.height() * 0.1f));
    move(window_x_offset, window_y_offset);

    QAction* open_action = new QAction(this);
    open_action->setShortcut(Qt::CTRL + Qt::Key_O);
    connect(open_action, &QAction::triggered, this, &MainWindow::onOpenImage);
    addAction(open_action);

    setCentralWidget(m_viewport);
}

MainWindow::~MainWindow() = default;


void MainWindow::createEmptyImage()
{
    const QSize canvas_size = QSize(500, 500);
    resize(canvas_size);
    m_viewport->createEmptyCanvas(canvas_size);
}

void MainWindow::openImage(QString fname)
{
    if (!QFileInfo::exists(fname))
    {
        QMessageBox::critical(this, tr("Error opening file"), tr("Could not open %1").arg(fname));
        return;
    }

    QImage image = QImage(fname);
    if (image.isNull())
    {
        QMessageBox::critical(this, tr("Error opening image"), tr("Could not read image from %1").arg(fname));
    }

    const QSize screen_size = currentScreenResolution();
    QSize window_size = image.size();
    if (image.width() >= screen_size.width() || image.height() >= screen_size.height())
    {
        window_size = screen_size * 0.8;
    }

    m_viewport->createCanvasFromImage(image);
    resize(window_size);
}

void MainWindow::onOpenImage()
{
    QSettings settings;
    const QString default_path = settings.value("path/assets").toString();

    const QString fname = QFileDialog::getOpenFileName(this, tr("Select Image"), default_path, "Images (*.png);;");
    if (!fname.isEmpty())
    {
        openImage(fname);
    }
}

QSize MainWindow::currentScreenResolution() const
{
    return QGuiApplication::screenAt(geometry().topLeft())->availableGeometry().size();
}
