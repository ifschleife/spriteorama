#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_5_Core>

#include <QImage>


class Viewport : public QOpenGLWidget, protected QOpenGLFunctions_4_5_Core
{
public:
    explicit Viewport(QWidget* parent);

    void loadImageFromFile(const QString& image_path);

private:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;

    void mousePressEvent(class QMouseEvent* event) override;
    void mouseMoveEvent(class QMouseEvent* event) override;
    void mouseReleaseEvent(class QMouseEvent* event) override;

private:
    bool m_drawing;
    QPoint m_draw_pos;

    class QOpenGLShaderProgram* m_texture_shader_program;

    QImage m_image;
    qreal m_scale;

    GLuint m_texture_id;
    GLuint m_vertex_buffer;
};
