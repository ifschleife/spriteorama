#include "viewport.hpp"

#include "opengl_canvas.hpp"

#include <QDebug>
#include <QGridLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QScrollBar>
#include <QWheelEvent>


Viewport::Viewport(QWidget* parent)
    : QWidget(parent)
    , m_canvas(new OpenGLCanvas(this))
    , m_scroll_delta(0)
    // , m_horizontal_scrollbar(new QScrollBar(Qt::Horizontal, this))
    // , m_vertical_scrollbar(new QScrollBar(Qt::Vertical, this))
{
    QGridLayout* layout = new QGridLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_canvas, 0, 0);
    // layout->addWidget(m_horizontal_scrollbar);
    setLayout(layout);
}

void Viewport::createEmptyCanvas(const QSize& size)
{
    QImage image = QImage(size, QImage::Format_RGB32);
    image.fill(Qt::red);
    QPainter painter(&image);
    painter.fillRect(100, 100, size.width()-200, size.height()-200, Qt::blue);

    m_canvas->setImage(image);
}

void Viewport::createCanvasFromImage(QImage image)
{
    m_canvas->setImage(image);
}

QSize Viewport::sizeHint() const
{
    return m_canvas->size();
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
    const QSize image_size = m_canvas->getImageScreenSize();

    const int canvas_width = width() - std::min(0, width() - image_size.width());
    const int canvas_height = height() + std::max(0, image_size.height() - height());

    const int offset_x = (canvas_width - image_size.width()) / 2;
    const int offset_y = (canvas_height - image_size.height()) / 2;
    const QPoint offset = QPoint(offset_x, offset_y);

    return (screen_pos - offset) / m_scale.m11();
}
