#ifndef FRAMECONTROLLER_H
#define FRAMECONTROLLER_H

#include <memory>

#include <QObject>

#include "Frames/MainFrame.h"

class TFrameController : public QObject
{
    Q_OBJECT
public:
    explicit TFrameController(QObject *inParent = nullptr);
    ~TFrameController();

    std::shared_ptr<TfmeMainFrame> MainFrame();

private:
    std::shared_ptr<TfmeMainFrame> fMainFrame = nullptr;

public slots:
};

#endif // FRAMECONTROLLER_H
