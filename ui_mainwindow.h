/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout;
    QLabel *exeFilesLabel;
    QHBoxLayout *horizontalLayout_2;
    QLineEdit *pathToExeFilesLineEdit;
    QToolButton *pathToExeFilesToolButton;
    QLabel *argumentsLabel;
    QVBoxLayout *verticalLayout_3;
    QLineEdit *commandLineArgumentsLineEdit;
    QLabel *ScreenShotDirLabel;
    QHBoxLayout *horizontalLayout_3;
    QLineEdit *pathToScreenShotsLineEdit;
    QToolButton *pathToScreenShotsToolButton;
    QHBoxLayout *horizontalLayout;
    QLabel *intervalLabel;
    QSpinBox *timeOutSpinBox;
    QLabel *TerminateProcessByMaskLabel;
    QHBoxLayout *horizontalLayout_4;
    QLineEdit *terminateProcessByMaskLineEdit;
    QPushButton *LaunchPushButton;
    QPushButton *NextLaunchPushButton;
    QTextEdit *resultTextEdit;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(636, 518);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        verticalLayout_2 = new QVBoxLayout(centralWidget);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        exeFilesLabel = new QLabel(centralWidget);
        exeFilesLabel->setObjectName(QString::fromUtf8("exeFilesLabel"));

        verticalLayout->addWidget(exeFilesLabel);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        pathToExeFilesLineEdit = new QLineEdit(centralWidget);
        pathToExeFilesLineEdit->setObjectName(QString::fromUtf8("pathToExeFilesLineEdit"));

        horizontalLayout_2->addWidget(pathToExeFilesLineEdit);

        pathToExeFilesToolButton = new QToolButton(centralWidget);
        pathToExeFilesToolButton->setObjectName(QString::fromUtf8("pathToExeFilesToolButton"));

        horizontalLayout_2->addWidget(pathToExeFilesToolButton);


        verticalLayout->addLayout(horizontalLayout_2);

        argumentsLabel = new QLabel(centralWidget);
        argumentsLabel->setObjectName(QString::fromUtf8("argumentsLabel"));

        verticalLayout->addWidget(argumentsLabel);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        commandLineArgumentsLineEdit = new QLineEdit(centralWidget);
        commandLineArgumentsLineEdit->setObjectName(QString::fromUtf8("commandLineArgumentsLineEdit"));

        verticalLayout_3->addWidget(commandLineArgumentsLineEdit);


        verticalLayout->addLayout(verticalLayout_3);

        ScreenShotDirLabel = new QLabel(centralWidget);
        ScreenShotDirLabel->setObjectName(QString::fromUtf8("ScreenShotDirLabel"));

        verticalLayout->addWidget(ScreenShotDirLabel);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        pathToScreenShotsLineEdit = new QLineEdit(centralWidget);
        pathToScreenShotsLineEdit->setObjectName(QString::fromUtf8("pathToScreenShotsLineEdit"));

        horizontalLayout_3->addWidget(pathToScreenShotsLineEdit);

        pathToScreenShotsToolButton = new QToolButton(centralWidget);
        pathToScreenShotsToolButton->setObjectName(QString::fromUtf8("pathToScreenShotsToolButton"));

        horizontalLayout_3->addWidget(pathToScreenShotsToolButton);


        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        intervalLabel = new QLabel(centralWidget);
        intervalLabel->setObjectName(QString::fromUtf8("intervalLabel"));

        horizontalLayout->addWidget(intervalLabel);

        timeOutSpinBox = new QSpinBox(centralWidget);
        timeOutSpinBox->setObjectName(QString::fromUtf8("timeOutSpinBox"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(timeOutSpinBox->sizePolicy().hasHeightForWidth());
        timeOutSpinBox->setSizePolicy(sizePolicy);
        timeOutSpinBox->setMinimum(1);
        timeOutSpinBox->setMaximum(999);

        horizontalLayout->addWidget(timeOutSpinBox);


        verticalLayout->addLayout(horizontalLayout);

        TerminateProcessByMaskLabel = new QLabel(centralWidget);
        TerminateProcessByMaskLabel->setObjectName(QString::fromUtf8("TerminateProcessByMaskLabel"));

        verticalLayout->addWidget(TerminateProcessByMaskLabel);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        terminateProcessByMaskLineEdit = new QLineEdit(centralWidget);
        terminateProcessByMaskLineEdit->setObjectName(QString::fromUtf8("terminateProcessByMaskLineEdit"));

        horizontalLayout_4->addWidget(terminateProcessByMaskLineEdit);


        verticalLayout->addLayout(horizontalLayout_4);

        LaunchPushButton = new QPushButton(centralWidget);
        LaunchPushButton->setObjectName(QString::fromUtf8("LaunchPushButton"));

        verticalLayout->addWidget(LaunchPushButton);

        NextLaunchPushButton = new QPushButton(centralWidget);
        NextLaunchPushButton->setObjectName(QString::fromUtf8("NextLaunchPushButton"));
        NextLaunchPushButton->setEnabled(false);

        verticalLayout->addWidget(NextLaunchPushButton);

        resultTextEdit = new QTextEdit(centralWidget);
        resultTextEdit->setObjectName(QString::fromUtf8("resultTextEdit"));

        verticalLayout->addWidget(resultTextEdit);


        verticalLayout_2->addLayout(verticalLayout);

        MainWindow->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        exeFilesLabel->setText(QCoreApplication::translate("MainWindow", "Directory containing executable files:", nullptr));
        pathToExeFilesToolButton->setText(QCoreApplication::translate("MainWindow", "...", nullptr));
        argumentsLabel->setText(QCoreApplication::translate("MainWindow", "Command Line Arguments", nullptr));
        ScreenShotDirLabel->setText(QCoreApplication::translate("MainWindow", "Directory for screenshot images:", nullptr));
        pathToScreenShotsToolButton->setText(QCoreApplication::translate("MainWindow", "...", nullptr));
        intervalLabel->setText(QCoreApplication::translate("MainWindow", "Interval:", nullptr));
        timeOutSpinBox->setSuffix(QCoreApplication::translate("MainWindow", " seconds", nullptr));
        TerminateProcessByMaskLabel->setText(QCoreApplication::translate("MainWindow", "Terminate process by mask (separator is '|'):", nullptr));
        LaunchPushButton->setText(QCoreApplication::translate("MainWindow", "Launch", nullptr));
        NextLaunchPushButton->setText(QCoreApplication::translate("MainWindow", "Next launch", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
