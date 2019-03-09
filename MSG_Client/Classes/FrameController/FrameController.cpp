#include "FrameController.h"

//-----------------------------------------------------------------------------
TFrameController::TFrameController(QObject *inParent) : QObject(inParent)
{

}
//-----------------------------------------------------------------------------
TFrameController::~TFrameController()
{

}
//-----------------------------------------------------------------------------
std::shared_ptr<TfmeMainFrame> TFrameController::MainFrame()
{
    if (!fMainFrame)
        fMainFrame = std::make_shared<TfmeMainFrame>();

    return fMainFrame;
}
//-----------------------------------------------------------------------------
