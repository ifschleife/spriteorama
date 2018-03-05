#pragma once

#include <QMainWindow>


class MainWindow : public QMainWindow
{
public:
    explicit MainWindow();
    ~MainWindow() override;

    void openSprite(QString fname);

private slots:
    void onOpenSprite();

private:
    class Viewport* m_viewport;
};
