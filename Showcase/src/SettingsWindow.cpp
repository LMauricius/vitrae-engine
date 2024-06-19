#include "SettingsWindow.h"

SettingsWindow::SettingsWindow(AssetCollection &assetCollection, Status &status)
    : QMainWindow(), ui(), m_assetCollection(assetCollection), m_status(status)
{
    ui.setupUi(this);
}

SettingsWindow::~SettingsWindow() {}

void SettingsWindow::updateValues()
{
    ui.totalAvgDuraion->setText(QString::number(m_status.totalAvgFrameDuration.count() * 1000.0) +
                                "ms");
    ui.totalFPS->setText(QString::number(m_status.totalFPS));
    ui.currentAvgDuration->setText(
        QString::number(m_status.currentAvgFrameDuration.count() * 1000.0) + "ms");
    ui.currentFPS->setText(QString::number(m_status.currentFPS));
}
