#ifndef COMMANDSYSTEM_H
#define COMMANDSYSTEM_H

#include <memory>
#include <thread>
#include <mutex>
#include <queue>

#include <QObject>

#include "Classes/AbstractCommand/AbstractCommand.h"

namespace Core
{
    class TCommandSystem : public QObject
    {
        Q_OBJECT
    public:
        explicit TCommandSystem(QObject* inParent = nullptr);
        ~TCommandSystem();

        void addCommand(std::shared_ptr<TAbstractCommand> inCommand); // Метод добавит в систему новую команду

    protected:
        std::queue<std::shared_ptr<TAbstractCommand>> fCommandsQueue; // Очередь выполнения команд
        std::unique_ptr<std::thread> fCommandsThread = nullptr; // Поток - выполнитель команд
        std::mutex fQueueAccessMutex; // Мъютекс доступа к очереди выполнения команд

        void startThread(); // Метод инициализирует и запустит поток, исполняющий команды
        bool interpretedConammds(); // Метод интерпритирует команды из очереди
    };
}

#endif // COMMANDSYSTEM_H
