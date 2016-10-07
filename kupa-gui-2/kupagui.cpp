#include "kupagui.h"
#include "ui_kupagui.h"
#include "tinyxml.h"
#include "parseXml.h"
#include <QFileDialog>
#include <QSaveFile>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <QMessageBox>

#include <QCoreApplication>
#include <QFile>
#include <QStringList>
#include <QTextStream>
#include <QXmlStreamReader>
#include <QDesktopServices>
#include <QDateTime>

QString TypeOfConnection = "";
QString ModeOperation = " --ModeOperation=true";
string theCommand;
int resultNumber;
QString tcp_mem_user ="";
QString tcp_mem_user_wmem ="";
QString tcp_mem_user_rmem ="";
QString tcp_mem_server = "";
QString tcp_mem_server_wmem = "";
QString tcp_mem_server_rmem = "";
QString tcp_cc = "";
QString SimuTime="";
QString udp_bw="";
QString file_size="";
QString error_model="";
QString error_model_2="";
QString user_bw="";
QString server_bw="";
QString error_rate="";
QString error_rate_2="";
//QString chan_jitter="";
string dce_source;

using namespace std;

kupagui::kupagui(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::kupagui)
{

    ui->setupUi(this);
    //loadTextFile();
}

kupagui::~kupagui()
{
    delete ui;
}

double kupagui::findDataHttp(string s){
    int a = s.find('[');
    string c = s.substr(a);
    int d = c.find('/')-1;
    c= c.substr(1,d);
    double data = atof(c.c_str());
    return data;
}
double kupagui::findSpeedHttp(string s){
    string o="";
    int a = s.find('(');
    int b = s.find(')');
    for (int i=a+1; i<b; i++){
       o += s[i];
    }
    double speed = atof(o.c_str());
    return speed;
}

double kupagui::findTime (string s){
    string o="";
    int a = s.find('-');
    int b = s.find("sec");
    for (int i=a+1; i<b; i++){
       o += s[i];
    }
    double time = atof(o.c_str());
    return time;
}
double kupagui::findData (string s){
  string o ="";
      int a = s.find("sec");
      int b = s.find("Bytes");
      for (int i=a+3; i<b; i++){
         o += s[i];
      }
      double k = atof(o.c_str());
      return k;
}
double kupagui::findDataUdp(string s){
     string o ="";
     unsigned a = s.rfind("sec");
     int b = s.find("ms");
     for (int i=a+3; i<b; i++){
        o += s[i];
     }
     double k = atof(o.c_str());
     return k;
 }

void kupagui::printCalcThroughPut(double throughput){
    QString unit;
    if (throughput < 1)
    {
        throughput=throughput*1000; // in Bit/Sec
        QString calTp = QString::number (throughput);
        unit="bps";
    }
    else if (throughput >= 1 && throughput < 100)
    {
        unit="Kbps";
    }

    else if (throughput >= 100 && throughput < 100000)
    {
        throughput=throughput/1000; // in Mbit/Sec
        unit="Mbps";
    }

    else if (throughput >= 100000)
    {
        throughput=throughput/1000000; // in Gbit/Sec
        unit="Gbps";
    }
    QString calTp = QString::number (throughput);
    ui->output_result->append ("\nCalculated throughput is "+ calTp +" "+unit);

}

string kupagui::GetLowerCase(string stringName) {
std::transform(stringName.begin(), stringName.end(), stringName.begin(), ::tolower);
return stringName;

}


void kupagui::on_button_generate_command_clicked()
{
    QString FinalCommand = "";

    QString ModeOperation = " --ModeOperation=true";
    QString userBwUnit;
    if (ui->user_bw_down_unit->currentIndex()==0) {
        userBwUnit="Mbps";
    }
    else if (ui->user_bw_down_unit->currentIndex()==1) {
        userBwUnit="Gbps";
    }
    QString serverBwUnit;
    if (ui->user_bw_up_unit->currentIndex()==0) {
        serverBwUnit="Mbps";
    }
    else if (ui->user_bw_up_unit->currentIndex()==1) {
        serverBwUnit="Gbps";
    }
    user_bw=" --user_bw_down="+ui->user_bw_down->text()+userBwUnit;
    server_bw=" --user_bw_up="+ui->user_bw_up->text()+serverBwUnit;
    error_rate=" --errRate="+ui->error_rate->text();
    error_rate_2=" --errRate2="+ui->error_rate_2->text();

    dce_source = ui->dce_source->text ().toUtf8 ().constData ();


    //chan_jitter=" --chan_jitter=1";

    QString chan_k_dw;
    QString delay_pdv_dw;
    QString avg_delay_dw;

    QString chan_k_up;
    QString delay_pdv_up;
    QString avg_delay_up;

    delay_pdv_dw=" --delay_pdv_dw="+ui->delay_pdv->text ();
    avg_delay_dw=" --avg_delay_dw="+ui->avg_delay->text ();
    chan_k_dw=" --chan_k_dw="+ui->k->text();

    delay_pdv_up=" --delay_pdv_up="+ui->delay_pdv_up->text ();
    avg_delay_up=" --avg_delay_up="+ui->avg_delay_up->text ();
    chan_k_up=" --chan_k_up="+ui->k_up->text();

    if (ui->error_model->currentIndex()==0){
        error_model=" --ErrorModel=1"; //rate error model
    }else{
        error_model=" --ErrorModel=2"; //burst error model
    }

    if (ui->error_model_2->currentIndex()==0){
        error_model_2=" --ErrorModel2=1"; //rate error model
    }else{
        error_model_2=" --ErrorModel2=2"; //burst error model
    }

    int min, def, max;
    std::size_t first, second;
    first=0;second=0;
    string mem_user,wmem_user, rmem_user;
    string mem_server, wmem_server, rmem_server;

/* -----------------------for iperf tcp--------------------------- */
    if (ui->tabWidget->currentIndex()==0){
        TypeOfConnection =" --TypeOfConnection=p";
        if (ui->tcp_upload->isChecked()==true){
            ModeOperation = " --ModeOperation=false";
        }
        if (ui->tcp_mem_user->displayText().isEmpty() == false){
            mem_user = ui->tcp_mem_user->text ().toUtf8 ().constData ();
            mem_user = RemoveComma (mem_user);

            first = mem_user.find(' ');
            if (first!=std::string::npos){
                min=atoi(mem_user.substr (0,first).c_str ());
            }
            second = mem_user.find(' ', first+2);
            if (second!=std::string::npos){
                def=atoi(mem_user.substr (first, second-first).c_str ());
                max= atoi(mem_user.substr (second).c_str ());
            }
            tcp_mem_user=" --tcp_mem_user="+QString::number (min)+","+QString::number (def)+","+QString::number (max);
           }
        if (ui->tcp_mem_user_wmem->displayText().isEmpty() == false){
            wmem_user = ui->tcp_mem_user_wmem->text ().toUtf8 ().constData ();
            wmem_user = RemoveComma (wmem_user);

            first = wmem_user.find(' ');
            if (first!=std::string::npos){
                min=atoi(wmem_user.substr (0,first).c_str ());
            }
            second = wmem_user.find(' ', first+2);
            if (second!=std::string::npos){
                def=atoi(wmem_user.substr (first, second-first).c_str ());
                max= atoi(wmem_user.substr (second).c_str ());
            }
            tcp_mem_user_wmem=" --tcp_mem_user_wmem="+QString::number (min)+","+QString::number (def)+","+QString::number (max);
        }
        if (ui->tcp_mem_user_rmem->displayText().isEmpty() == false){
            rmem_user = ui->tcp_mem_user_rmem->text ().toUtf8 ().constData ();
            rmem_user = RemoveComma (rmem_user);
            first = rmem_user.find(' ');
            if (first!=std::string::npos){
                min=atoi(rmem_user.substr (0,first).c_str ());
            }
            second = rmem_user.find(' ', first+2);
            if (second!=std::string::npos){
                def=atoi(rmem_user.substr (first, second-first).c_str ());
                max= atoi(rmem_user.substr (second).c_str ());
            }
            tcp_mem_user_rmem=" --tcp_mem_user_rmem="+QString::number (min)+","+QString::number (def)+","+QString::number (max);
        }
        if (ui->tcp_mem_server->displayText().isEmpty() == false){
            mem_server = ui->tcp_mem_server->text ().toUtf8 ().constData ();
            mem_server = RemoveComma (mem_server);

            first = mem_server.find(' ');
            if (first!=std::string::npos){
                min=atoi(mem_server.substr (0,first).c_str ());
            }
            second = mem_server.find(' ', first+2);
            if (second!=std::string::npos){
                def=atoi(mem_server.substr (first, second-first).c_str ());
                max= atoi(mem_server.substr (second).c_str ());
            }
            tcp_mem_server=" --tcp_mem_server="+QString::number (min)+","+QString::number (def)+","+QString::number (max);
        }

        if (ui->tcp_mem_server_wmem->displayText().isEmpty() == false){
            wmem_server = ui->tcp_mem_server_wmem->text ().toUtf8 ().constData ();
            wmem_server = RemoveComma (wmem_server);

            first = wmem_server.find(' ');
            if (first!=std::string::npos){
                min=atoi(wmem_server.substr (0,first).c_str ());
            }
            second = wmem_server.find(' ', first+2);
            if (second!=std::string::npos){
                def=atoi(wmem_server.substr (first, second-first).c_str ());
                max= atoi(wmem_server.substr (second).c_str ());
            }
            tcp_mem_server_wmem=" --tcp_mem_server_wmem="+QString::number (min)+","+QString::number (def)+","+QString::number (max);
         }

        if (ui->tcp_mem_server_rmem->displayText().isEmpty() == false){
            rmem_server = ui->tcp_mem_server_rmem->text ().toUtf8 ().constData ();
            rmem_server = RemoveComma (rmem_server);

            first = rmem_server.find(' ');
            if (first!=std::string::npos){
                min=atoi(rmem_server.substr (0,first).c_str ());
            }
            second = rmem_server.find(' ', first+2);
            if (second!=std::string::npos){
                def=atoi(rmem_server.substr (first, second-first).c_str ());
                max= atoi(rmem_server.substr (second).c_str ());
            }
            tcp_mem_server_rmem=" --tcp_mem_server_rmem="+QString::number (min)+","+QString::number (def)+","+QString::number (max);
         }

        tcp_cc = " --tcp_cc="+ui->tcp_cc->currentText().toLower();
        SimuTime =" --SimuTime="+ui->iperf_time->text();
        FinalCommand = TypeOfConnection  + ModeOperation +tcp_mem_user+tcp_mem_user_wmem+ tcp_mem_user_rmem+ tcp_mem_server + tcp_mem_server_wmem+tcp_mem_server_rmem+ tcp_cc + SimuTime;
        resultNumber=1;
    }
/* -----------------------for iperf udp--------------------------- */
    else if (ui->tabWidget->currentIndex()==1){
        TypeOfConnection =" --TypeOfConnection=u";
        if (ui->udp_upload->isChecked()==true){
            ModeOperation = " --ModeOperation=false";
        }

        udp_bw=" --udp_bw="+ui->udp_bw->text();
        SimuTime =" --SimuTime="+ui->iperf_time_udp->text();
        FinalCommand = TypeOfConnection +  ModeOperation + udp_bw + SimuTime;
        resultNumber=2;
    }

/* -----------------------for wget-thttpd--------------------------- */
    else if(ui->tabWidget->currentIndex()==2){
        TypeOfConnection =" --TypeOfConnection=w";

        if (ui->tcp_mem_user_wget->displayText().isEmpty() == false){
            mem_user = ui->tcp_mem_user_wget->text ().toUtf8 ().constData ();
            mem_user = RemoveComma (mem_user);

            first = mem_user.find(' ');
            if (first!=std::string::npos){
                min=atoi(mem_user.substr (0,first).c_str ());
            }
            second = mem_user.find(' ', first+1);
            if (second!=std::string::npos){
                def=atoi(mem_user.substr (first, second-first).c_str ());
                max= atoi(mem_user.substr (second).c_str ());
            }
            tcp_mem_user=" --tcp_mem_user="+QString::number (min)+","+QString::number (def)+","+QString::number (max);
        }

        if (ui->tcp_mem_user_wmem_wget->displayText().isEmpty() == false){
            wmem_user = ui->tcp_mem_user_wmem_wget->text ().toUtf8 ().constData ();
            wmem_user = RemoveComma (wmem_user);

            first = wmem_user.find(' ');
            if (first!=std::string::npos){
                min=atoi(wmem_user.substr (0,first).c_str ());
            }
            second = wmem_user.find(' ', first+1);
            if (second!=std::string::npos){
                def=atoi(wmem_user.substr (first, second-first).c_str ());
                max= atoi(wmem_user.substr (second).c_str ());
            }
            tcp_mem_user_wmem=" --tcp_mem_user_wmem="+QString::number (min)+","+QString::number (def)+","+QString::number (max);
        }

        if (ui->tcp_mem_user_rmem_wget->displayText().isEmpty() == false){
            rmem_user = ui->tcp_mem_user_rmem_wget->text ().toUtf8 ().constData ();
            rmem_user = RemoveComma (rmem_user);

            first = rmem_user.find(' ');
            if (first!=std::string::npos){
                min=atoi(rmem_user.substr (0,first).c_str ());
            }
            second = rmem_user.find(' ', first+1);
            if (second!=std::string::npos){
                def=atoi(rmem_user.substr (first, second-first).c_str ());
                max= atoi(rmem_user.substr (second).c_str ());
            }
            tcp_mem_user_rmem=" --tcp_mem_user_rmem="+QString::number (min)+","+QString::number (def)+","+QString::number (max);
        }

        if (ui->tcp_mem_server_wget->displayText().isEmpty() == false){
            mem_server = ui->tcp_mem_server_wget->text ().toUtf8 ().constData ();
            mem_server = RemoveComma (mem_server);

            first = mem_server.find(' ');
            if (first!=std::string::npos){
                min=atoi(mem_server.substr (0,first).c_str ());
            }
            second = mem_server.find(' ', first+1);
            if (second!=std::string::npos){
                def=atoi(mem_server.substr (first, second-first).c_str ());
                max= atoi(mem_server.substr (second).c_str ());
            }
            tcp_mem_server=" --tcp_mem_server="+QString::number (min)+","+QString::number (def)+","+QString::number (max);
         }

        if (ui->tcp_mem_server_wmem_wget->displayText().isEmpty() == false){
            wmem_server = ui->tcp_mem_server_wmem_wget->text ().toUtf8 ().constData ();
            wmem_server = RemoveComma (wmem_server);

            first = wmem_server.find(' ');
            if (first!=std::string::npos){
                min=atoi(wmem_server.substr (0,first).c_str ());
            }
            second = wmem_server.find(' ', first+1);
            if (second!=std::string::npos){
                def=atoi(wmem_server.substr (first, second-first).c_str ());
                max= atoi(wmem_server.substr (second).c_str ());
            }
            tcp_mem_server_wmem=" --tcp_mem_server_wmem="+QString::number (min)+","+QString::number (def)+","+QString::number (max);
         }

        if (ui->tcp_mem_server_rmem_wget->displayText().isEmpty() == false){
            rmem_server = ui->tcp_mem_server_rmem_wget->text ().toUtf8 ().constData ();
            rmem_server = RemoveComma (rmem_server);

            first = rmem_server.find(' ');
            if (first!=std::string::npos){
                min=atoi(rmem_server.substr (0,first).c_str ());
            }
            second = rmem_server.find(' ', first+1);
            if (second!=std::string::npos){
                def=atoi(rmem_server.substr (first, second-first).c_str ());
                max= atoi(rmem_server.substr (second).c_str ());
            }
            tcp_mem_server_rmem=" --tcp_mem_server_rmem="+QString::number (min)+","+QString::number (def)+","+QString::number (max);
         }

        tcp_cc = " --tcp_cc="+ui->tcp_cc_wget->currentText().toLower();
        file_size = " --htmlSize="+ui->wget_file_size->text();
        FinalCommand = TypeOfConnection  +  ModeOperation +tcp_mem_user+tcp_mem_user_wmem+ tcp_mem_user_rmem+ tcp_mem_server + tcp_mem_server_wmem+tcp_mem_server_rmem+ tcp_cc + file_size;
        resultNumber=3;
    }
//concatenates all commands
    FinalCommand = FinalCommand + user_bw + server_bw + error_model + error_rate + error_model_2 + error_rate_2 + chan_k_dw + avg_delay_dw + delay_pdv_dw + chan_k_up + avg_delay_up + delay_pdv_up ;
    ui->final_command->setText(FinalCommand);
    statusBar()->showMessage(tr("command created"));
    theCommand = FinalCommand.toUtf8 ().constData ();
//create new shell file
    dce_source = ui->dce_source->text ().toUtf8 ().constData ();
    FILE * pFile;
    pFile = fopen ("shell-kupakupa.sh", "w");
       if (pFile != NULL){
            std::fstream myfile;
            myfile.open ("shell-kupakupa.sh");
            myfile << "#!/bin/bash \n";
            myfile <<"#"<<resultNumber<<"\n";
            myfile << "export KUPA_HOME=`pwd` \n";

            myfile << "cd "+dce_source+" \n";

            /*this one will not work and never will. because it create new shell inside a shell.
            the command after waf shell will not be axecuted, it's considered on different shell environment.*/
            //myfile << "./waf shell \n";
            myfile <<dce_source<<"/build/myscripts/kupakupa/bin/kupakupa "+theCommand+"\n";

            //myfile <<"./waf --run \"kupakupa "<< theCommand <<"\"\n";
            //back to current folder
            //myfile << "cd $KUPA_HOME";
            myfile.close();
            fclose (pFile);
            system("chmod +x shell-kupakupa.sh");
            }
       statusBar()->showMessage(tr("shell file created"));
}

string kupagui::GetStdoutFromCommand(string cmd) {

    string data;
    FILE * stream;
    const int max_buffer = 50000;
    char buffer[max_buffer];
    cmd.append(" 2>&1");

    stream = popen(cmd.c_str(), "r");
    if (stream) {
    while (!feof(stream))
    if (fgets(buffer, max_buffer, stream) != NULL) data.append(buffer);
    pclose(stream);
    }
    return data;
}

void kupagui::on_button_run_clicked()
{
    dce_source = ui->dce_source->text ().toUtf8 ().constData ();
    string ls = GetStdoutFromCommand("./shell-kupakupa.sh");
    QString qstr = QString::fromStdString(ls);
    ui->output_result->setText (qstr);
    string n = GetStdoutFromCommand ("head -n 2 shell-kupakupa.sh | tail -n 1");
    qstr = QString::fromStdString (n);
    //remove previous output
    system("rm -f ./stdout-kupa.txt");
    statusBar()->showMessage(tr("simulation is done"));
    if (n[1]=='1'){
        if ( ui->tcp_download->isChecked ()){
            GetStdoutFromCommand ("cat "+dce_source+"/files-0/var/log/*/stdout > ./stdout-kupa.txt");
          }
        else if (ui->tcp_upload->isChecked ()) {
            GetStdoutFromCommand ("cat "+dce_source+"/files-5/var/log/*/stdout > ./stdout-kupa.txt");
          }
        ui->output_result->append(qstr);
        resultNumber=n[1]-48;
      }

     else if (n[1]=='2'){
       if ( ui->udp_download->isChecked ()){
           GetStdoutFromCommand ("cat "+dce_source+"/files-0/var/log/*/stdout > ./stdout-kupa.txt");
         }
       else if (ui->udp_upload->isChecked ()) {
           GetStdoutFromCommand ("cat "+dce_source+"/files-5/var/log/*/stdout > ./stdout-kupa.txt");
         }
       ui->output_result->append(qstr);
       resultNumber=n[1]-48;
      }

    else {
        GetStdoutFromCommand ("cat "+dce_source+"/files-0/var/log/*/stderr > ./stdout-kupa.txt");
        ui->output_result->append(qstr);
        resultNumber=n[1]-48;
      }
}



void kupagui::on_actionLoad_Command_triggered()
{
  dce_source = ui->dce_source->text ().toUtf8 ().constData ();
  const QString source = QString::fromStdString (dce_source);
  QString filename = QFileDialog::getOpenFileName(this,
                                 tr("Open Xml"), source+"/inputDCE.xml",
                                 tr("Xml files (*.xml)"));

  QFile file(filename);
          if (!file.open(QFile::ReadOnly | QFile::Text))
  {
      std::cerr << "Error: Cannot read file " << qPrintable(filename)
                << ": " << qPrintable(file.errorString())
                << std::endl;

  }

  /*QXmlStreamReader Rxml;
  Rxml.setDevice(&file);
  Rxml.readNext();*/

  char TypeOfConnection;
  //string delay;
  string tcp_cc,udp_bw,delay,user_bw_up,user_bw_down;
  string tcp_mem_user, tcp_mem_user_wmem, tcp_mem_user_rmem,tcp_mem_server,tcp_mem_server_wmem,tcp_mem_server_rmem;
  int htmlSize,ErrorModel, ErrorModel2;
  double k_dw,avg_delay_dw,pdv_dw, k_up,avg_delay_up,pdv_up,SimuTime,errRate, errRate2;
  bool downloadMode;

  ParseInput parser;
  parser.parseInputXml(filename.toUtf8 ().constData (),TypeOfConnection,tcp_cc,udp_bw,SimuTime,downloadMode,errRate, errRate2,k_up, pdv_up, avg_delay_up, k_dw, pdv_dw, avg_delay_dw, ErrorModel, ErrorModel2, user_bw_down, user_bw_up, htmlSize,tcp_mem_user, tcp_mem_user_wmem, tcp_mem_user_rmem, tcp_mem_server, tcp_mem_server_wmem, tcp_mem_server_rmem);

  if (TypeOfConnection=='p'){
      ui->tabWidget->setCurrentIndex (0);
      ui->tcp_cc->setCurrentText (QString::fromStdString (tcp_cc));
      if (downloadMode){
          ui->tcp_download->setChecked (true);
        }
      else {
           ui->tcp_upload->setChecked (true);
        }
      ui->tcp_mem_user->setText (QString::fromStdString (tcp_mem_user));
      ui->tcp_mem_user_wmem->setText (QString::fromStdString (tcp_mem_user_wmem));
      ui->tcp_mem_user_rmem->setText (QString::fromStdString (tcp_mem_user_rmem));

      ui->tcp_mem_server->setText (QString::fromStdString (tcp_mem_server));
      ui->tcp_mem_server_wmem->setText (QString::fromStdString (tcp_mem_server_wmem));
      ui->tcp_mem_server_rmem->setText (QString::fromStdString (tcp_mem_server_rmem));
    }
  if (TypeOfConnection=='u'){
      ui->tabWidget->setCurrentIndex (1);
      ui->udp_bw->setValue (atoi(udp_bw.c_str ()));
      if (downloadMode){
          ui->udp_download->setChecked (true);
        }
      else {
           ui->udp_upload->setChecked (true);
        }
    }
  if (TypeOfConnection=='w'){
      ui->tabWidget->setCurrentIndex (2);
      ui->tcp_cc_wget->setCurrentText (QString::fromStdString (tcp_cc));
      ui->tcp_mem_user_wget->setText (QString::fromStdString (tcp_mem_user));
      ui->tcp_mem_user_wmem_wget->setText (QString::fromStdString (tcp_mem_user_wmem));
      ui->tcp_mem_user_rmem_wget->setText (QString::fromStdString (tcp_mem_user_rmem));

      ui->tcp_mem_server_wget->setText (QString::fromStdString (tcp_mem_server));
      ui->tcp_mem_server_wmem_wget->setText (QString::fromStdString (tcp_mem_server_wmem));
      ui->tcp_mem_server_rmem_wget->setText (QString::fromStdString (tcp_mem_server_rmem));
    }

  if (user_bw_down.find ('M')!=std::string::npos){
      std::string v = user_bw_down.erase (user_bw_down.find ('M'),4);
      ui->user_bw_down->setValue (atoi(v.c_str ()));
      ui->user_bw_down_unit->setCurrentIndex (0);
  }
  if (user_bw_down.find ('G')!=std::string::npos){
      std::string v = user_bw_down.erase (user_bw_down.find ('G'),4);
      ui->user_bw_down->setValue (atoi(v.c_str ()));
      ui->user_bw_down_unit->setCurrentIndex (1);
  }

  if (user_bw_up.find ('M')!=std::string::npos){
      std::string v = user_bw_up.erase (user_bw_up.find ('M'),4);
      ui->user_bw_up->setValue (atoi(v.c_str ()));
      ui->user_bw_up_unit->setCurrentIndex (0);
  }
  if (user_bw_up.find ('G')!=std::string::npos){
      std::string v = user_bw_up.erase (user_bw_up.find ('G'),4);
      ui->user_bw_up->setValue (atoi(v.c_str ()));
      ui->user_bw_up_unit->setCurrentIndex (1);
  }


  if (ErrorModel==1){
      ui->error_model->setCurrentIndex (0);
    }
  else {
      ui->error_model->setCurrentIndex (1);
    }

  if (ErrorModel2==1){
      ui->error_model_2->setCurrentIndex (0);
    }
  else {
      ui->error_model_2->setCurrentIndex (1);
    }

  ui->error_rate->setValue(errRate);
  ui->error_rate_2->setValue(errRate2);

  ui->k->setValue (k_dw);
  ui->avg_delay->setValue (avg_delay_dw);
  ui->delay_pdv->setValue (pdv_dw);

  ui->k_up->setValue (k_up);
  ui->avg_delay_up->setValue (avg_delay_up);
  ui->delay_pdv_up->setValue (pdv_up);

  on_button_generate_command_clicked ();
  statusBar()->showMessage(tr("Xml loaded"));
  }



void kupagui::on_button_getResult_clicked()
{

  //n = n[5];
  dce_source = ui->dce_source->text ().toUtf8 ().constData ();
  double lastReceive = atof(GetStdoutFromCommand ("cat "+ dce_source+"/lastReceive.txt").c_str());
  double firstSend = atof(GetStdoutFromCommand ("cat "+ dce_source+"/firstSend.txt").c_str());
  double totalRec = atof(GetStdoutFromCommand ("cat "+ dce_source+"/recTotal.txt").c_str());

  double throughput= ((totalRec*8)/(lastReceive-firstSend))*1000000; // in kByte/sec



  string n = GetStdoutFromCommand ("tail -n 1 ./stdout-kupa.txt");
  QString q  = QString::fromStdString (n);
  QFile file("out.txt");
  file.open(QIODevice::ReadWrite | QIODevice::Append |QIODevice::Text);
  QTextStream out(&file);


  //get time log
  QDateTime now = QDateTime::currentDateTime();

    if (resultNumber==1){
        ui->output_result->toPlainText ();
        //calculate throughput
        double tcp_time = findTime (n);
        double tcp_data = findData (n);
        double tcp_tp = (tcp_data*8)/tcp_time;
        QString unit;

        if (n[n.find ("Bytes")-1] =='K'){
            if (tcp_tp < 0.1) {
                tcp_tp=tcp_tp*1000;
                unit="bps";
            }
            else if (tcp_tp >= 0.1 && tcp_tp < 100){
                tcp_tp=tcp_tp;
                unit="Kbps";
            }
            else if (tcp_tp >= 100) {
                tcp_tp=tcp_tp/1000;
                unit="Mbps";
            }

            //out << "Kbps \t";
          }
        else if (n[n.find ("Bytes")-1] =='M'){
            if (tcp_tp < 0.1) {
                tcp_tp=tcp_tp*1000;
                unit="KBps";
            }
            else if (tcp_tp >= 0.1 && tcp_tp < 100) {
                tcp_tp=tcp_tp;
                unit="Mbps";
            }
            else if (tcp_tp >= 100) {
                tcp_tp=tcp_tp/1000;
                unit="Gbps";
            }
            //ui->output_result->append ("Mbps");
            //out << "Mbps \t";
          }
        else if (n[n.find ("Bytes")-1]=='G'){
            if (tcp_tp < 0.1) {
                tcp_tp=tcp_tp*1000;
                unit="Mbps";
            }
            else {
                tcp_tp=tcp_tp;
                unit="Gbps";
            }
            //ui->output_result->append ("Gbps");
            //out << "Gbps \t";
          }
        else{
            tcp_tp=tcp_tp;
            unit="bps";
          }

         QString tp = QString::number (tcp_tp);
         ui->output_result->setText ("IPERF throughput is " + tp + " " +unit );
         //ui->output_result->append (tp);
         //ui->output_result->append (unit+"\n");
         out << now.toString ()+ "\t"+tp + "\t";
         out << unit << " \t";
         printCalcThroughPut(throughput);
         out << ui->error_rate->text ()+"\n";
      }
    else if (resultNumber==2){
        ui->output_result->toPlainText ();
        int r = n.size ()-2; //last chat should be size -1, but i found it should be -2. interesting..
        if (n[r]=='r'){
            //if last sentence contains "out-of-order", we get the result from a line before that
            n = GetStdoutFromCommand ("tail -n 2 ./stdout-kupa.txt | head -n 1");
          }

        double udp_data = findDataUdp(n);
        QString jitter = QString::number (udp_data);

        ui->output_result->setText ("Measured jitter is " + jitter + " ms");
        //ui->output_result->append ("measured jitter is");
        //ui->output_result->append (jitter);
        out << now.toString () + "\t"+jitter + "\t"+"ms"+"\t"+"\t" +ui->delay_pdv->text ()+"\t"+ui->avg_delay->text ()+"\n";
        //ui->output_result->append ("ms");

         printCalcThroughPut(throughput);

      }
    else if (resultNumber==3){
        ui->output_result->toPlainText ();
        //ui->output_result->setText ("the last command run is http connection and the last line outputfile is "+q);

        n = GetStdoutFromCommand ("tail -n 5 ./stdout-kupa.txt | head -n 1");
        double http_data = findDataHttp (n);
        double http_speed = findSpeedHttp (n);
        double http_time = http_data/http_speed;

        if (n[n.find('s')-3]=='K'){
            http_time = http_time/1000;

          }
        else if (n[n.find('s')-3]=='M'){
            http_time = http_time/1000000;

          }
        else if (n[n.find('s')-3]=='G'){
            http_time = http_time/1000000000;

          }

        QString ht = QString::number (http_time);
        ui->output_result->setText ("Measured download time is " + ht + " s");
        out << now.toString () + "\t"+ht + "\t";
        out << "s \t" + ui->error_rate->text ()+"\n";

         printCalcThroughPut(throughput);
      }
    else {
        ui->output_result->toPlainText ();
        ui->output_result->setText ("This is unknown connection. Please run the generated command first!!");
      }
    file.close();
    statusBar()->showMessage(tr("result loaded"));
}


void kupagui::on_actionRUN_triggered()
{
    on_button_run_clicked ();
}

void kupagui::on_actionSave_Result_triggered()
{
  dce_source = ui->dce_source->text ().toUtf8 ().constData ();
     QString fileName = QFileDialog::getSaveFileName (ui->output_result, tr("Save file"), QString::fromStdString (dce_source)+"/kupakuparesult.txt",
                                                      tr("Text Files (*.txt)"));
     if (fileName != "") {
                  QFile file(fileName);
                  if (!file.open(QIODevice::WriteOnly)) {
                      // error message
                  } else {
                      QTextStream stream(&file);
                      stream << ui->output_result->toPlainText();
                      stream.flush();
                      file.close();
                  }
              }
    statusBar()->showMessage(tr("result saved"));
}

void kupagui::on_actionEdit_XML_triggered()
{
  dce_source = ui->dce_source->text ().toUtf8 ().constData ();
  GetStdoutFromCommand ("gedit "+dce_source+"/inputDCE.xml");
  statusBar()->showMessage(tr("opening external program to edit XML"));
}


void kupagui::on_actionAbout_triggered()
{
  QMessageBox msgBox;
  msgBox.setText("<h2>kupakupa-GUI</h2>");
  msgBox.setInformativeText("<p>Author: Kae Won Choi, Ewaldo Zihan, Aneta"
            "<p>kaewon.choi@gmail.com"
            "<p>zewaldo88@gmail.com"
            "<p>dragon21century@gmail.com"
            "<p>this is application tu run ns-3-dce simulation of iperf and wget with jitter"
            "enjoy :)");
  msgBox.setStandardButtons(QMessageBox::Ok);
  msgBox.exec();
  statusBar()->showMessage(tr("about page is shown"));
}

void kupagui::on_actionSave_Command_triggered()
{
  dce_source = ui->dce_source->text ().toUtf8 ().constData ();
  QString type;
  QString congestion="reno";
  int server_param_min=4096;
  int server_param_def=8192;
  int server_param_max=8388608;
  int user_param_min=4096;
  int user_param_def=8192;
  int user_param_max=8388608;
  string server_param;
  string user_param;
  string user_param_wmem;
  string user_param_rmem;
  string server_param_rmem;
  string server_param_wmem;
  QString mode;
  QString simuTime;

  QString filename = QFileDialog::getSaveFileName(this,
                                         tr("Save Xml"), QString::fromStdString (dce_source)+"/inputDCE.xml",
                                         tr("Xml files (*.xml)"));

  QFile file(filename);
  file.open(QIODevice::WriteOnly);

  QXmlStreamWriter xmlWriter(&file);
  xmlWriter.setAutoFormatting(true);
  xmlWriter.writeStartDocument();

  xmlWriter.writeStartElement("SimulationParameters");

  if (ui->tabWidget->currentIndex ()==0){
      type="iperf-tcp";
      congestion=ui->tcp_cc->currentText ().toLower ();
      server_param = ui->tcp_mem_server->text ().toUtf8 ().constData ();
      user_param = ui->tcp_mem_user->text ().toUtf8 ().constData ();
      server_param_rmem = ui->tcp_mem_server_rmem->text ().toUtf8 ().constData ();
      server_param_wmem=ui->tcp_mem_server_wmem->text ().toUtf8 ().constData ();
      user_param_rmem=ui->tcp_mem_user_rmem->text ().toUtf8 ().constData ();
      user_param_wmem=ui->tcp_mem_user_wmem->text ().toUtf8 ().constData ();

      mode = ui->tcp_download->isChecked ()?"download":"upload";
      simuTime = ui->iperf_time->text ();
    } else if (ui->tabWidget->currentIndex ()==1){
      type="iperf-udp";
      mode = ui->udp_download->isChecked ()? "download":"upload";
      simuTime = ui->iperf_time_udp->text ();
    } else {
      type="http";
      congestion = ui->tcp_cc_wget->currentText ().toLower ();
      server_param = ui->tcp_mem_server_wget->text ().toUtf8 ().constData ();
      user_param = ui->tcp_mem_user_wget->text ().toUtf8 ().constData ();
      server_param_rmem = ui->tcp_mem_server_rmem_wget->text ().toUtf8 ().constData ();
      server_param_wmem=ui->tcp_mem_server_wmem_wget->text ().toUtf8 ().constData ();
      user_param_rmem=ui->tcp_mem_user_rmem_wget->text ().toUtf8 ().constData ();
      user_param_wmem=ui->tcp_mem_user_wmem_wget->text ().toUtf8 ().constData ();
    }



  xmlWriter.writeTextElement("TypeOfConnection", type);
  xmlWriter.writeTextElement("congestionControl", congestion);
  xmlWriter.writeTextElement("UDPBandwidth", ui->udp_bw->text ());
  xmlWriter.writeTextElement("ModeOperation", mode);
  xmlWriter.writeTextElement("Delay", "0ms");
  xmlWriter.writeTextElement("ErrorRate",ui->error_rate->text());
  xmlWriter.writeTextElement("ErrorRateUP",ui->error_rate_2->text());

  xmlWriter.writeStartElement("DelayParamDOWN");

  xmlWriter.writeAttribute("k",ui->k->text ());
  xmlWriter.writeAttribute("avg_delay",ui->avg_delay->text ());
  xmlWriter.writeAttribute("pdv",ui->delay_pdv->text ());
  xmlWriter.writeEndElement();
  xmlWriter.writeStartElement("DelayParamUP");
  xmlWriter.writeAttribute("k",ui->k_up->text ());
  xmlWriter.writeAttribute("avg_delay",ui->avg_delay_up->text ());
  xmlWriter.writeAttribute("pdv",ui->delay_pdv_up->text ());
  xmlWriter.writeEndElement();


  QString userBwUnit;
  if (ui->user_bw_down_unit->currentIndex()==0) {
      userBwUnit="Mbps";
  }
  else if (ui->user_bw_down_unit->currentIndex()==1) {
      userBwUnit="Gbps";
  }
  xmlWriter.writeTextElement("UserBandwidthDown", ui->user_bw_down->text ()+userBwUnit);

  QString serverBwUnit;
  if (ui->user_bw_up_unit->currentIndex()==0) {
      serverBwUnit="Mbps";
  }
  else if (ui->user_bw_up_unit->currentIndex()==1) {
      serverBwUnit="Gbps";
  }
  xmlWriter.writeTextElement("UserBandwidthUp",ui->user_bw_up->text ()+serverBwUnit);;

  xmlWriter.writeTextElement("Errormodel",QString::number(ui->error_model->currentIndex ()+1));
  xmlWriter.writeTextElement("ErrormodelUP",QString::number(ui->error_model_2->currentIndex ()+1));
  xmlWriter.writeTextElement("SizeOfHttpFile", ui->wget_file_size->text());
  xmlWriter.writeTextElement("SimulationTime", simuTime);


  if (ui->tabWidget->currentIndex ()==0 or ui->tabWidget->currentIndex ()==2){
      server_param = RemoveComma(server_param);
      user_param = RemoveComma (user_param);

      std::size_t first, second;
      first=0;second=0;
      first = server_param.find(' ');
        if (first!=std::string::npos){
          server_param_min=atoi(server_param.substr (0,first).c_str ());
          }
      second = server_param.find(' ', first+2);
        if (second!=std::string::npos){
          server_param_def=atoi(server_param.substr (first, second-first).c_str ());
          server_param_max = atoi(server_param.substr (second).c_str ());
          }
      //reset first and second to get min def max value of tcp_mem_user
      first = 0; second = 0;
      first = user_param.find(' ');
          if (first!=std::string::npos){
            user_param_min=atoi(user_param.substr (0,first).c_str ());
            }
      second = user_param.find(' ', first+2);
          if (second!=std::string::npos){
            user_param_def=atoi(user_param.substr (first, second-first).c_str ());
            user_param_max = atoi(user_param.substr (second).c_str ());
            }
    }
    xmlWriter.writeStartElement("UserTCPMem");
    xmlWriter.writeAttribute("min",QString::number (user_param_min));
    xmlWriter.writeAttribute("def",QString::number (user_param_def));
    xmlWriter.writeAttribute("max",QString::number (user_param_max));
    xmlWriter.writeEndElement();

    xmlWriter.writeStartElement("ServerTCPMem");
    xmlWriter.writeAttribute("min",QString::number (server_param_min));
    xmlWriter.writeAttribute("def",QString::number (server_param_def));
    xmlWriter.writeAttribute("max",QString::number (server_param_max));
    xmlWriter.writeEndElement();

    if (ui->tabWidget->currentIndex ()==0 or ui->tabWidget->currentIndex ()==2){
        server_param_rmem = RemoveComma(server_param_rmem);
        user_param_rmem = RemoveComma (user_param_rmem);

        std::size_t first, second;
        first=0;second=0;
        first = server_param.find(' ');
          if (first!=std::string::npos){
            server_param_min=atoi(server_param.substr (0,first).c_str ());
            }
        second = server_param.find(' ', first+2);
          if (second!=std::string::npos){
            server_param_def=atoi(server_param.substr (first, second-first).c_str ());
            server_param_max = atoi(server_param.substr (second).c_str ());
            }
        //reset first and second to get min def max value of tcp_mem_user
        first = 0; second = 0;
        first = user_param.find(' ');
            if (first!=std::string::npos){
              user_param_min=atoi(user_param.substr (0,first).c_str ());
              }
        second = user_param.find(' ', first+2);
            if (second!=std::string::npos){
              user_param_def=atoi(user_param.substr (first, second-first).c_str ());
              user_param_max = atoi(user_param.substr (second).c_str ());
              }
      }

    xmlWriter.writeStartElement("UserRmem");
    xmlWriter.writeAttribute("min",QString::number (user_param_min));
    xmlWriter.writeAttribute("def",QString::number (user_param_def));
    xmlWriter.writeAttribute("max",QString::number (user_param_max));
    xmlWriter.writeEndElement();

    xmlWriter.writeStartElement("ServerRmem");
    xmlWriter.writeAttribute("min",QString::number (server_param_min));
    xmlWriter.writeAttribute("def",QString::number (server_param_def));
    xmlWriter.writeAttribute("max",QString::number (server_param_max));
    xmlWriter.writeEndElement();

    if (ui->tabWidget->currentIndex ()==0 or ui->tabWidget->currentIndex ()==2){
        server_param_wmem = RemoveComma(server_param_wmem);
        user_param_wmem = RemoveComma (user_param_wmem);

        std::size_t first, second;
        first=0;second=0;
        first = server_param.find(' ');
          if (first!=std::string::npos){
            server_param_min=atoi(server_param.substr (0,first).c_str ());
            }
        second = server_param.find(' ', first+2);
          if (second!=std::string::npos){
            server_param_def=atoi(server_param.substr (first, second-first).c_str ());
            server_param_max = atoi(server_param.substr (second).c_str ());
            }
        //reset first and second to get min def max value of tcp_mem_user
        first = 0; second = 0;
        first = user_param.find(' ');
            if (first!=std::string::npos){
              user_param_min=atoi(user_param.substr (0,first).c_str ());
              }
        second = user_param.find(' ', first+2);
            if (second!=std::string::npos){
              user_param_def=atoi(user_param.substr (first, second-first).c_str ());
              user_param_max = atoi(user_param.substr (second).c_str ());
              }
      }

    xmlWriter.writeStartElement("UserWmem");
    xmlWriter.writeAttribute("min",QString::number (user_param_min));
    xmlWriter.writeAttribute("def",QString::number (user_param_def));
    xmlWriter.writeAttribute("max",QString::number (user_param_max));
    xmlWriter.writeEndElement();

    xmlWriter.writeStartElement("ServerWmem");
    xmlWriter.writeAttribute("min",QString::number (server_param_min));
    xmlWriter.writeAttribute("def",QString::number (server_param_def));
    xmlWriter.writeAttribute("max",QString::number (server_param_max));
    xmlWriter.writeEndElement();

  xmlWriter.writeEndElement();
  xmlWriter.writeEndElement();
  xmlWriter.writeEndDocument();

  file.close();
  statusBar()->showMessage(tr("Xml Saved"));
}

string kupagui::RemoveComma (std::string& str)
{
int i = 0;
//std::cout<<"remove comma from "<< str << std::endl;
std::string str2=str;
for (i=0; i<3; i++)
{
	std::size_t found=str.find(',');
	if (found!=std::string::npos)
	{
	//std::cout<<"remove comma from "<< str << std::endl;
	str2 = str.replace(str.find(','),1," ");
	} else {
	//std::cout<<"no comma found..";
	}
}
std::cout<<str2;
return str2;
}

void kupagui::on_button_changefolder_clicked()
{
  QString dir = QFileDialog::getExistingDirectory(this, tr("Select your ns-3-dce folder source"),
                                                  "/home",
                                                  QFileDialog::ShowDirsOnly
                                                  | QFileDialog::DontResolveSymlinks);
  ui->dce_source->setText (dir);
  statusBar()->showMessage(tr("DCE source changed"));
}
