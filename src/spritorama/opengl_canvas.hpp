#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_5_Core>

#include <QImage>
#include <QMatrix4x4>
#include <QTransform>


class OpenGLCanvas : public QOpenGLWidget, protected QOpenGLFunctions_4_5_Core
{
public:
    explicit OpenGLCanvas(QWidget* parent);

    QSize sizeHint() const override;

    void setImage(QImage image);
    QSize getImageSize() const;

    void drawLine(const QPoint& start, const QPoint& end);
    void drawPoint(const QPoint& pos);

    void setTransform(const QTransform& transform);

private:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;

private:
    void calculateProjectionMatrix(const QSize& viewport_size);
    void createTextureFromImage();

private:
    class QOpenGLShaderProgram* m_texture_shader_program;

    QImage m_image;
    QMatrix4x4 m_projection_matrix;
    QTransform m_image_transform;

    GLuint m_texture_id;
    GLuint m_vertex_buffer;
    GLuint m_mvp_uni_id;
};
