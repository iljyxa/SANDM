
#include "version.hpp"
#include "gui/main_window.hpp"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    app.setWindowIcon(QIcon(":/resources/icons/icon.svg"));
    app.setApplicationName("SANDM");
    app.setApplicationVersion(APP_VERSION_STRING);
    QApplication::setFont(QFont ("Droid Sans Mono"));

    MainWindow main_window;
    main_window.showMaximized();
    main_window.show();

    return app.exec();
}
