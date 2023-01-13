#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFileDialog>
#include <QFile>
#include <QDebug>
#include <QElapsedTimer>
#include "signinghandler.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //_signingHandler = new SigningHandler("http://45.79.200.52:8080", this);
    _signingHandler = new SigningHandler("http://127.0.0.1:8080", this);
    connect(_signingHandler, &SigningHandler::signatureCreated, this, &MainWindow::on_signatureCreated);
    connect(_signingHandler, &SigningHandler::validateSignatureResult, this, &MainWindow::on_validateSignatureResult);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_btnOpenFile_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open File"));

    if (!filename.isEmpty()) {
        QFile file(filename);
        if (file.open(QIODevice::ReadOnly)) {
            _payload = file.readAll();
            ui->txtPyload->document()->setPlainText(_payload);
        }
    }
}


void MainWindow::on_btnChainIt_clicked()
{
    _signingHandler->createSignature(QByteArray(), _payload);
}


void MainWindow::on_txtPyload_textChanged()
{
    _payload = ui->txtPyload->document()->toRawText().toUtf8();
}


void MainWindow::on_btnCheckIsValid_clicked()
{
    QByteArray signature = QByteArray::fromHex(ui->txtResult->text().toUtf8());
    _signingHandler->validateSignature(_payload, signature);
}

void MainWindow::on_signatureCreated(const QByteArray signature)
{
    ui->txtResult->setText(signature.toHex());
}

void MainWindow::on_validateSignatureResult(const SigningHandler::SignatureValidation response)
{
    QString txt = "";
    switch (response) {
    case SigningHandler::SignatureValidation::InvalidSignature:
        txt = tr("Invalid signature");
        break;
    case SigningHandler::SignatureValidation::NonExistentSignature:
        txt = tr("Nonexistent signature");
        break;
    case SigningHandler::SignatureValidation::ValidSignature:
        txt = tr("Valid signature");
        break;
    }

    ui->lblIsValid->setText(txt);
}

void MainWindow::on_btnLogin_clicked()
{
    QString email = ui->txtEmail->text();
    QString password = ui->txtPassword->text();

    if ((email.length() > 0) && (password.length() > 0)) {
        _signingHandler->login(email, password);
    }
}


void MainWindow::on_btnAddUser_clicked()
{
    QString email = ui->txtEmail->text();
    QString password = ui->txtPassword->text();
    if ((email.length() > 0) && (password.length() > 0)) {
        _signingHandler->addUser(email, password);
    }
}

