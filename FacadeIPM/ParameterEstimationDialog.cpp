#include "ParameterEstimationDialog.h"
#include <QFileDialog>

ParameterEstimationDialog::ParameterEstimationDialog(QWidget *parent) : QDialog(parent) {
	ui.setupUi(this);

	ui.checkBoxAutoDetection->setChecked(true);
	ui.spinBoxGrammarID->setRange(1, 8);
	ui.checkBoxOptimization->setChecked(true);

	connect(ui.pushButtonFacadeImage, SIGNAL(clicked()), this, SLOT(onFacadeImage()));
	connect(ui.pushButtonOK, SIGNAL(clicked()), this, SLOT(onOK()));
	connect(ui.pushButtonCancel, SIGNAL(clicked()), this, SLOT(onCancel()));
}

ParameterEstimationDialog::~ParameterEstimationDialog() {
}

void ParameterEstimationDialog::onFacadeImage() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Open Image"), "", tr("Image Files (*.png *.jpg *.bmp)"));
	//QString dir = QFileDialog::getOpenFileName(this, tr("Open Directory"), ui.lineEditFacadeImage->text());
	if (!filename.isEmpty()) {
		ui.lineEditFacadeImage->setText(filename);
	}
}

void ParameterEstimationDialog::onOK() {
	accept();
}

void ParameterEstimationDialog::onCancel() {
	reject();
}
