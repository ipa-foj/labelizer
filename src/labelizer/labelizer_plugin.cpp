// include header of the plugin
#include <labelizer/labelizer_plugin.hpp>

// ros includes
#include <ros/ros.h>

// OpenCV includes
#include <cv_bridge/cv_bridge.h>
#include <opencv/cv.h>

// Qt includes
#include <QString>
#include <QSize>

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

	//
	if (context.serialNumber() > 1)
	{
		widget_->setWindowTitle(widget_->windowTitle() + " (" + QString::number(context.serialNumber()) + ")");
	}
	context.addWidget(widget_);
}


/*
 * Function to shut down the plugin.
 */
void labelizer::LabelizerPlugin::shutdownPlugin()
{

}

/*
 * Export the labelizer plugin to allow rqt to load it.
 */
PLUGINLIB_EXPORT_CLASS(labelizer::LabelizerPlugin, rqt_gui_cpp::Plugin)
