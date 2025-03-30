#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <QMainWindow>
#include "code_editor.hpp"
#include "console.hpp"
#include "memory_model.hpp"
#include "memory_view.hpp"
#include "register_editor.hpp"
#include "virtual_machine_controller.hpp"
#include "../../core/include/assembler.hpp"
#include "../../core/include/common_definitions.hpp"
#include "../../core/include/virtual_machine.hpp"

/**
 * @brief Главное окно приложения SANDM (Simple Assembler Non-stack Demo Machine)
 *
 * Класс реализует основное окно приложения, содержащее редактор кода,
 * отображение регистров, памяти и консоль ввода-вывода.
 */
class MainWindow final : public QMainWindow, public ProcessorIo {
    Q_OBJECT

public:
    // === Конструктор ===
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    // === Управление виртуальной машиной ===
    void OnRun();
    void OnStep();
    void OnDebug();
    void OnUpdateVm() const;
    void OnStateVmChanged(VmState state, bool debugging) const;
    void OnErrorOccurred(const QString& error) const;
    void OnCodeChanged();

    // === Файловые операции ===
    void OnOpenFile();
    void OnSaveFile();
    void OnSaveAsFile();

    // === Взаимодействие с пользователем ===
    void UpdateStatusBar(int row, int column, common::Word value) const;

private:
    // === Компоненты интерфейса ===
    VirtualMachineController* vm_controller_;
    std::unique_ptr<Assembler> assembler_;
    CodeEditor* code_editor_;
    RegisterEditor* register_editor_;
    MemoryModel* memory_table_model_;
    MemoryView* memory_table_view_;
    Console* console_;
    QToolBar* tool_bar_;
    QStatusBar* status_bar_;
    QMenu* examples_menu_;

    // === Действия панели инструментов ===
    QAction* action_start_;
    QAction* action_stop_;
    QAction* action_pause_continue_;
    QAction* action_debug_;
    QAction* action_step_;

    // === Состояние приложения ===
    QString current_file_path_;  ///< Путь к текущему открытому файлу
    bool is_bytecode_fresh_;     ///< Флаг актуальности байт-кода

    // === Вспомогательные методы ===
    void CreateToolBar();
    void SetToolbarActions(VmState state, bool debugging) const;
    void UpdateByteCode();
    void CreateMenus();
    void LoadExamples();
    void LoadExampleFile(const QString& file_path);

    // === Справка и информация ===
    void ShowHelp();
    static void ShowAbout();

    // === Реализация ProcessorIo ===
    void Input(common::Bytes& bytes, common::Type& type) override;
    void Output(common::Bytes& bytes, common::Type& type) override;
};

#endif