#include "main_window.hpp"

#include <QApplication>
#include <QCommandLineParser>
#include <QDir>
#include <QSettings>
#include <QSurface>


int main(int argc, char** argv)
{
    QSurfaceFormat format;
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setVersion(4, 5);
    format.setSamples(1);
    format.setSwapInterval(0);
    QSurfaceFormat::setDefaultFormat(format);

    QApplication app(argc, argv);
    app.setApplicationName("spritorama");

    QSettings settings;
    settings.setValue("path/root", QDir::current().absolutePath());
    settings.setValue("path/assets", QDir::current().absolutePath() + "/assets");

    QCommandLineParser parser;
    parser.setApplicationDescription("Spritorama Sprite and Diorama Editor");
    parser.addHelpOption();

    const QCommandLineOption open_option(QStringList() << "open" << "o", "Open sprite on start.", "filename");
    parser.addOption(open_option);
    parser.process(app);

    MainWindow main_window;

    const QString open_filename = parser.value("open");
    if (open_filename.isEmpty())
    {
        main_window.createEmptyImage();
    }
    else
    {
        main_window.openImage(open_filename);
    }

    main_window.show();
    return app.exec();
}
