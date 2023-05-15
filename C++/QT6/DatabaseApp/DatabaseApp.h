#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_DatabaseApp.h"

class DatabaseApp : public QMainWindow
{
    Q_OBJECT

public:
    DatabaseApp(QWidget *parent = nullptr);
    ~DatabaseApp();

private:
    Ui::DatabaseAppClass ui;
};
