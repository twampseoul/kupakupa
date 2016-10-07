//#ifndef KUPAGUI_H
#define KUPAGUI_H

#include <QMainWindow>
#include <string.h>
#include <stdio.h>
#include <QAbstractButton>
#include <QXmlStreamReader>
#include "tinyxml.h"

using namespace std;

namespace Ui {
class kupagui;
class help;
}

class kupagui : public QMainWindow
{
    Q_OBJECT

public:
    explicit kupagui(QWidget *parent = 0);
    ~kupagui();
    string GetStdoutFromCommand(string cmd);
    double findTime(string s);
    double findData (string s);
    double findDataUdp (string s);
    double findDataHttp(string s);
    double findSpeedHttp(string s);
    string RemoveComma (std::string& str);
    void printCalcThroughPut(double throughput);
    string GetLowerCase(string stringName);

private slots:
    void on_button_run_clicked();
    void on_button_generate_command_clicked();
    void on_actionLoad_Command_triggered();
    void on_button_getResult_clicked();
    void on_actionRUN_triggered();

    void on_actionSave_Result_triggered();

    void on_actionEdit_XML_triggered();

    void on_actionAbout_triggered();

    void on_actionSave_Command_triggered();

    void on_button_changefolder_clicked();


private:
    Ui::kupagui *ui;
    //string loadTextFile();
};
