#include <QActionGroup>
#include <QDesktopServices>
#include <QDirIterator>
#include <QFileDialog>
#include <QKeyCombination>
#include <QMenuBar>
#include <QMessageBox>
#include <QProperty>
#include <QPushButton>
#include <QResource>
#include <QSplitter>
#include <QStatusBar>
#include <QTextBrowser>
#include <QToolBar>
#include <QVBoxLayout>

#include "../include/gui/main_window.hpp"
#include "../include/gui/style_colors.hpp"

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    vm_controller_(new VirtualMachineController(this)),
    assembler_(std::make_unique<Assembler>()),
    code_editor_(new CodeEditor(*assembler_, this)),
    register_editor_(new RegisterEditor(this)),
    memory_table_model_(new MemoryModel(*vm_controller_, this)),
    memory_table_view_(new MemoryView(this)),
    console_(new Console(this)),
    tool_bar_(new QToolBar(this)),
    status_bar_(new QStatusBar(this)),
    examples_menu_(new QMenu(this)),
    action_start_(new QAction(this)),
    action_stop_(new QAction(this)),
    action_pause_continue_(new QAction(this)),
    action_debug_(new QAction(this)),
    action_step_(new QAction(this)),
    is_bytecode_fresh_(false) {
    setCentralWidget(code_editor_);

    memory_table_view_->setModel(memory_table_model_);
    for (int column = 0; column < memory_table_model_->columnCount(); column++) {
        memory_table_view_->setColumnWidth(column, 95);
    }

    // ReSharper disable once CppDFAMemoryLeak
    auto* vertical_splitter = new QSplitter(Qt::Vertical, this);
    // ReSharper disable once CppDFAMemoryLeak
    auto* horizontal_splitter = new QSplitter(Qt::Horizontal, this);

    vertical_splitter->addWidget(register_editor_);
    vertical_splitter->addWidget(memory_table_view_);
    vertical_splitter->addWidget(console_);

    horizontal_splitter->addWidget(code_editor_);
    horizontal_splitter->addWidget(vertical_splitter);

    setCentralWidget(horizontal_splitter);

    setStatusBar(status_bar_);

    connect(memory_table_view_, &MemoryView::CellHovered, this,
            [this](const int row, const int column, const std::optional<snm::Word> value) {
                row == -1 ? UpdateStatusBar() : UpdateStatusBar(value.value(), memory_table_model_->Address(row, column));
            });

    connect(vm_controller_, &VirtualMachineController::StateChanged, this, &MainWindow::OnStateVmChanged);
    connect(vm_controller_, &VirtualMachineController::Update, this, &MainWindow::OnUpdateVm);
    connect(vm_controller_, &VirtualMachineController::Reseted, this, &MainWindow::OnResetVm);
    connect(vm_controller_, &VirtualMachineController::ErrorOccurred, this, &MainWindow::OnErrorOccurred);
    connect(vm_controller_, &VirtualMachineController::Reseted, this, &MainWindow::OnCodeChanged);

    connect(register_editor_, &RegisterEditor::AccumulatorEdited, vm_controller_,
            &VirtualMachineController::OnAccumulatorEdited);
    connect(register_editor_, &RegisterEditor::AuxiliaryEdited, vm_controller_,
            &VirtualMachineController::OnAuxiliaryEdited);
    connect(register_editor_, &RegisterEditor::InstructionPointerEdited, vm_controller_,
            &VirtualMachineController::OnInstructionPointerEdited);
    connect(register_editor_, &RegisterEditor::AccumulatorHovered, [this](const bool hovered) {
        hovered ? UpdateStatusBar(register_editor_->accumulator_edit->value()) : UpdateStatusBar();
    });
    connect(register_editor_, &RegisterEditor::AuxiliaryHovered, [this](const bool hovered) {
        hovered ? UpdateStatusBar(register_editor_->auxiliary_edit->value()) : UpdateStatusBar();
    });

    connect(code_editor_, &CodeEditor::BreakpointAdded, vm_controller_, &VirtualMachineController::OnInsertBreakpoint);
    connect(code_editor_, &CodeEditor::BreakpointRemoved, vm_controller_,
            &VirtualMachineController::OnRemoveBreakpoint);
    connect(code_editor_, &CodeEditor::textChanged, this, &MainWindow::OnCodeChanged);

    connect(this, &MainWindow::ThemeApplied, code_editor_, &CodeEditor::OnApplyTheme);

    CreateToolBar();
    CreateMenus();
    ApplyTheme();
}

void MainWindow::CreateMenus() {
    QMenu* file_menu = menuBar()->addMenu("Файл");
    const QAction* open_action = file_menu->addAction("Открыть", QKeySequence(Qt::CTRL | Qt::Key_O));
    const QAction* save_action = file_menu->addAction("Сохранить", QKeySequence(Qt::CTRL | Qt::Key_S));
    const QAction* save_as_action = file_menu->addAction("Сохранить как ...",
                                                         QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_S));
    const QAction* exit_action = file_menu->addAction("Выход", QKeySequence(Qt::CTRL | Qt::Key_Q));

    connect(open_action, &QAction::triggered, this, &MainWindow::OnOpenFile);
    connect(save_action, &QAction::triggered, this, &MainWindow::OnSaveFile);
    connect(save_as_action, &QAction::triggered, this, &MainWindow::OnSaveAsFile);
    connect(exit_action, &QAction::triggered, this, &QMainWindow::close);

    QMenu* emulator_menu = menuBar()->addMenu("Эмулятор");
    emulator_menu->addAction(action_start_);
    emulator_menu->addAction(action_debug_);
    emulator_menu->addAction(action_stop_);

    // Добавляем новую команду "Сбросить"
    // ReSharper disable once CppDFAMemoryLeak
    const auto action_reset = new QAction("Сбросить", this);
    connect(action_reset, &QAction::triggered, vm_controller_, &VirtualMachineController::OnReset);
    emulator_menu->addAction(action_reset);

    // Меню примеров
    examples_menu_ = menuBar()->addMenu("Примеры");
    LoadExamples();

    CreateSettingsMenu();

    // Меню помощи
    QMenu* help_menu = menuBar()->addMenu("Помощь");
    const QAction* help_action = help_menu->addAction("Справка", QKeySequence(Qt::Key_F1));
    const QAction* about_action = help_menu->addAction("О программе");

    connect(help_action, &QAction::triggered, this, &MainWindow::ShowHelp);
    connect(about_action, &QAction::triggered, this, &MainWindow::ShowAbout);
}

void MainWindow::LoadExamples() {
    examples_menu_->clear();

    const QString resource_prefix = ":/docs/examples/";
    //QResource::registerResource(qApp->applicationDirPath() + "/resources.rcc");

    QStringList resource_files;
    QDirIterator it(resource_prefix, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString resource_path = it.next();
        if (resource_path.endsWith(".snm")) {
            resource_files << resource_path;
        }
    }

    resource_files.sort();

    for (const QString& resource_path : resource_files) {
        QString file_name = QFileInfo(resource_path).fileName();
        const QAction* example_action = examples_menu_->addAction(file_name);

        connect(example_action, &QAction::triggered, this, [this, resource_path]() {
            LoadExampleFromResource(resource_path);
        });
    }

    // Если примеров нет, добавляем заглушку
    if (examples_menu_->actions().isEmpty()) {
        QAction* no_examples_action = examples_menu_->addAction("Нет доступных примеров");
        no_examples_action->setEnabled(false);
    }
}

void MainWindow::LoadExampleFromResource(const QString& resource_path) {
    // Проверяем, есть ли несохраненные изменения
    if (!code_editor_->document()->isEmpty()) {
        const QMessageBox::StandardButton reply = QMessageBox::question(
            this,
            "Подтверждение",
            "Текущий код будет заменен. Продолжить?",
            QMessageBox::Yes | QMessageBox::No
            );

        if (reply == QMessageBox::No) {
            return;
        }
    }

    // Загружаем файл из ресурсов
    QFile file(resource_path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось загрузить пример");
        return;
    }

    QTextStream in(&file);
    code_editor_->setPlainText(in.readAll());
    file.close();

    current_file_path_.clear(); // Сбрасываем путь, так как это пример
    is_bytecode_fresh_ = false;
}

void MainWindow::OnOpenFile() {
    const QString file_name = QFileDialog::getOpenFileName(
        this,
        "Открыть файл",
        "",
        "SANDM файлы (*.snm);;Все файлы (*.*)"
        );

    if (file_name.isEmpty()) {
        return;
    }

    if (!code_editor_->document()->isEmpty()) {
        const QMessageBox::StandardButton reply = QMessageBox::question(
            this,
            "Подтверждение",
            "Текущий код будет заменен. Продолжить?",
            QMessageBox::Yes | QMessageBox::No
            );

        if (reply == QMessageBox::No) {
            return;
        }
    }

    QFile file(file_name);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось открыть файл");
        return;
    }

    QTextStream in(&file);
    code_editor_->setPlainText(in.readAll());
    file.close();

    current_file_path_ = file_name;
    is_bytecode_fresh_ = false;
}

void MainWindow::OnSaveFile() {
    if (current_file_path_.isEmpty()) {
        OnSaveAsFile();
        return;
    }

    QFile file(current_file_path_);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось сохранить файл");
        return;
    }

    QTextStream out(&file);
    out << code_editor_->toPlainText();
    file.close();
}

void MainWindow::OnSaveAsFile() {
    QString file_name = QFileDialog::getSaveFileName(
        this,
        "Сохранить файл",
        "",
        "SANDM файлы (*.snm);;Все файлы (*.*)"
        );

    if (file_name.isEmpty()) {
        return;
    }

    // Добавляем расширение, если его нет
    if (!file_name.endsWith(".snm", Qt::CaseInsensitive)) {
        file_name += ".snm";
    }

    current_file_path_ = file_name;
    OnSaveFile();
}

void MainWindow::ShowHelp() {
    // ReSharper disable once CppDFAMemoryLeak
    const auto help_dialog = new QDialog(this);
    help_dialog->setWindowTitle("Справка");
    help_dialog->setMinimumSize(1024, 768);

    // ReSharper disable once CppDFAMemoryLeak
    auto* text_browser = new QTextBrowser(help_dialog);
    text_browser->setOpenExternalLinks(true);
    text_browser->setOpenLinks(false);

    connect(text_browser, &QTextBrowser::anchorClicked, [](const QUrl& link) {
        QDesktopServices::openUrl(link);
    });

    QString content;
    //QFile file(":/docs/README.html");
    QFile file(":/README.md");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        content = file.readAll();
        file.close();
    } else {
        content = "Файл справки не найден";
    }

    //text_browser->setHtml(content);
    text_browser->setMarkdown(content);

    // ReSharper disable once CppDFAMemoryLeak
    const auto close_button = new QPushButton("Закрыть", help_dialog);
    connect(close_button, &QPushButton::clicked, help_dialog, &QDialog::accept);

    // ReSharper disable once CppDFAMemoryLeak
    auto* main_layout = new QVBoxLayout(help_dialog);
    main_layout->addWidget(text_browser);

    // ReSharper disable once CppDFAMemoryLeak
    auto* button_layout = new QHBoxLayout(help_dialog);
    button_layout->addStretch();
    button_layout->addWidget(close_button);
    button_layout->addStretch();

    main_layout->addLayout(button_layout);

    help_dialog->exec();
    help_dialog->deleteLater();
}

void MainWindow::ShowAbout() {
    const QString about_text = QString(
        "<center>"
        "<h1>SANDM</h1>"
        "<p>Simple Assembler Non-stack Demo Machine</p>"
        "<p><b>Версия: %1</b></p>"
        "<p>Сборка от %2 в %3</p>"
        "</center>"
        ).arg(
        QApplication::applicationVersion(),
        QString(__DATE__),
        QString(__TIME__)
        );

    QMessageBox about_box;
    about_box.setWindowTitle("О программе");
    about_box.setText(about_text);
    about_box.setIconPixmap(QPixmap(":/resources/icons/sandm.svg").scaled(64, 64));
    about_box.exec();
}

void MainWindow::CreateToolBar() {
    tool_bar_ = addToolBar("Инструменты");
    tool_bar_->setToolButtonStyle(Qt::ToolButtonIconOnly);

    action_start_ = tool_bar_->addAction("Запуск");
    action_debug_ = tool_bar_->addAction("Отладка");
    action_pause_continue_ = tool_bar_->addAction("Пауза/Продолжить");
    action_stop_ = tool_bar_->addAction("Остановка");
    action_step_ = tool_bar_->addAction("Шаг");

    action_start_->setIcon(QIcon(":/resources/icons/start.png"));
    action_debug_->setIcon(QIcon(":/resources/icons/debug.png"));
    action_pause_continue_->setIcon(QIcon(":/resources/icons/pause.png"));
    action_stop_->setIcon(QIcon(":/resources/icons/stop.png"));
    action_step_->setIcon(QIcon(":/resources/icons/next.png"));

    connect(action_start_, &QAction::triggered, this, &MainWindow::OnRun);
    connect(action_debug_, &QAction::triggered, this, &MainWindow::OnDebug);
    connect(action_pause_continue_, &QAction::triggered, vm_controller_, &VirtualMachineController::OnPauseContinue);
    connect(action_stop_, &QAction::triggered, vm_controller_, &VirtualMachineController::OnStop);
    connect(action_step_, &QAction::triggered, this, &MainWindow::OnStep);

    action_start_->setShortcut(QKeyCombination(Qt::CTRL | Qt::Key_R));
    action_debug_->setShortcut(QKeyCombination(Qt::CTRL | Qt::Key_F5));
    action_pause_continue_->setShortcut(QKeyCombination(Qt::CTRL | Qt::Key_R));
    action_stop_->setShortcut(QKeyCombination(Qt::CTRL | Qt::SHIFT | Qt::Key_F5));
    action_step_->setShortcut(QKeyCombination(Qt::CTRL | Qt::Key_F7));

    emit OnStateVmChanged(STOPPED, false);
}

void MainWindow::SetToolbarActions(const VmState state, bool debugging = false) const {
    action_start_->setVisible(state == STOPPED);
    action_debug_->setVisible(state == STOPPED);
    action_pause_continue_->setVisible(state == PAUSED || state == RUNNING);
    action_pause_continue_->setIcon(state == PAUSED
        ? QIcon(":/resources/icons/continue.png")
        : QIcon(":/resources/icons/pause.png"));
    action_stop_->setVisible(state == RUNNING || state == PAUSED);
    action_step_->setVisible(state != RUNNING);
}

void MainWindow::UpdateStatusBar(const std::optional<snm::Word> value, const int address) const {
    snm::Bytes bytes(value);

    QString status_text;

    if (!value) {
        status_text = "";
    } else if (address < 0) {
        status_text = QString(
                          "Decimal: %2 | Signed decimal: %3 | Float: %4 | Binary: %5")
                      .arg(static_cast<snm::Word>(bytes))
                      .arg(static_cast<snm::SignedWord>(bytes))
                      .arg(static_cast<snm::Real>(bytes))
                      .arg(QString::fromStdString(bytes.ToBinString()));
    } else {
        status_text = QString(
                          "Address: 0x%1 | Decimal: %2 | Signed decimal: %3 | Float: %4 | Binary: %5")
                      .arg(address, 4, 16, QChar('0'))
                      .arg(static_cast<snm::Word>(bytes))
                      .arg(static_cast<snm::SignedWord>(bytes))
                      .arg(static_cast<snm::Real>(bytes))
                      .arg(QString::fromStdString(bytes.ToBinString()));
    }


    statusBar()->showMessage(status_text);
}

void MainWindow::Output(snm::Bytes& bytes, snm::Type& type) {
    const QString output_string(VirtualMachine::BytesToString(bytes, type).data());
    console_->insertPlainText(output_string);
}

void MainWindow::InputAsync(snm::Type type, InputCallback callback) {
    console_->GetInputStringAsync([this, type, callback](const QString& input) {
        snm::Bytes bytes{};
        try {
            bytes = VirtualMachine::BytesFromString(input.toStdString(), type);
        } catch (const std::exception& e) {
            console_->insertPlainText("\nError: " + QString(e.what()));
        }
        callback(bytes);
    });
}

bool MainWindow::UpdateByteCode() {
    if (is_bytecode_fresh_) {
        return true;
    }

    try {
        auto [byte_code, source_to_bytecode_map] = assembler_->CompileWithDebugInfo(
            code_editor_->toPlainText().toStdString());
        vm_controller_->Load(byte_code, source_to_bytecode_map);
        is_bytecode_fresh_ = true;
        return true;
    } catch (const std::exception& e) {
        console_->append(QString(e.what()) + "\n");
    }

    return false;
}

void MainWindow::OnRun() {
    vm_controller_->ResetProcessor();

    if (!UpdateByteCode()) {
        return;
    }

    emit vm_controller_->OnRun();
}

void MainWindow::OnStep() {
    if (vm_controller_->GetState() == STOPPED && !UpdateByteCode()) {
        return;
    }

    emit vm_controller_->OnStep();
}

void MainWindow::OnDebug() {
    vm_controller_->ResetProcessor();

    if (!UpdateByteCode()) {
        return;
    }

    emit vm_controller_->OnDebug();
}

void MainWindow::OnStateVmChanged(const VmState state, const bool debugging) const {
    SetToolbarActions(state, debugging);
    code_editor_->ClearHighlightedLines();
    if (state == PAUSED) {
        const unsigned int current_line = vm_controller_->GetCurrentCodeLine();
        code_editor_->ScrollToLine(current_line);
        code_editor_->HighlightLine(current_line);
    }

    register_editor_->SetReadOnly(state == RUNNING);
    memory_table_view_->SetReadOnly(state == RUNNING);
}

void MainWindow::OnUpdateVm() const {
    emit memory_table_model_->layoutChanged();

    const auto [accumulator, auxiliary, instruction_pointer] = vm_controller_->GetRegisters();

    emit register_editor_->accumulator_edit->setValue(static_cast<int>(accumulator));
    emit register_editor_->auxiliary_edit->setValue(static_cast<int>(auxiliary));
    emit register_editor_->instruction_pointer_edit->setValue(instruction_pointer);
}

void MainWindow::OnResetVm() {
    is_bytecode_fresh_ = false;
}

void MainWindow::OnErrorOccurred(const QString& error) const {
    console_->append(error + "\n");
}

void MainWindow::OnCodeChanged() {
    is_bytecode_fresh_ = false;
}

// Реализация новых методов:
void MainWindow::CreateSettingsMenu() {
    QMenu* settings_menu = menuBar()->addMenu("Настройки");

    QMenu* theme_menu = settings_menu->addMenu("Тема");

    // ReSharper disable once CppDFAMemoryLeak
    auto* theme_group = new QActionGroup(this);
    theme_group->setExclusive(true);

    QAction* auto_theme_action = theme_menu->addAction("Системная");
    auto_theme_action->setCheckable(true);
    auto_theme_action->setChecked(true);
    theme_group->addAction(auto_theme_action);

    QAction* light_theme_action = theme_menu->addAction("Светлая");
    light_theme_action->setCheckable(true);
    theme_group->addAction(light_theme_action);

    QAction* dark_theme_action = theme_menu->addAction("Темная");
    dark_theme_action->setCheckable(true);
    theme_group->addAction(dark_theme_action);

    connect(auto_theme_action, &QAction::triggered, this, [this]() {
        qApp->styleHints()->setColorScheme(Qt::ColorScheme::Unknown);
        ApplyTheme();
    });
    connect(light_theme_action, &QAction::triggered, this, [this]() {
        qApp->styleHints()->setColorScheme(Qt::ColorScheme::Light);
        ApplyTheme();
    });
    connect(dark_theme_action, &QAction::triggered, this, [this]() {
        qApp->styleHints()->setColorScheme(Qt::ColorScheme::Dark);
        ApplyTheme();
    });

    connect(qApp->styleHints(), &QStyleHints::colorSchemeChanged, this, [this] {
        ApplyTheme();
    });
}

void MainWindow::ApplyTheme() {
    QPalette palette = this->palette();
    palette.setColor(QPalette::Window, StyleColors::Main());
    palette.setColor(QPalette::WindowText, StyleColors::Text());
    palette.setColor(QPalette::Base, StyleColors::Background());
    palette.setColor(QPalette::Text, StyleColors::Text());
    palette.setColor(QPalette::Button, StyleColors::Background());
    palette.setColor(QPalette::ButtonText, StyleColors::Text());
    palette.setColor(QPalette::Highlight, StyleColors::TextHighlight());
    this->setPalette(palette);
    this->setAutoFillBackground(true);

    emit ThemeApplied();
}
