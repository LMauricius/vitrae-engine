#include "SettingsWindow.h"

SettingsWindow::SettingsWindow(AssetCollection &assetCollection)
    : QMainWindow(), ui(), m_assetCollection(assetCollection)
{
    ui.setupUi(this);
}

SettingsWindow::~SettingsWindow() {}