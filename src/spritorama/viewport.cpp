#include "viewport.hpp"

#include <QDebug>
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QPainter>
#include <QPaintEvent>


Viewport::Viewport(QWidget* parent)
    : QOpenGLWidget(parent)
    , QOpenGLFunctions_4_5_Core()
    , m_shader(new QOpenGLShaderProgram(this))
    , m_image("test_image_1920_1080.png")
    , m_vertices{-0.5f, 0.5f, 0.0f, -1.0f, 0.5f, 0.5f}
{
    m_scale = 1.5;
}

void Viewport::initializeGL()
{
    initializeOpenGLFunctions();

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &m_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
    const GLsizei buffer_size = static_cast<GLsizei>(m_vertices.size() * sizeof(float));
    glNamedBufferData(m_vertex_buffer, buffer_size, m_vertices.data(), GL_STATIC_DRAW);
}

void Viewport::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.scale(m_scale, m_scale);
    QRect rect = event->rect();
    if (m_drawing)
        rect = m_draw_bb;

    painter.drawImage(rect, m_image, rect);
}

void Viewport::mousePressEvent(QMouseEvent* event)
{
    m_drawing = true;
    m_mouse_pos = event->pos() / m_scale;
}

void Viewport::mouseMoveEvent(QMouseEvent* event)
{
    if (m_drawing)
    {
        const QPoint current_pos = event->pos() / m_scale;
        const int min_x = qMin(current_pos.x(), m_mouse_pos.x());
        const int min_y = qMin(current_pos.y(), m_mouse_pos.y());
        const int max_x = qMax(current_pos.x(), m_mouse_pos.x());
        const int max_y = qMax(current_pos.y(), m_mouse_pos.y());

        const QRect rect(min_x, min_y, max_x, max_y);

        QPainter painter(&m_image);
        painter.setPen(Qt::red);
        painter.drawLine(m_mouse_pos, current_pos);
        m_mouse_pos = current_pos;

        m_draw_bb = rect;
        update(m_draw_bb);
    }
}

void Viewport::mouseReleaseEvent(QMouseEvent*)
{
    m_drawing = false;
}
