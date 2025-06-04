#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <QMainWindow>
#include "code_editor.hpp"
#include "console.hpp"
#include "core/assembler.hpp"
#include "core/common_definitions.hpp"
#include "core/virtual_machine.hpp"
#include "gui/memory_model.hpp"
#include "gui/memory_view.hpp"
#include "gui/register_editor.hpp"
#include "gui/virtual_machine_controller.hpp"

/**
 * @brief Главное окно приложения SANDM (Simple Assembler Non-stack Demo Machine)
 *
 * Класс реализует основное окно приложения, содержащее редактор кода,
 * отображение регистров, памяти и консоль ввода-вывода.
 */
class MainWindow final : public QMainWindow, public ProcessorIo {
    Q_OBJECT

public:
    /**
     * @brief Конструктор главного окна
     * @param parent Родительский виджет (по умолчанию nullptr)
     */
    explicit MainWindow(QWidget* parent = nullptr);

signals:
    /**
     * @brief Сигнал, испускаемый при применении новой темы
     */
    void ThemeApplied();

private slots:
    // === Управление виртуальной машиной ===
    /**
     * @brief Обработчик запуска виртуальной машины. Перед запуском сбрасывает текущее состояние виртуальной машины
     * и выполняет трансляцию исходного кода (при необходимости) и его загрузку.
     */
    void OnRun();
    /**
     * @brief Обработчик выполнения одного шага программы. Если виртуальная машина остановлена, выполняет трансляцию
     * исходного кода (при необходимости) и его загрузку.
     */
    void OnStep();
    /**
     * @brief Обработчик запуска виртуальной машины в режиме отладки. Перед запуском сбрасывает текущее состояние
     * виртуальной машины и выполняет трансляцию исходного кода (при необходимости) и его загрузку.
     * В режиме отладки выполнение приостанавливается на точках останова.
     */
    void OnDebug();
    /**
     * @brief Обработчик, выоплняемый при запросе обновления состояния виртуальной машины.
     */
    void OnUpdateVm() const;
    /**
     * @brief Обработчик, выполняемой при сбросе виртуальной машины
     */
    void OnResetVm();
    /**
     * @brief Обработчик изменения состояния виртуальной машины
     * @param state Новое состояние виртуальной машины
     * @param debugging Флаг режима отладки
     */
    void OnStateVmChanged(VmState state, bool debugging) const;
    /**
     * @brief Обработчик возникновения ошибки. Выполняет вывод ошибки в консоль.
     * @param error Сообщение об ошибке
     */
    void OnErrorOccurred(const QString& error) const;
    /**
     * @brief Обработчик изменения кода в редакторе. Устанавливает признак необходимости обновления байт-кода
     */
    void OnCodeChanged();

    // === Файловые операции ===
    /**
     * @brief Обработчик открытия файла
     */
    void OnOpenFile();
    /**
     * @brief Обработчик сохранения файла
     */
    void OnSaveFile();
    /**
     * @brief Обработчик сохранения файла с выбором имени
     */
    void OnSaveAsFile();

    // === Взаимодействие с пользователем ===
    /**
     * @brief Обновление строки состояния
     * @param address
     * @param value Значение в ячейке памяти
     */
    void UpdateStatusBar(int address, common::Word value) const;

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
    QString current_file_path_; ///< Путь к текущему открытому файлу
    bool is_bytecode_fresh_; ///< Флаг актуальности байт-кода

    // === Вспомогательные методы ===
    void CreateToolBar();
    void SetToolbarActions(VmState state, bool debugging) const;
    void UpdateStatusBar_TableCell(int row, int column, common::Word value) const;
    bool UpdateByteCode();
    void CreateMenus();
    void LoadExamples();
    void LoadExampleFromResource(const QString& resource_path);
    void CreateSettingsMenu();
    void ApplyTheme();

    // === Справка и информация ===
    void ShowHelp();
    static void ShowAbout();

    // === Реализация ProcessorIo ===
    void InputAsync(common::Type type, InputCallback callback) override;
    void Output(common::Bytes& bytes, common::Type& type) override;
};

#endif
