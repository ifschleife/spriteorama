#include "viewport.hpp"

#include <QDebug>
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QPainter>
#include <QWheelEvent>


Viewport::Viewport(QWidget* parent)
    : QOpenGLWidget(parent)
    , QOpenGLFunctions_4_5_Core()
    , m_drawing(false)
    , m_scroll_delta(0.0)
    , m_texture_shader_program(new QOpenGLShaderProgram(this))
    , m_image(QSize(500, 500), QImage::Format_RGB32)
    , m_scale(1.0)
{
    // setUpdateBehavior(QOpenGLWidget::NoPartialUpdate);
    
    m_image.fill(Qt::red);
}

void Viewport::loadImageFromFile(const QString& image_path)
{
    m_image = QImage(image_path).rgbSwapped();
    if (m_image.isNull())
        return;

    glDeleteTextures(1, &m_texture_id);

    createTextureFromImage();

    update();
}

QSize Viewport::sizeHint() const
{
    return m_image.size();
}

void Viewport::initializeGL()
{
    initializeOpenGLFunctions();

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    calculateProjectionMatrix(size());
    const QMatrix4x4 mvp = m_projection_matrix * m_image_matrix;
    glGenBuffers(1, &m_mvp_uni_id);
    glBindBuffer(GL_UNIFORM_BUFFER, m_mvp_uni_id);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(float)*16, mvp.constData(), GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glGenBuffers(1, &m_vertex_buffer);
    createTextureFromImage();

    m_texture_shader_program->addShaderFromSourceFile(QOpenGLShader::Vertex, "src/spritorama/shaders/texture.vert");
    m_texture_shader_program->addShaderFromSourceFile(QOpenGLShader::Fragment, "src/spritorama/shaders/texture.frag");
    m_texture_shader_program->link();
    // pos_attr = static_cast<GLuint>(m_texture_shader_program.attribLocation("position"));
}

void Viewport::paintGL()
{
    glDisable(GL_DEPTH_TEST);

    m_texture_shader_program->bind();
    glBindTextureUnit(0, m_texture_id);

    const QMatrix4x4 mvp = m_projection_matrix * m_image_matrix;
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_mvp_uni_id);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float)*16, mvp.constData());
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(0);
    glBindTextureUnit(0, 0);
}

void Viewport::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);

    calculateProjectionMatrix(QSize(width, height));
}

void Viewport::mousePressEvent(QMouseEvent* event)
{
    m_drawing = true;

    m_draw_pos = mapScreenToImage(event->pos());
    drawPoint(m_draw_pos);
    update();
}

void Viewport::mouseMoveEvent(QMouseEvent* event)
{
    if (m_drawing)
    {
        const QPoint new_draw_pos = mapScreenToImage(event->pos());
        drawLine(m_draw_pos, new_draw_pos);
        m_draw_pos = new_draw_pos;
        update();
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
        if (qAbs(m_scroll_delta) > qreal(150))
        {
            m_scale *= event->delta() > 0 ? 2.0 : 0.5;
            m_projection_matrix.scale(event->delta() > 0 ? 2.0f : 0.5f);

            update();

            m_scroll_delta = qreal(0);
        }
    }
    else
    {
        event->ignore();
    }
}

void Viewport::calculateProjectionMatrix(const QSize& viewport_size)
{
    const float left = -viewport_size.width() / 2.0f;
    const float right = -left;
    const float bottom = -viewport_size.height() / 2.0f;
    const float top = -bottom;

    m_projection_matrix.setToIdentity();
    m_projection_matrix.ortho(left, right, bottom, top, -1.0f, 1.0f);
}

void Viewport::createTextureFromImage()
{
    glCreateTextures(GL_TEXTURE_2D, 1, &m_texture_id);
    glTextureStorage2D(m_texture_id, 1, GL_RGBA8, m_image.width(), m_image.height());
    glTextureSubImage2D(m_texture_id, 0, 0, 0, m_image.width(), m_image.height(),
                        GL_RGBA, GL_UNSIGNED_BYTE, m_image.constBits());
    glTextureParameteri(m_texture_id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(m_texture_id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(m_texture_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(m_texture_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    m_image_matrix.setToIdentity();
    m_image_matrix.translate(-m_image.width() / 2.0f, -m_image.height() / 2.0f, 0.0f);

    constexpr GLsizei vertex_count = 4*2;
    const float image_width = static_cast<float>(m_image.width());
    const float image_height = static_cast<float>(m_image.height());
    const float vertices[vertex_count] = {0.0f,0.0f, image_width,0.0f, image_width,image_height, 0.0f,image_height};
    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
    glNamedBufferData(m_vertex_buffer, vertex_count * sizeof(float), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Viewport::drawLine(const QPoint& start, const QPoint& end)
{
    QPolygon stroke;
    stroke.append(start);
    stroke.append(end);

    QPainter painter(&m_image);
    painter.setPen(Qt::blue);
    painter.drawPolyline(stroke);

    const QRect aabb = stroke.boundingRect();
    glTextureSubImage2D(m_texture_id, 0, aabb.left(), aabb.top(), aabb.width(), aabb.height(),
                        GL_RGBA, GL_UNSIGNED_BYTE, m_image.copy(aabb).constBits());
}

void Viewport::drawPoint(const QPoint& pos)
{
    QPainter painter(&m_image);
    painter.setPen(Qt::blue);
    painter.drawPoint(pos);
    
    const QRect aabb(pos, QSize(1, 1));
    glTextureSubImage2D(m_texture_id, 0, aabb.left(), aabb.top(), aabb.width(), aabb.height(),
                        GL_RGBA, GL_UNSIGNED_BYTE, m_image.copy(aabb).constBits());
}

QPoint Viewport::mapScreenToImage(const QPoint& screen_pos) const
{
    const int offset_x = (width() - static_cast<int>(m_image.width()*m_scale)) / 2;
    const int offset_y = (height() - static_cast<int>(m_image.height()*m_scale)) / 2;
    const QPoint offset = QPoint(offset_x, offset_y);

    return (screen_pos - offset) / m_scale;
}
