// include header of the plugin
#include <labelizer/labelizer_plugin.hpp>

// std includes
#include <fstream>

// ros includes
#include <ros/ros.h>
#include <ros/package.h>

// Qt includes
#include <QString>
#include <QSize>
#include <QPixmap>
#include <QDir>
#include <QProcess>
#include <QGraphicsScene>

// Plugin-export libraries --> needed when building a rqt-plugin in c++
#include <pluginlib/class_list_macros.h>

// Json-parser includes
#include <jsoncpp/json/json.h>

// Boost
//#include <boost/python.hpp>

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
	// ********** create a new widget and set up the user-interface **********
	widget_ = new QWidget();
	ui_.setupUi(widget_);

	// ********** add the user-interface to the rqt context window (s.t. it shows up in rqt) **********
	if (context.serialNumber() > 1)
	{
		widget_->setWindowTitle(widget_->windowTitle() + " (" + QString::number(context.serialNumber()) + ")");
	}
	context.addWidget(widget_);

	// ********** display the names of the color to allow the user to select the one he wants to label **********
	// ------- read in the json-file in which the names are stored -------
	std::string package_path = ros::package::getPath("labelizer");
	std::string file_path = package_path + "/config/parameters.json";
	std::ifstream json_file(file_path.c_str(), std::ifstream::binary);
	Json::Reader json_reader;
	Json::Value root;
	bool parsing_successful = json_reader.parse(json_file, root, false);

	if(parsing_successful==false)
	{
		ROS_FATAL("couldn't parse json-config file for labelizer");
		return;
	}

	// ------- add the color names to the list widget, s.t. they can be selected -------
	for(auto& val : root["colors"])
	{
		ui_.color_name_list->addItem(QString::fromStdString(val.asString()));
	}

	// ------- initially set the selected color to the first one provided -------
	ui_.color_name_list->setCurrentItem(ui_.color_name_list->item(0));

	// ********** read out the stored keywords, that will be added for the image search **********
	for(auto& keyword : root["search_extensions"])
	{
		color_search_keyowrds_.push_back(keyword.asString());
	}

	// ********** set up the frame widget that shows the to-be-labeled images and an empty opencv-image **********
	image_scene_ = new MouseQScene(this);
	image_ = cv::Mat();

	// ********** connect signals and slots **********
	connect(ui_.dwnld_btn, SIGNAL(pressed()), this, SLOT(downloadImages()));
	connect(ui_.start_btn, SIGNAL(pressed()), this, SLOT(labelImages()));
	connect(image_scene_, SIGNAL(imageClicked(double,double)), this, SLOT(showClickCoordinates(double,double)));
}


/*
 * Function to shut down the plugin.
 */
void labelizer::LabelizerPlugin::shutdownPlugin()
{
	// ********** delete the pointers **********
	delete image_scene_;
}

/*
 * Function that converts the given cv::Mat image into a QImage and displays it in the image frame of the user-interface.
 */
void labelizer::LabelizerPlugin::displayImage(const QString& image_path)
{
	// ********** show the image in the GUI **********
	QPixmap pic(image_path);
	image_scene_->clear();
	image_scene_->addPixmap(pic);
	image_scene_->setSceneRect(pic.rect());
	ui_.image_frame->setScene(image_scene_);

	// ********** load the image as opencv image for further handling **********
	image_ = cv::imread(image_path.toUtf8().constData(), CV_LOAD_IMAGE_COLOR);
}

/*
 * Function to download images from google-images that show up, when searching for the selected color.
 */
void labelizer::LabelizerPlugin::downloadImages()
{
	for(int color_index=0; color_index<ui_.color_name_list->count(); ++color_index)
	{
		// get the current color that should be searched
		std::string color_name = ui_.color_name_list->item(color_index)->text().toUtf8().constData();

		// get the path to the package
		std::string package_path = ros::package::getPath("labelizer");

		// launch the python script to search and download the images from google images
		QString python_script_path = QString::fromStdString(package_path + "/python/google-images-download.py");
		QString command("python");
		QStringList args;
		args << python_script_path << "--color_name" << QString::fromStdString(color_name) << "--keywords";
		for(const std::string& keyword : color_search_keyowrds_)
		{
			args << QString::fromStdString(keyword);
		}
		QProcess *python_script_process = new QProcess(this);
		python_script_process->start(command, args);
		python_script_process->waitForFinished(2147483647); // wait for n ms --> maximal integer value, ~35min
		delete python_script_process;
	}
}

/*
 * Function that starts the labeling process for the currently selected color.
 */
void labelizer::LabelizerPlugin::labelImages()
{
	// get the current color that should be searched and the path to the folder in which the images are stored
	std::string color_name = ui_.color_name_list->currentItem()->text().toUtf8().constData();
	std::string folder_path = ros::package::getPath("labelizer") + "/python/" + color_name + "/";

	// display the first image
	displayImage(QString::fromStdString(folder_path+"1.jpg"));

	// get the number of images that have been downloaded
	QDir directory(QString::fromStdString(folder_path));
	int number_of_files = directory.count()-2; // -2 because the upper directories are also counted
	ui_.dwnld_progress_bar->setValue(number_of_files/10);
}

void labelizer::LabelizerPlugin::showClickCoordinates(const double x, const double y)
{
	std::stringstream text_string;
	text_string << "(" << x << " | " << y << ")";
	QString text = QString::fromStdString(text_string.str());
	ui_.color_region->setText(text);
}


/*
 * Export the labelizer plugin to allow rqt to load it.
 */
PLUGINLIB_EXPORT_CLASS(labelizer::LabelizerPlugin, rqt_gui_cpp::Plugin)
