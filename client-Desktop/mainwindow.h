#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "signinghandler.h"
#include <QMainWindow>
#include "signinghandler.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnOpenFile_clicked();

    void on_btnChainIt_clicked();

    void on_txtPyload_textChanged();

    void on_btnCheckIsValid_clicked();

    void on_signatureCreated(const QByteArray signature);
    void on_validateSignatureResult(const SigningHandler::SignatureValidation response);

private:
    Ui::MainWindow *ui;
    QByteArray _payload;
    SigningHandler *_signingHandler;
};
#endif // MAINWINDOW_H
