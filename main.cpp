
#include "version.hpp"
#include "gui/main_window.hpp"

int main(int argc, char* argv[]) {

    QApplication app(argc, argv);

    app.setWindowIcon(QIcon(":/resources/icons/sandm.svg")); // NOLINT(*-static-accessed-through-instance)
    app.setApplicationName("SANDM"); // NOLINT(*-static-accessed-through-instance)
    app.setApplicationVersion(APP_VERSION_STRING); // NOLINT(*-static-accessed-through-instance)
    app.setFont(QFont ("Droid Sans Mono")); // NOLINT(*-static-accessed-through-instance)

    MainWindow main_window;
    main_window.resize(1024, 768);
    const QScreen *screen = QGuiApplication::primaryScreen();
    const QRect screen_geometry = screen->geometry();
    const int x = (screen_geometry.width() - main_window.width()) / 2;
    const int y = (screen_geometry.height() - main_window.height()) / 2;
    main_window.move(x, y);
    main_window.showMaximized();
    main_window.show();

    return app.exec(); // NOLINT(*-static-accessed-through-instance)
}