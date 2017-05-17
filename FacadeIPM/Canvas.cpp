#include "Canvas.h"
#include <QPainter>
#include <iostream>
#include "Utils.h"

Canvas::Canvas(QWidget *parent) : QWidget(parent) {
	ctrlPressed = false;
	shiftPressed = false;
}

Canvas::~Canvas() {
}

void Canvas::paintEvent(QPaintEvent *event) {
	if (!image.isNull()) {
		QPainter painter(this);
		painter.drawImage(0, 0, image);

		painter.setPen(QPen(QColor(255, 255, 0, 128), 3));
		for (int i = 0; i < y_splits.size() - 1; ++i) {
			for (int j = 0; j < x_splits.size() - 1; ++j) {
				if (win_rects[i][j].valid) {
					int x1 = (x_splits[j] + win_rects[i][j].left) / orig_image.width() * image.width();
					int y1 = (y_splits[i] + win_rects[i][j].top) / orig_image.height() * image.height();
					int x2 = (x_splits[j] + win_rects[i][j].right) / orig_image.width() * image.width();
					int y2 = (y_splits[i] + win_rects[i][j].bottom) / orig_image.height() * image.height();
					painter.drawRect(x1, y1, x2 - x1 + 1, y2 - y1 + 1);
				}
			}
		}

		painter.drawImage(0, 0, final_result);
	}
}

void Canvas::resizeEvent(QResizeEvent *e) {
	if (!orig_image.isNull()) {
		float scale = std::min((float)width() / orig_image.width(), (float)height() / orig_image.height());
		image = orig_image.scaled(orig_image.width() * scale, orig_image.height() * scale);
	}
}

void Canvas::set(const QString& filename, const std::vector<float>& y_splits, const std::vector<float>& x_splits, const std::vector<std::vector<fs::WindowPos>>& win_rects, const cv::Mat& final_result) {
	orig_image = QImage(filename);
	float scale = std::min((float)width() / orig_image.width(), (float)height() / orig_image.height());
	image = orig_image.scaled(orig_image.width() * scale, orig_image.height() * scale);

	this->y_splits = y_splits;
	this->x_splits = x_splits;
	this->win_rects = win_rects;

	cv::Mat resized_final_result;
	cv::resize(final_result, resized_final_result, cv::Size(image.width(), image.height()));
	this->final_result = QImage(image.width(), image.height(), QImage::Format_ARGB32);
	this->final_result.fill(QColor(0, 0, 0, 0));
	QPainter painter(&this->final_result);
	painter.setPen(QPen(QColor(0, 0, 255, 128), 1));
	painter.setBrush(QBrush(QColor(0, 0, 255, 128)));
	for (int y = 0; y < resized_final_result.rows; y++) {
		for (int x = 0; x < resized_final_result.cols; x++) {
			cv::Vec3b col = resized_final_result.at<cv::Vec3b>(y, x);
			if (col[0] == 0 && col[1] == 0 && col[2] == 255) {
				painter.drawLine(x, y, x + 1, y);
			}
		}
	}

	update();
}

void Canvas::keyPressEvent(QKeyEvent* e) {
	ctrlPressed = false;
	shiftPressed = false;

	if (e->modifiers() & Qt::ControlModifier) {
		ctrlPressed = true;
	}
	if (e->modifiers() & Qt::ShiftModifier) {
		shiftPressed = true;
	}

	switch (e->key()) {
	case Qt::Key_Space:
		break;
	}

	update();
}

void Canvas::keyReleaseEvent(QKeyEvent* e) {
	switch (e->key()) {
	case Qt::Key_Control:
		ctrlPressed = false;
		break;
	case Qt::Key_Shift:
		shiftPressed = false;
		break;
	default:
		break;
	}
}

