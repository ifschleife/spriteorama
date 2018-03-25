#pragma once

#include <QMainWindow>


class MainWindow : public QMainWindow
{
public:
    explicit MainWindow();
    ~MainWindow() override;

    void createEmptyImage();
    void openImage(QString fname);

private slots:
    void onOpenImage();

private:
    QSize currentScreenResolution() const;

private:
    class Viewport* m_viewport;
};
