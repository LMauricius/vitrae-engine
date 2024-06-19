#pragma once

#include "ui_Settings.h"

#include <QtWidgets/QMainWindow>

#include "Status.hpp"
#include "assetCollection.hpp"

class SettingsWindow : public QMainWindow
{
  public:
    SettingsWindow(AssetCollection &assetCollection, Status &status);
    virtual ~SettingsWindow();

    void updateValues();

  private:
    Ui::MainWindow ui;

    AssetCollection &m_assetCollection;
    Status &m_status;
};