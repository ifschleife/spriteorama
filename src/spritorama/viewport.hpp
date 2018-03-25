#pragma once

#include <QImage>
#include <QTransform>
#include <QWidget>


class Viewport : public QWidget
{
public:
    explicit Viewport(QWidget* parent);

    void createEmptyCanvas(const QSize& size);
    void createCanvasFromImage(QImage image);

    QSize sizeHint() const override;

private:
    void mousePressEvent(class QMouseEvent* event) override;
    void mouseMoveEvent(class QMouseEvent* event) override;
    void mouseReleaseEvent(class QMouseEvent* event) override;
    void wheelEvent(class QWheelEvent* event) override;

private:
    QPoint mapScreenPosToCanvas(const QPoint& screen_pos) const;

private:
    class OpenGLCanvas* m_canvas;
    class QScrollBar* m_horizontal_scrollbar;
    class QScrollBar* m_vertical_scrollbar;

    QTransform m_scale;
    QTransform m_translation;

    int m_scroll_delta;
    bool m_drawing;
    QPoint m_draw_pos;
};
