#ifndef MXCOMPTON_H
#define MXCOMPTON_H

#include <QMainWindow>
#include <QMessageBox>
#include <QProcess>
#include <QFile>

namespace Ui {
class mxcompton;
}

struct Result {
    int exitCode;
    QString output;
};

class mxcompton : public QMainWindow
{
    Q_OBJECT


protected:
    QProcess *proc;
    QTimer *timer;

public:
    explicit mxcompton(QWidget *parent = 0);
    ~mxcompton();

    Result runCmd(QString cmd);
    QString getVersion(QString name);
    void setup();
    void CheckComptonRunning();

private slots:
    //void on_checkBoxautostart_toggled(bool checked);

    void on_comptonButton_clicked();

    void on_configureButton_clicked();

    void on_buttonAbout_clicked();

    void on_buttonHelp_clicked();

    void on_buttonCancel_clicked();

//    void on_checkXFCEonly_clicked();

    void on_checkBoxautostart_clicked();

    void on_pushButton_clicked();

private:
    Ui::mxcompton *ui;

};

#endif // MXCOMPTON_H
