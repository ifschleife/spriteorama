#include "viewport.hpp"

#include <QDebug>
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QPainter>


Viewport::Viewport(QWidget* parent)
    : QOpenGLWidget(parent)
    , QOpenGLFunctions_4_5_Core()
    , m_drawing(false)
    , m_texture_shader_program(new QOpenGLShaderProgram(this))
    , m_image(QSize(500, 500), QImage::Format_RGB32)
    , m_scale(1.0)
{
    setFixedSize(m_image.size() * m_scale);

    // setUpdateBehavior(QOpenGLWidget::NoPartialUpdate);
    m_image = m_image.rgbSwapped();
}

void Viewport::loadImageFromFile(const QString& image_path)
{
    m_image = QImage(image_path).rgbSwapped();
    if (m_image.isNull())
        return;

    glDeleteTextures(1, &m_texture_id);

    createTextureFromImage();

    setFixedSize(m_image.size() * m_scale);
    update();
}

void Viewport::initializeGL()
{
    initializeOpenGLFunctions();

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    constexpr GLsizei vertex_count = 4*2;
    constexpr float vertices[vertex_count] = {-1.0f,-1.0f, 1.0f,-1.0f, 1.0f,1.0f, -1.0,1.0f};

    glGenBuffers(1, &m_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
    glNamedBufferData(m_vertex_buffer, vertex_count * sizeof(float), vertices, GL_STATIC_DRAW);

    m_texture_shader_program->addShaderFromSourceFile(QOpenGLShader::Vertex, "src/spritorama/shaders/texture.vert");
    m_texture_shader_program->addShaderFromSourceFile(QOpenGLShader::Fragment, "src/spritorama/shaders/texture.frag");
    m_texture_shader_program->link();
    // pos_attr = static_cast<GLuint>(m_texture_shader_program.attribLocation("position"));

    createTextureFromImage();
}

void Viewport::paintGL()
{
    glDisable(GL_DEPTH_TEST);

    m_texture_shader_program->bind();
    glBindTextureUnit(0, m_texture_id);

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
}

void Viewport::mousePressEvent(QMouseEvent* event)
{
    m_drawing = true;
    m_draw_pos = event->pos() / m_scale;
    drawPoint(m_draw_pos);
    update();
}

void Viewport::mouseMoveEvent(QMouseEvent* event)
{
    if (m_drawing)
    {
        const QPoint new_draw_pos = event->pos() / m_scale;
        drawLine(m_draw_pos, new_draw_pos);
        m_draw_pos = new_draw_pos;

        update();
    }
}

void Viewport::mouseReleaseEvent(QMouseEvent*)
{
    m_drawing = false;
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

    glTextureSubImage2D(m_texture_id, 0, pos.x(), pos.y(), 1, 1,
                        GL_RGBA, GL_UNSIGNED_BYTE, m_image.copy(QRect(pos, QSize(1, 1))).constBits());
}
