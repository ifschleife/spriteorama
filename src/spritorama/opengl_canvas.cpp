#include "opengl_canvas.hpp"

#include <QDebug>
#include <QOpenGLShaderProgram>
#include <QPainter>


OpenGLCanvas::OpenGLCanvas(QWidget* parent)
    : QOpenGLWidget(parent)
    , QOpenGLFunctions_4_5_Core()
    , m_texture_shader_program(new QOpenGLShaderProgram(this))
{
    // setUpdateBehavior(QOpenGLWidget::NoPartialUpdate);
}

QSize OpenGLCanvas::sizeHint() const
{
    return m_image.size();
}

void OpenGLCanvas::setImage(QImage image)
{
    m_image = image.rgbSwapped();
    if (m_image.isNull())
        return;

    glDeleteTextures(1, &m_texture_id);
    createTextureFromImage();

    adjustSize();
    update();
}

QSize OpenGLCanvas::getImageSize() const
{
    return m_image.size();
}

void OpenGLCanvas::drawLine(const QPoint& start, const QPoint& end)
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
    update();
}

void OpenGLCanvas::drawPoint(const QPoint& pos)
{
    QPainter painter(&m_image);
    painter.setPen(Qt::blue);
    painter.drawPoint(pos);
    
    const QRect aabb(pos, QSize(1, 1));
    glTextureSubImage2D(m_texture_id, 0, aabb.left(), aabb.top(), aabb.width(), aabb.height(),
                        GL_RGBA, GL_UNSIGNED_BYTE, m_image.copy(aabb).constBits());
    update();
}

void OpenGLCanvas::setTransform(const QTransform& transform)
{
    m_image_transform = transform;
}

void OpenGLCanvas::initializeGL()
{
    initializeOpenGLFunctions();

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    calculateProjectionMatrix(size());
    const QMatrix4x4 mvp = m_projection_matrix * m_image_transform;
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

void OpenGLCanvas::paintGL()
{
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    m_texture_shader_program->bind();
    glBindTextureUnit(0, m_texture_id);

    const QMatrix4x4 mvp = m_projection_matrix * m_image_transform;
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

void OpenGLCanvas::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);

    calculateProjectionMatrix(QSize(width, height));
}

void OpenGLCanvas::calculateProjectionMatrix(const QSize& viewport_size)
{
    const float left = -viewport_size.width() / 2.0f;
    const float right = -left;
    const float bottom = -viewport_size.height() / 2.0f;
    const float top = -bottom;

    m_projection_matrix.setToIdentity();
    m_projection_matrix.ortho(left, right, bottom, top, -1.0f, 1.0f);
}

void OpenGLCanvas::createTextureFromImage()
{
    glCreateTextures(GL_TEXTURE_2D, 1, &m_texture_id);
    glTextureStorage2D(m_texture_id, 1, GL_RGBA8, m_image.width(), m_image.height());
    glTextureSubImage2D(m_texture_id, 0, 0, 0, m_image.width(), m_image.height(),
                        GL_RGBA, GL_UNSIGNED_BYTE, m_image.constBits());
    glTextureParameteri(m_texture_id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(m_texture_id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(m_texture_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(m_texture_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    constexpr GLsizei vertex_count = 4*2;
    const float image_width = static_cast<float>(m_image.width());
    const float image_height = static_cast<float>(m_image.height());
    const float vertices[vertex_count] = {0.0f,0.0f, image_width,0.0f, image_width,image_height, 0.0f,image_height};
    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
    glNamedBufferData(m_vertex_buffer, vertex_count * sizeof(float), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
