#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_5_Core>

#include <QImage>

#include <vector>


class Viewport : public QOpenGLWidget, protected QOpenGLFunctions_4_5_Core
{
public:
    explicit Viewport(QWidget* parent);

    void initializeGL() override;
    void paintEvent(class QPaintEvent* event) override;

    void mousePressEvent(class QMouseEvent* event) override;
    void mouseMoveEvent(class QMouseEvent* event) override;
    void mouseReleaseEvent(class QMouseEvent* event) override;

private:
    class QOpenGLShaderProgram* m_shader;
    QImage m_image;
    QPoint m_mouse_pos;
    bool m_drawing = false;
    QRect m_draw_bb;
    qreal m_scale = 1.0;

    GLuint m_vertex_buffer;
    std::vector<float> m_vertices;
};
