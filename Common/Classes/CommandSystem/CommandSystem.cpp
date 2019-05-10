#include "CommandSystem.h"

#include <future>
#include <functional>

#include <QDebug>

using namespace Core;

//-----------------------------------------------------------------------------
TCommandSystem::TCommandSystem(QObject* inParent) : QObject(inParent)
{

}
//-----------------------------------------------------------------------------
TCommandSystem::~TCommandSystem()
{

}
//-----------------------------------------------------------------------------
/**
 * @brief TCommandSystem::addCommand - Метод добавит в систему новую команду
 * @param inCommand - Добавляемая команда
 */
void TCommandSystem::addCommand(std::shared_ptr<TAbstractCommand> inCommand)
{
    if (!inCommand)
        return;

    if (inCommand->commandType() != TAbstractCommand::eCommandType::ctUnknown)
    {
        fQueueAccessMutex.lock(); // Захватываем доступ к очереди

        fCommandsQueue.push(inCommand); // Закидываем команду в очередь

        if (!fCommandsThread) // Поток не инициализирован
            startThread(); // Инициализируем и запускаем поток
        else // Поток инициализирован
            if (!fCommandsThread->joinable()) // Поток не идёт
            {
                fCommandsThread->join(); // На всякий случай ожидаем завершения потока
                startThread(); // Инициализируем и запускаем поток
            } // В случаи если поток уже запущен но не завершён просто освобождаем доступ к очереди

        fQueueAccessMutex.unlock(); // Освобождаем доступ к очереди
    }
}
//-----------------------------------------------------------------------------
/**
 * @brief TCommandSystem::startThread - Метод инициализирует и запустит поток, исполняющий команды
 */
void TCommandSystem::startThread()
{
    std::packaged_task<bool()> Task(std::bind(&TCommandSystem::interpretedConammds, this));
    fCommandsThread.reset(new std::thread(std::move(Task)));
}
//-----------------------------------------------------------------------------
/**
 * @brief TCommandSystem::interpretedConammds - Метод интерпритирует команды из очереди (Выполняется в отдельном потоке)
 * @return Вернёт признак успешности операции
 */
bool TCommandSystem::interpretedConammds()
{
    bool Result = false;

    while (!fCommandsQueue.empty()) // Пока в очереди что то есть
    {
        fQueueAccessMutex.lock(); // Захватываем доступ к очереди

        std::shared_ptr<TAbstractCommand> Command = fCommandsQueue.front(); // Получаем из очереди первый элемент
        fCommandsQueue.pop(); // И выкидываем его из очереди

        fQueueAccessMutex.unlock(); // Освобождаем доступ к очереди

        if (!Command) // Если команда не инициализирована
        {
            Result = false; // Ошибка
            qDebug() << tr("Не инициализированная команда проигнорирована");
        }
        else // Команда получена
        {
            Result = Command->interpreted(); // Интерпритируем команду и получаем результат

            if (!Result)
                qDebug() << tr("Ошибка при выполнении команды:") << Command->commandCode() << static_cast<quint32>(Command->commandType());
        }
    }

    return Result; // Возвращаем результат выполнения
}
//-----------------------------------------------------------------------------
