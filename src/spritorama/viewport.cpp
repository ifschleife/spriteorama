#include "viewport.hpp"

#include "opengl_canvas.hpp"

#include <QDebug>
#include <QScrollBar>
#include <QVBoxLayout>


Viewport::Viewport(QWidget* parent)
    : QWidget(parent)
    , m_canvas(new OpenGLCanvas(this))
    // , m_horizontal_scrollbar(new QScrollBar(Qt::Horizontal, this))
    // , m_vertical_scrollbar(new QScrollBar(Qt::Vertical, this))
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(m_canvas);
    // layout->addWidget(m_horizontal_scrollbar);
    setLayout(layout);
}

void Viewport::loadImageFromFile(QString image_path)
{
    QImage image(image_path);
    if (image.isNull())
        return;

    m_canvas->setImage(image);
}
