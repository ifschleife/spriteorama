#include "main_window.hpp"

#include "viewport.hpp"

#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QSettings>


MainWindow::MainWindow()
    : QMainWindow{nullptr}
    , m_viewport(new Viewport(this))
{
    setCentralWidget(m_viewport);
}

MainWindow::~MainWindow() = default;


void MainWindow::createEmptyImage()
{
    m_viewport->createEmptyCanvas();
}

void MainWindow::openImage(QString fname)
{
    if (!QFileInfo::exists(fname))
    {
        QMessageBox::warning(this, tr("Error opening file"), tr("Could not open %1").arg(fname));
        return;
    }

    m_viewport->createCanvasFromImage(fname);
}

void MainWindow::onOpenImage()
{
    QSettings settings;
    const QString default_path = settings.value("path/root").toString();

    const QString fname = QFileDialog::getOpenFileName(this, tr("Select Image"), default_path, "Images (*.png);;");
    if (!fname.isEmpty())
    {
        openImage(fname);
    }
}
