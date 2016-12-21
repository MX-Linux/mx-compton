/**********************************************************************
 *  mxcompton.cpp
 **********************************************************************
 * Copyright (C) 2015 MX Authors
 *
 * Authors: Dolphin Oracle
 *          MX Linux <http://mxlinux.org>
 *
 * This file is part of mx-compton.
 *
 * mx-compton is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * mx-welcome is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with mx-compton.  If not, see <http://www.gnu.org/licenses/>.
 **********************************************************************/

#include "mxcompton.h"
#include "ui_mxcompton.h"
#include "QDebug"
#include "QDir"
#include <QFileDialog>

mxcompton::mxcompton(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::mxcompton)
{
    ui->setupUi(this);
    setup();
}


// Util function for getting bash command output and error code
Result mxcompton::runCmd(QString cmd)
{
    QEventLoop loop;
    proc = new QProcess(this);
    proc->setReadChannelMode(QProcess::MergedChannels);
    connect(proc, SIGNAL(finished(int)), &loop, SLOT(quit()));
    proc->start("/bin/bash", QStringList() << "-c" << cmd);
    loop.exec();
    disconnect(proc, 0, 0, 0);
    Result result = {proc->exitCode(), proc->readAll().trimmed()};
    delete proc;
    return result;
}


void mxcompton::setup()
{
    // check to see if compton is enabled
    QString home_path = QDir::homePath();
    qDebug() << "Home Path =" << home_path;
    QFileInfo file_start(home_path + "/.config/autostart/zcompton.desktop");
    //check to see if compton.desktop startup file exists
    if (file_start.exists()) {
        //check to see if Hidden=true or false
        QString status = runCmd("grep Hidden= " + file_start.absoluteFilePath()).output.section('=',1,1);
        qDebug() << "Autostart status is " << status;
        if ( status == "false") {
            ui->checkBoxautostart->setChecked(true);
        }
//        //check to see if XFCE only
//        status = runCmd("grep OnlyShowIn= " + file_start.absoluteFilePath()).output;
//        qDebug() << "XFCE only status is " << status;
//        if (status.contains("XFCE")) {
//            qDebug() << "XFCE only status is XFCE";
//            ui->checkXFCEonly->setChecked(true);
//        }
    } else {
        //copy in a startup file, startup initially disabled
        runCmd("cp /usr/share/mx-compton/zcompton.desktop " + file_start.absoluteFilePath());
    }

    //check to see if existing compton.conf file
    QFileInfo file_conf(home_path + "/.config/compton.conf");
    if (file_conf.exists()) {
        qDebug() << "Found existing conf file";
    } else {
         runCmd("cp /usr/share/mx-compton/compton.conf " + file_conf.absoluteFilePath());
    }

    //check to see if compton is running
    if ( system("pgrep --exact compton") == 0 ) {
        ui->comptonButton->setText(tr("Stop Compton"));
    }
}


mxcompton::~mxcompton()
{
    delete ui;
}



//void mxcompton::on_checkBoxautostart_toggled(bool checked)
//{
//    QString home_path = QDir::homePath();
//    QFileInfo file_start(home_path + "/.config/autostart/zcompton.desktop");
//    if (ui->checkBoxautostart->isChecked()) {
//        runCmd("sed -i -r s/Hidden=.*/Hidden=false/ " + file_start.absoluteFilePath());
//    } else {
//        runCmd("sed -i -r s/Hidden=.*/Hidden=true/ " + file_start.absoluteFilePath());
//    }
//}

void mxcompton::on_comptonButton_clicked()
{
    if (ui->comptonButton->text() == tr("Launch Compton")) {
        system("pkill -x compton");
        system("compton-launch.sh");
        ui->comptonButton->setText(tr("Stop Compton"));
    } else {
        system("pkill -x compton");
        ui->comptonButton->setText(tr("Launch Compton"));
    }
}

void mxcompton::on_configureButton_clicked()
{
    system("compton-conf &");
}

void mxcompton::on_buttonAbout_clicked()
{
    this->hide();
    QMessageBox msgBox(QMessageBox::NoIcon,
                       tr("About MX Compton"), "<p align=\"center\"><b><h2>" +
                       tr("MX Compton") + "</h2></b></p><p align=\"center\">" + tr("Version: ") + getVersion("mx-compton") + "</p><p align=\"center\"><h3>" +
                       tr("Configure Compton Settings") +
                       "</h3></p><p align=\"center\"><a href=\"http://mxlinux.org\">http://mxlinux.org</a><br /></p><p align=\"center\">" +
                       tr("Copyright (c) MX Linux") + "<br /><br /></p>", 0, this);
    msgBox.addButton(tr("Cancel"), QMessageBox::AcceptRole); // because we want to display the buttons in reverse order we use counter-intuitive roles.
    msgBox.addButton(tr("License"), QMessageBox::RejectRole);
    if (msgBox.exec() == QMessageBox::RejectRole) {
        system("mx-viewer file:///usr/share/doc/mx-compton/license.html '" + tr("MX Compton").toUtf8() + " " + tr("License").toUtf8() + "'");
    }
    this->show();
}



void mxcompton::on_buttonHelp_clicked()
{

    QString cmd = QString("mx-viewer https://mxlinux.org/wiki/help-files/help-mx-compton '%1'").arg(tr("MX Compton"));
    system(cmd.toUtf8());

}

// Get version of the program
QString mxcompton::getVersion(QString name)
{
    QString cmdstr = QString("dpkg -l %1 | awk 'NR==6 {print $3}'").arg(name);
    return runCmd(cmdstr).output;
}

void mxcompton::on_buttonCancel_clicked()
{
     qApp->quit();
}

//void mxcompton::on_checkXFCEonly_clicked()
//{
//    QString home_path = QDir::homePath();
//    QFileInfo file_start(home_path + "/.config/autostart/zcompton.desktop");
//    QString status = runCmd("grep OnlyShowIn= " + file_start.absoluteFilePath()).output;
//    QString status2 = status.remove("XFCE;");
//    qDebug() << "only show status after click is :" << status.section('=',1,-1);
//    if (ui->checkXFCEonly->isChecked()) {
//        qDebug() << "Adding XFCE; to list";
//        runCmd("sed -i -r 's/OnlyShowIn=.*/OnlyShowIn=XFCE;" + status.section('=',1,-1) + "/' " + file_start.absoluteFilePath());
//    } else {
//        qDebug() << " removing XFCE" << status2;
//        runCmd("sed -i -r 's/OnlyShowIn=.*/" + status2 + "/' " + file_start.absoluteFilePath());
//    }
//    qDebug() << "ending Only Show line " << runCmd("grep OnlyShowIn= " + file_start.absoluteFilePath()).output;
//}

void mxcompton::on_checkBoxautostart_clicked()
{
    QString home_path = QDir::homePath();
    QFileInfo file_start(home_path + "/.config/autostart/zcompton.desktop");
    if (ui->checkBoxautostart->isChecked()) {
        runCmd("sed -i -r s/Hidden=.*/Hidden=false/ " + file_start.absoluteFilePath());
    } else {
        runCmd("sed -i -r s/Hidden=.*/Hidden=true/ " + file_start.absoluteFilePath());
    }
    qDebug() << "autostart set to " << runCmd("grep Hidden= " + file_start.absoluteFilePath()).output;
}

void mxcompton::on_pushButton_clicked()
{
    QString home_path = QDir::homePath();
    QFileInfo file_conf(home_path + "/.config/compton.conf");
    runCmd("xdg-open " + file_conf.absoluteFilePath());
}
