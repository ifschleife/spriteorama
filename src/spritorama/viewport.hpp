#pragma once

#include <QImage>
#include <QWidget>


class Viewport : public QWidget
{
public:
    explicit Viewport(QWidget* parent);

    void loadImageFromFile(QString image_path);

private:
    class OpenGLCanvas* m_canvas;
    class QScrollBar* m_horizontal_scrollbar;
    class QScrollBar* m_vertical_scrollbar;
};
