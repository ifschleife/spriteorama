#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_5_Core>

#include <QImage>


class Viewport : public QOpenGLWidget, protected QOpenGLFunctions_4_5_Core
{
public:
    explicit Viewport(QWidget* parent);

    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;

    void mousePressEvent(class QMouseEvent* event) override;
    void mouseMoveEvent(class QMouseEvent* event) override;
    void mouseReleaseEvent(class QMouseEvent* event) override;

private:
    class QOpenGLShaderProgram* m_texture_shader_program;

    GLuint m_texture_id;
    QImage m_image;
    
    QPoint m_draw_pos;
    bool m_drawing = false;
    qreal m_scale = 1.0;

    GLuint m_vertex_buffer;
};
