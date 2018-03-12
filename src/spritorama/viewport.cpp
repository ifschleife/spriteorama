#include "viewport.hpp"

#include "opengl_canvas.hpp"

#include <QDebug>
#include <QMouseEvent>
#include <QScrollBar>
#include <QVBoxLayout>
#include <QWheelEvent>


Viewport::Viewport(QWidget* parent)
    : QWidget(parent)
    , m_canvas(new OpenGLCanvas(this))
    , m_scroll_delta(0)
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

    m_scale = QTransform::fromScale(1.0, 1.0);
    m_translation = QTransform::fromTranslate(-image.width() / 2.0, -image.height() / 2.0);
    m_canvas->setTransform(m_translation * m_scale);
    m_canvas->update();
}

void Viewport::mousePressEvent(QMouseEvent* event)
{
    m_drawing = true;

    m_draw_pos = mapScreenPosToCanvas(event->pos());
    m_canvas->drawPoint(m_draw_pos);
}

void Viewport::mouseMoveEvent(QMouseEvent* event)
{
    if (m_drawing)
    {
        const QPoint new_draw_pos = mapScreenPosToCanvas(event->pos());
        m_canvas->drawLine(m_draw_pos, new_draw_pos);
        m_draw_pos = new_draw_pos;
    }
}

void Viewport::mouseReleaseEvent(QMouseEvent*)
{
    m_drawing = false;
}

void Viewport::wheelEvent(QWheelEvent* event)
{
    if (event->modifiers() & Qt::ControlModifier)
    {
        m_scroll_delta += event->delta();
        if (qAbs(m_scroll_delta) > 150)
        {
            const qreal scale_factor = event->delta() > 0 ? 2.0 : 0.5;
            m_scale.scale(scale_factor, scale_factor);
            m_canvas->setTransform(m_translation * m_scale);
            m_canvas->update();

            m_scroll_delta = 0;
        }
    }
    else
    {
        event->ignore();
    }
}

QPoint Viewport::mapScreenPosToCanvas(const QPoint& screen_pos) const
{
    const qreal scale = m_scale.m11();
    const QSize image_size = m_canvas->getImageSize();
    const int offset_x = (width() - static_cast<int>(image_size.width()*scale)) / 2;
    const int offset_y = (height() - static_cast<int>(image_size.height()*scale)) / 2;
    const QPoint offset = QPoint(offset_x, offset_y);

    return (screen_pos - offset) / scale;
}
