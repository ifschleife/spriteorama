#include "main_window.hpp"

#include <memory>
#include <QApplication>
#include <QCommandLineParser>
#include <QDir>
#include <QSettings>


int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    app.setApplicationName("spritorama");

    QSettings settings;
    settings.setValue("path/root", QDir::current().absolutePath());

    QCommandLineParser parser;
    parser.setApplicationDescription("Spritorama Sprite and Diorama Editor");
    parser.addHelpOption();

    const QCommandLineOption open_option(QStringList() << "open" << "o", "Open sprite on start.", "filename");
    parser.addOption(open_option);

    std::unique_ptr<MainWindow> main_window = std::make_unique<MainWindow>();
    main_window->show();

    parser.process(app);

    const QString open_filename = parser.value("open");
    if (!open_filename.isEmpty())
        main_window->openSprite(open_filename);

    return app.exec();
}
