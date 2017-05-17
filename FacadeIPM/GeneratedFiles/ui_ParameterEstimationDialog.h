/********************************************************************************
** Form generated from reading UI file 'ParameterEstimationDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PARAMETERESTIMATIONDIALOG_H
#define UI_PARAMETERESTIMATIONDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_ParameterEstimationDialog
{
public:
    QLineEdit *lineEditFacadeImage;
    QPushButton *pushButtonCancel;
    QPushButton *pushButtonOK;
    QLabel *label_15;
    QPushButton *pushButtonFacadeImage;
    QCheckBox *checkBoxAutoDetection;
    QLabel *label_16;
    QSpinBox *spinBoxGrammarID;

    void setupUi(QDialog *ParameterEstimationDialog)
    {
        if (ParameterEstimationDialog->objectName().isEmpty())
            ParameterEstimationDialog->setObjectName(QStringLiteral("ParameterEstimationDialog"));
        ParameterEstimationDialog->resize(522, 120);
        lineEditFacadeImage = new QLineEdit(ParameterEstimationDialog);
        lineEditFacadeImage->setObjectName(QStringLiteral("lineEditFacadeImage"));
        lineEditFacadeImage->setGeometry(QRect(100, 10, 371, 20));
        pushButtonCancel = new QPushButton(ParameterEstimationDialog);
        pushButtonCancel->setObjectName(QStringLiteral("pushButtonCancel"));
        pushButtonCancel->setGeometry(QRect(420, 80, 91, 31));
        pushButtonOK = new QPushButton(ParameterEstimationDialog);
        pushButtonOK->setObjectName(QStringLiteral("pushButtonOK"));
        pushButtonOK->setGeometry(QRect(320, 80, 91, 31));
        label_15 = new QLabel(ParameterEstimationDialog);
        label_15->setObjectName(QStringLiteral("label_15"));
        label_15->setGeometry(QRect(270, 40, 101, 20));
        label_15->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        pushButtonFacadeImage = new QPushButton(ParameterEstimationDialog);
        pushButtonFacadeImage->setObjectName(QStringLiteral("pushButtonFacadeImage"));
        pushButtonFacadeImage->setGeometry(QRect(480, 10, 31, 23));
        checkBoxAutoDetection = new QCheckBox(ParameterEstimationDialog);
        checkBoxAutoDetection->setObjectName(QStringLiteral("checkBoxAutoDetection"));
        checkBoxAutoDetection->setGeometry(QRect(20, 40, 211, 21));
        label_16 = new QLabel(ParameterEstimationDialog);
        label_16->setObjectName(QStringLiteral("label_16"));
        label_16->setGeometry(QRect(20, 10, 81, 20));
        label_16->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        spinBoxGrammarID = new QSpinBox(ParameterEstimationDialog);
        spinBoxGrammarID->setObjectName(QStringLiteral("spinBoxGrammarID"));
        spinBoxGrammarID->setGeometry(QRect(380, 40, 51, 22));
        QWidget::setTabOrder(lineEditFacadeImage, pushButtonOK);
        QWidget::setTabOrder(pushButtonOK, pushButtonCancel);

        retranslateUi(ParameterEstimationDialog);

        QMetaObject::connectSlotsByName(ParameterEstimationDialog);
    } // setupUi

    void retranslateUi(QDialog *ParameterEstimationDialog)
    {
        ParameterEstimationDialog->setWindowTitle(QApplication::translate("ParameterEstimationDialog", "ParameterEstimationDialog", 0));
        pushButtonCancel->setText(QApplication::translate("ParameterEstimationDialog", "Cancel", 0));
        pushButtonOK->setText(QApplication::translate("ParameterEstimationDialog", "OK", 0));
        label_15->setText(QApplication::translate("ParameterEstimationDialog", "Facade grammar ID:", 0));
        pushButtonFacadeImage->setText(QApplication::translate("ParameterEstimationDialog", "...", 0));
        checkBoxAutoDetection->setText(QApplication::translate("ParameterEstimationDialog", "Automatic detection of grammar ID", 0));
        label_16->setText(QApplication::translate("ParameterEstimationDialog", "Facade image:", 0));
    } // retranslateUi

};

namespace Ui {
    class ParameterEstimationDialog: public Ui_ParameterEstimationDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PARAMETERESTIMATIONDIALOG_H
