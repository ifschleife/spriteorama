#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_5_Core>

#include <QImage>
#include <QMatrix4x4>


class Viewport : public QOpenGLWidget, protected QOpenGLFunctions_4_5_Core
{
public:
    explicit Viewport(QWidget* parent);

    void loadImageFromFile(const QString& image_path);
    QSize sizeHint() const override;

private:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;

    void mousePressEvent(class QMouseEvent* event) override;
    void mouseMoveEvent(class QMouseEvent* event) override;
    void mouseReleaseEvent(class QMouseEvent* event) override;
    void wheelEvent(class QWheelEvent* event) override;

private:
    void calculateProjectionMatrix(const QSize& viewport_size);
    void createTextureFromImage();
    void drawLine(const QPoint& start, const QPoint& end);
    void drawPoint(const QPoint& pos);
    QPoint mapScreenToImage(const QPoint& screen_pos) const;

private:
    bool m_drawing;
    QPoint m_draw_pos;
    qreal m_scroll_delta;

    class QOpenGLShaderProgram* m_texture_shader_program;

    QImage m_image;
    QMatrix4x4 m_image_matrix;
    QMatrix4x4 m_projection_matrix;
    qreal m_scale;

    GLuint m_texture_id;
    GLuint m_vertex_buffer;
    GLuint m_mvp_uni_id;
};
