#pragma once

#include "ui_Settings.h"

#include <QtWidgets/QMainWindow>

#include "assetCollection.hpp"

class SettingsWindow : public QMainWindow
{
  public:
    SettingsWindow(AssetCollection &assetCollection);
    virtual ~SettingsWindow();

  private:
    Ui::MainWindow ui;

    AssetCollection &m_assetCollection;
};