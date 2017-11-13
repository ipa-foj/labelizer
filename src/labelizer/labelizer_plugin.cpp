// include header of the plugin
#include <labelizer/labelizer_plugin.hpp>

// ros includes
#include <ros/ros.h>

// Qt includes
#include <QString>
#include <QSize>
#include <QPixmap>

// Plugin-export libraries --> needed when building a rqt-plugin in c++
#include <pluginlib/class_list_macros.h>

/*
 * default constructor, setting up the initial QtWidget with a name and defining that the follwing code describes a
 * rqt_cpp plugin
 */
labelizer::LabelizerPlugin::LabelizerPlugin()
	: rqt_gui_cpp::Plugin(),
	  widget_(0)
{
	setObjectName("LabelizerPlugin");
}

/*
 * Function that initializes the plugin on rqt start. It connects the signals and slots and defines the
 * callback functions for the corresponding functions and objects.
 */
void labelizer::LabelizerPlugin::initPlugin(qt_gui_cpp::PluginContext &context)
{
	// create a new widget and set up the user-interface
	widget_ = new QWidget();
	ui_.setupUi(widget_);

	// add the user-interface to the rqt context window (s.t. it shows up in rqt)
	if (context.serialNumber() > 1)
	{
		widget_->setWindowTitle(widget_->windowTitle() + " (" + QString::number(context.serialNumber()) + ")");
	}
	context.addWidget(widget_);

	// testing, TODO: remove!!
	displayImage(cv::Mat());
}


/*
 * Function to shut down the plugin.
 */
void labelizer::LabelizerPlugin::shutdownPlugin()
{

}

/*
 * Function that converts the given cv::Mat image into a QImage and displays it in the image frame of the user-interface.
 */
void labelizer::LabelizerPlugin::displayImage(const cv::Mat& image)
{
//	QImage converted_image(image.data, image.cols, image.rows, image.step[0], QImage::Format_RGB888);
//	ui_.image_area->setImage(converted_image);

	QPixmap pic("/home/rmb-fj/Pictures/black+color/2.gif");
	ui_.image_area->setPixmap(pic);
	ui_.image_area->show();
}

/*
 * Export the labelizer plugin to allow rqt to load it.
 */
PLUGINLIB_EXPORT_CLASS(labelizer::LabelizerPlugin, rqt_gui_cpp::Plugin)
