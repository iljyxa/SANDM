
#include <QApplication>
#include "gui/include/main_window.hpp"
#include "version.hpp"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    const QIcon icon(":/resources/icons/icon.ico");
    app.setWindowIcon(icon);
    app.setApplicationName("SANDM");
    app.setApplicationVersion(APP_VERSION_STRING);
    app.setOrganizationName("Organization");
    /*
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale: uiLanguages) {
        const QString baseName = "SANDM_IDE_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }*/

    MainWindow main_window;
    main_window.showMaximized();
    main_window.show();

    return app.exec();
}
