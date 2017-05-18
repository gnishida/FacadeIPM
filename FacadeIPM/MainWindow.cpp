#include "MainWindow.h"
#include <QFileDialog>
#include <QDir>
#include <QFileInfo>
#include <boost/filesystem.hpp>
#include "Classifier.h"
#include "Regression.h"
#include "Utils.h"
#include "ParameterEstimationDialog.h"
#include "facadeA.h"
#include "facadeB.h"
#include "facadeC.h"
#include "facadeD.h"
#include "facadeE.h"
#include "facadeF.h"
#include "facadeG.h"
#include "facadeH.h"
#include <boost/algorithm/string.hpp>
#include <boost/shared_ptr.hpp>
#include <QTextStream>
#include "FacadeSegmentation.h"
#include "WindowPositioning.h"
#include "WindowRecognition.h"
#include "FacadeReconstruction.h"

#ifndef SQR
#define SQR(x)	((x) * (x))
#endif

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
	ui.setupUi(this);

	canvas = new Canvas(this);
	setCentralWidget(canvas);

	connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));
	connect(ui.actionParameterEstimation, SIGNAL(triggered()), this, SLOT(onParameterEstimation()));

	cnns_loaded = false;
}

void MainWindow::onParameterEstimation() {
	ParameterEstimationDialog dlg;
	if (!dlg.exec()) return;

	QString filename = dlg.ui.lineEditFacadeImage->text();
	if (filename.isEmpty()) return;

	loadCNNs();
	setWindowTitle("Facade IPM - " + filename);

	cv::Mat facade_img = cv::imread(filename.toUtf8().constData());

	// floor height / column width
	cv::Mat resized_facade_img;
	cv::resize(facade_img, resized_facade_img, cv::Size(227, 227));
	std::vector<float> floor_params = floors_regression->Predict(resized_facade_img);
	int num_floors = std::round(floor_params[0] + 0.3);
	int num_columns = std::round(floor_params[1] + 0.3);
	float average_floor_height = (float)facade_img.rows / num_floors;
	float average_column_width = (float)facade_img.cols / num_columns;

	//////////////////////////////////////////////////////////////////////////////////
	// DEBUG
	std::cout << "----------------------------------------------" << std::endl;
	std::cout << "#floors = " << num_floors << ", #columns = " << num_columns << std::endl;
	std::cout << "----------------------------------------------" << std::endl;
	//////////////////////////////////////////////////////////////////////////////////

	// subdivide the facade into tiles and windows
	std::vector<float> x_splits;
	std::vector<float> y_splits;
	std::vector<std::vector<fs::WindowPos>> win_rects;
	fs::subdivideFacade(facade_img, average_floor_height, average_column_width, y_splits, x_splits, win_rects);

	// update #floors and #columns
	num_floors = y_splits.size() - 1;
	num_columns = x_splits.size() - 1;

	//////////////////////////////////////////////////////////////////////////////////
	// DEBUG
	std::cout << "----------------------------------------------" << std::endl;
	std::cout << "updated after subdivision:" << std::endl;
	std::cout << "#floors = " << num_floors << ", #columns = " << num_columns << std::endl;
	std::cout << "----------------------------------------------" << std::endl;
	//////////////////////////////////////////////////////////////////////////////////

	// obtain the dominant facade color
	cv::Scalar facade_color = fs::getDominantColor(facade_img, y_splits, x_splits, win_rects, 10);
	std::cout << "Facade color = (" << facade_color[0] << "," << facade_color[1] << "," << facade_color[2] << ")" << std::endl;

	// gray scale
	cv::Mat facade_gray_img;
	cv::cvtColor(facade_img, facade_gray_img, cv::COLOR_BGR2GRAY);
	cv::cvtColor(facade_gray_img, facade_gray_img, cv::COLOR_GRAY2BGR);

	// use window positioning CNN to locate windows
	for (int i = 0; i < y_splits.size() - 1; ++i) {
		for (int j = 0; j < x_splits.size() - 1; ++j) {
			cv::Mat tile_img(facade_gray_img, cv::Rect(x_splits[j], y_splits[i], x_splits[j + 1] - x_splits[j] + 1, y_splits[i + 1] - y_splits[i] + 1));

			cv::Mat resized_tile_img;
			cv::resize(tile_img, resized_tile_img, cv::Size(227, 227));

			// check the existence of window
			std::vector<Prediction> pred_exist = win_exist_classifier->Classify(resized_tile_img, 2);
			if (pred_exist[0].first == 1) {
				win_rects[i][j].valid = fs::WindowPos::VALID;
			}
			else {
				win_rects[i][j].valid = fs::WindowPos::INVALID;
			}

			if (fs::WindowPos::VALID) {
				// predict the window position
				//std::vector<float> pred_params = win_pos_regression->Predict(resized_tile_img);
				std::vector<float> pred_params = wp::parameterEstimation(win_pos_regression, resized_tile_img, true, 0.1, 3000);
				//utils::output_vector(pred_params);
				win_rects[i][j].left = std::round(pred_params[0] * tile_img.cols);
				win_rects[i][j].top = std::round(pred_params[1] * tile_img.rows);
				win_rects[i][j].right = std::round(pred_params[2] * tile_img.cols);
				win_rects[i][j].bottom = std::round(pred_params[3] * tile_img.rows);
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	// HACK:
	// refine the #floors / #columns
	for (int i = 0; i < y_splits.size() - 1; ) {
		int win_nums = 0;
		for (int j = 0; j < x_splits.size() - 1; ++j) {
			if (win_rects[i][j].valid) win_nums++;
		}

		// if there are too small number of windows detected on this floor,
		// assume that they are false detection, and remove them.
		if (win_nums < (float)(x_splits.size() - 1) * 0.3) {
			for (int j = 0; j < x_splits.size() - 1; ++j) {
				win_rects[i][j].valid = fs::WindowPos::INVALID;
			}
			num_floors--;
			if (i < y_splits.size() - 2) {
				y_splits.erase(y_splits.begin() + i + 1);
			}
			else {
				y_splits.erase(y_splits.begin() + i);
			}
			win_rects.erase(win_rects.begin() + i);
		}
		else {
			i++;
		}
	}
	for (int j = 0; j < x_splits.size() - 1; ++j) {
		int win_nums = 0;
		for (int i = 0; i < y_splits.size() - 1; ++i) {
			if (win_rects[i][j].valid) win_nums++;
		}

		// if there are too small number of windows detected on this column,
		// assume that they are false detection, and remove them.
		if (win_nums < (float)(y_splits.size() - 1) * 0.3) {
			for (int i = 0; i < y_splits.size() - 1; ++i) {
				win_rects[i][j].valid = fs::WindowPos::INVALID;
			}
			num_columns--;
		}
	}

	std::cout << "----------------------------------------------" << std::endl;
	std::cout << "updated after removing non-window floors/columns:" << std::endl;
	std::cout << "#updated floors = " << num_floors << ", #updated columns = " << num_columns << std::endl;
	std::cout << "----------------------------------------------" << std::endl;
	//////////////////////////////////////////////////////////////////////////////////////////


	// generate facade segmentation image
	cv::Mat segmentation_result = facade_img.clone();
	fs::generateFacadeSubdivisionImage(y_splits, x_splits, 3, cv::Scalar(0, 255, 255), segmentation_result);

	// generate initial facade parsing image
	cv::Mat initial_result = cv::Mat(facade_img.rows, facade_img.cols, CV_8UC3, cv::Scalar(0, 255, 255));
	fs::generateWindowImage(y_splits, x_splits, win_rects, -1, cv::Scalar(0, 0, 255), initial_result);

	cv::Mat initial_facade_parsing = cv::Mat(facade_img.rows, facade_img.cols, CV_8UC3, cv::Scalar(255, 255, 255));// cv::Scalar(0, 255, 255));
	fs::generateWindowImage(y_splits, x_splits, win_rects, -1, cv::Scalar(0, 0, 0), initial_facade_parsing);

	// generate input image for facade CNN
	cv::Mat input_img(227, 227, CV_8UC3, cv::Scalar(255, 255, 255));
	fs::generateWindowImage(y_splits, x_splits, win_rects, 1, cv::Scalar(0, 0, 0), input_img);
	//cv::imwrite("window227.png", input_img);

	// classification
	int facade_id;
	if (dlg.ui.checkBoxAutoDetection->isChecked()) {
		facade_id = facarec::recognition(fac_classifier, input_img, -1, num_floors);
		/*
		std::vector<Prediction> fac_predictions = fac_classifier->Classify(input_img, fac_regressions.size());
		facade_id = fac_predictions[0].first;
		for (int i = 0; i < fac_predictions.size(); ++i) {
			std::cout << fac_predictions[i].first << ": " << fac_predictions[i].second << std::endl;
		}

		////////////////////////////////////////////////////////////////////////////////
		// HACK: for facade grammar recognition
		// choose an appropriate facade grammar based on #floors
		if (num_floors == 1) {
			if (facade_id == 1 || facade_id == 2 || facade_id == 3) {
				facade_id = 0;
			}
			else if (facade_id == 5 || facade_id == 6 || facade_id == 7) {
				facade_id = 4;
			}
		}
		else if (num_floors == 2) {
			if (facade_id == 0) {
				facade_id = 1;
			}
			else if (facade_id == 2 || facade_id == 3) {
				facade_id = 1;
			}
			else if (facade_id == 4) {
				facade_id = 5;
			}
			else if (facade_id == 6) {
				facade_id = 5;
			}
		}
		else if (num_floors == 3) {
			if (facade_id == 0) {
				facade_id = 1;
			}
			else if (facade_id == 3) {
				facade_id = 2;
			}
			else if (facade_id == 4) {
				facade_id = 5;
			}
			else if (facade_id == 6) {
				facade_id = 5;
			}
		}
		else {
			// It is better to have different style for the 1st floor unless the number of floors is 1.
			if (facade_id == 0) {
				facade_id = 1;
			}
			else if (facade_id == 4) {
				facade_id = 5;
			}
		}
		*/
	}
	else {
		facade_id = dlg.ui.spinBoxGrammarID->value() - 1;
	}
	std::cout << "facade grammar: " << facade_id + 1 << std::endl;


	// recognize window styles
	std::vector<int> selected_win_types = winrec::recognition(facade_img, facade_id, y_splits, x_splits, win_rects, win_classifier);


	// parameter estimation
	std::vector<float> predicted_params = facarec::parameterEstimation(facade_id, fac_regressions[facade_id], input_img, facade_img.cols, facade_img.rows, num_floors, num_columns, initial_facade_parsing, selected_win_types);
	utils::output_vector(predicted_params);

	// generate final facade parsing image
	cv::Mat final_result;
	facarec::generateFacadeImage(facade_id, facade_img.cols, facade_img.rows, num_floors, num_columns, predicted_params, selected_win_types, -1, cv::Scalar(0, 255, 255), cv::Scalar(0, 0, 255), final_result);



	cv::imwrite("result_segmentation.png", segmentation_result);
	cv::imwrite("result_initial.png", initial_result);
	cv::imwrite("result_final.png", final_result);

	canvas->set(filename, y_splits, x_splits, win_rects, final_result);
}

void MainWindow::loadCNNs() {
	if (cnns_loaded) return;

	// load trained CNNs
	fac_classifier = boost::shared_ptr<Classifier>(new Classifier("models/facade/deploy.prototxt", "models/facade/train_iter_40000.caffemodel", "models/facade/mean.binaryproto"));
	fac_regressions.resize(8);
	fac_regressions[0] = boost::shared_ptr<Regression>(new Regression("models/facade/deploy_01.prototxt", "models/facade/train_01_iter_40000.caffemodel"));
	fac_regressions[1] = boost::shared_ptr<Regression>(new Regression("models/facade/deploy_02.prototxt", "models/facade/train_02_iter_40000.caffemodel"));
	fac_regressions[2] = boost::shared_ptr<Regression>(new Regression("models/facade/deploy_03.prototxt", "models/facade/train_03_iter_40000.caffemodel"));
	fac_regressions[3] = boost::shared_ptr<Regression>(new Regression("models/facade/deploy_04.prototxt", "models/facade/train_04_iter_40000.caffemodel"));
	fac_regressions[4] = boost::shared_ptr<Regression>(new Regression("models/facade/deploy_05.prototxt", "models/facade/train_05_iter_40000.caffemodel"));
	fac_regressions[5] = boost::shared_ptr<Regression>(new Regression("models/facade/deploy_06.prototxt", "models/facade/train_06_iter_40000.caffemodel"));
	fac_regressions[6] = boost::shared_ptr<Regression>(new Regression("models/facade/deploy_07.prototxt", "models/facade/train_07_iter_40000.caffemodel"));
	fac_regressions[7] = boost::shared_ptr<Regression>(new Regression("models/facade/deploy_08.prototxt", "models/facade/train_08_iter_40000.caffemodel"));

	floors_regression = boost::shared_ptr<Regression>(new Regression("models/floors/deploy_01.prototxt", "models/floors/train_01_iter_40000.caffemodel"));
	win_exist_classifier = boost::shared_ptr<Classifier>(new Classifier("models/window_existence/deploy.prototxt", "models/window_existence/train_iter_40000.caffemodel", "models/window_existence/mean.binaryproto"));
	win_pos_regression = boost::shared_ptr<Regression>(new Regression("models/window_position/deploy_01.prototxt", "models/window_position/train_01_iter_80000.caffemodel"));
	win_classifier = boost::shared_ptr<Classifier>(new Classifier("models/window/deploy.prototxt", "models/window/train_iter_40000.caffemodel", "models/window/mean.binaryproto"));

	cnns_loaded = true;
}