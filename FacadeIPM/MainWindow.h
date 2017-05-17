#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"
#include <opencv2/opencv.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>
#include "Classifier.h"
#include "Regression.h"
#include "Canvas.h"

class MainWindow : public QMainWindow {
	Q_OBJECT

private:
	Ui::MainWindowClass ui;
	Canvas* canvas;

	boost::shared_ptr<Classifier> fac_classifier;
	std::vector<boost::shared_ptr<Regression>> fac_regressions;
	boost::shared_ptr<Regression> floors_regression;
	boost::shared_ptr<Classifier> win_exist_classifier;
	boost::shared_ptr<Regression> win_pos_regression;
	boost::shared_ptr<Classifier> win_classifier;
	bool cnns_loaded;

public:
	MainWindow(QWidget *parent = 0);

	void loadCNNs();

public slots:
	void onParameterEstimation();
};

#endif // MAINWINDOW_H
