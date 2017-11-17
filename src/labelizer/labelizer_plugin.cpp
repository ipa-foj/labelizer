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
#include <QImage>
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
	segmented_image_scene_ = new MouseQScene(this);
	image_ = cv::Mat();

	// ********** initially set the channel-sigmas to 0 **********
	H_lower_ = 0.0;
	H_upper_ = 0.0;
	S_lower_ = 0.0;
	S_upper_ = 0.0;
	V_lower_ = 0.0;
	V_upper_ = 0.0;

	// ********** connect signals and slots **********
	connect(ui_.dwnld_btn, SIGNAL(pressed()), this, SLOT(downloadImages()));
	connect(ui_.start_btn, SIGNAL(pressed()), this, SLOT(startLabeleingImages()));
	connect(ui_.finished_btn, SIGNAL(pressed()), this, SLOT(labelNextImage()));
	connect(ui_.skip_btn, SIGNAL(pressed()), this, SLOT(saveNegativeLabelImage()));
	connect(image_scene_, SIGNAL(imageClicked(double,double)), this, SLOT(newPixelSelected(double,double)));
	connect(image_scene_, SIGNAL(mouseOnImage(double,double)), this,  SLOT(selectImagePixels(double,double)));
	connect(ui_.H_spin_lower, SIGNAL(valueChanged(double)), this, SLOT(sigmaHLbChanged(double)));
	connect(ui_.H_spin_upper, SIGNAL(valueChanged(double)), this, SLOT(sigmaHUbChanged(double)));
	connect(ui_.S_spin_lower, SIGNAL(valueChanged(double)), this, SLOT(sigmaSLbChanged(double)));
	connect(ui_.S_spin_upper, SIGNAL(valueChanged(double)), this, SLOT(sigmaSUbChanged(double)));
	connect(ui_.V_spin_lower, SIGNAL(valueChanged(double)), this, SLOT(sigmaVLbChanged(double)));
	connect(ui_.V_spin_upper, SIGNAL(valueChanged(double)), this, SLOT(sigmaVUbChanged(double)));
}


/*
 * Function to shut down the plugin.
 */
void labelizer::LabelizerPlugin::shutdownPlugin()
{
	// ********** delete the pointers **********
	delete image_scene_;
	delete segmented_image_scene_;
}

/*
 * Function that converts the given cv::Mat image into a QImage and displays it in the image frame of the user-interface.
 */
void labelizer::LabelizerPlugin::displayImage(const QString& image_path)
{
	// ********** load the image as opencv image for further handling **********
	image_ = cv::imread(image_path.toUtf8().constData(), CV_LOAD_IMAGE_COLOR);

	// ********** show the image in the GUI **********
	image_scene_->clear();
	if(image_.rows!=0 && image_.cols!=0)
	{
		cv::Mat rgb_image;
		cv::cvtColor(image_, rgb_image, CV_BGR2RGB);
		QPixmap pic = QPixmap::fromImage(QImage(rgb_image.data, rgb_image.cols,
												rgb_image.rows, rgb_image.step, QImage::Format_RGB888));
		image_scene_->addPixmap(pic);
		image_scene_->setSceneRect(pic.rect());
		ui_.image_frame->setScene(image_scene_);
	}
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
void labelizer::LabelizerPlugin::startLabeleingImages()
{
	// ********** get the current color that should be searched and the path to the folder in which the images are stored **********
	std::string color_name = ui_.color_name_list->currentItem()->text().toUtf8().constData();
	std::string folder_path = ros::package::getPath("labelizer") + "/python/" + color_name + "/";

	// ********** get the number of images that have been downloaded **********
	QDir directory(QString::fromStdString(folder_path));
	ui_.dwnld_progress_bar->setValue(0.0);
	if(directory.exists()==true)
	{
		number_of_files_ = directory.count()-2; // -2 because the upper directories are also counted
		labeled_images_ = 0.0;
	}
	else
	{
		return;
	}

	// ********** if there is no valid image, don't try to load one **********
	if(number_of_files_==0)
	{
		return;
	}

	// ********** display the first valid image **********
	image_index_ = 0;
	bool loaded_image=false;
	do
	{
		// ------- try to load the next image and check if it is a valid image (the download script might download not displayable images) -------
		std::stringstream file_path;
		++image_index_;
		file_path << folder_path << image_index_ << ".jpg";
		displayImage(QString::fromStdString(file_path.str()));

		// ------- check if the loaded image has a size larger than 0 -------
		if(image_.cols!=0 && image_.rows!=0)
		{
			loaded_image = true;
		}
		else
		{
			// ------- set the next file index -------
			++image_index_;
		}
	}while(loaded_image==false);

	// ********** initially set the selected pixel to the center of the image **********
	selected_x_ = image_.cols/2;
	selected_y_ = image_.rows/2;
	selectImagePixels(selected_x_, selected_y_);
}

/*
 * Function that saves the currently segmented image and loads the next one that should be labeled.
 */
void labelizer::LabelizerPlugin::labelNextImage()
{
	// ********** update the progressbar **********
	++labeled_images_;
	if(number_of_files_!=0)
	{
		ui_.dwnld_progress_bar->setValue(100*labeled_images_/number_of_files_);
	}
	else
	{
		// ------- if no valid file has been downloaded, don't try to save/load any image, because there is none -------
		return;
	}

	// ********** store the currently segmented image **********
	std::string color_name = ui_.color_name_list->currentItem()->text().toUtf8().constData();
	std::string folder_path = ros::package::getPath("labelizer") + "/python/" + color_name + "/";
	std::stringstream image_path;
	image_path << folder_path << image_index_ << "_mask.jpg" ;
	cv::imwrite(image_path.str().c_str(), segmented_image_);

	// ********** load the next valid image **********
	bool loaded_image=false;
	do
	{
		// ------- try to load the next image and check if it is a valid image (the download script might download not displayable images) -------
		std::stringstream file_path;
		++image_index_;
		file_path << folder_path << image_index_ << ".jpg";
		displayImage(QString::fromStdString(file_path.str()));

		// ------- check if the loaded image has a size larger than 0 -------
		if(image_.cols!=0 && image_.rows!=0)
		{
			loaded_image = true;
		}
		else
		{
			// ------- set the next file index -------
			++image_index_;
		}
	}while(loaded_image==false);

	// ********** initially set the selected pixel to the center of the image **********
	selected_x_ = image_.cols/2;
	selected_y_ = image_.rows/2;
	selectImagePixels(selected_x_, selected_y_);
}

void labelizer::LabelizerPlugin::saveNegativeLabelImage()
{
	// ********** set the segmented image to be a black image **********
	segmented_image_ = cv::Mat(image_.rows, image_.cols, image_.type(), cv::Scalar(0));

	// ********** load the next valid image **********
	labelNextImage();
}

/*
 * Slot (callback function) that is called, whenever the lower bound indicator for the H-channel is changed.
 */
void labelizer::LabelizerPlugin::sigmaHLbChanged(const double sigma)
{
	// ********** set the new bound **********
	H_lower_ = sigma;

	// ********** search every pixel with the new bounds for the currently selected pixel **********
	selectImagePixels(selected_x_, selected_y_);
}

/*
 * Slot (callback function) that is called, whenever the upper bound indicator for the H-channel is changed.
 */
void labelizer::LabelizerPlugin::sigmaHUbChanged(const double sigma)
{
	// ********** set the new bound indicator **********
	H_upper_ = sigma;

	// ********** search every pixel with the new bounds for the currently selected pixel **********
	selectImagePixels(selected_x_, selected_y_);
}

/*
 * Slot (callback function) that is called, whenever the lower bound indicator for the S-channel is changed.
 */
void labelizer::LabelizerPlugin::sigmaSLbChanged(const double sigma)
{
	// ********** set the new bound indicator **********
	S_lower_ = sigma;

	// ********** search every pixel with the new bounds for the currently selected pixel **********
	selectImagePixels(selected_x_, selected_y_);
}

/*
 * Slot (callback function) that is called, whenever the upper bound indicator for the S-channel is changed.
 */
void labelizer::LabelizerPlugin::sigmaSUbChanged(const double sigma)
{
	// ********** set the new bound indicator **********
	S_upper_ = sigma;

	// ********** search every pixel with the new bounds for the currently selected pixel **********
	selectImagePixels(selected_x_, selected_y_);
}

/*
 * Slot (callback function) that is called, whenever the lower bound indicator for the V-channel is changed.
 */
void labelizer::LabelizerPlugin::sigmaVLbChanged(const double sigma)
{
	// ********** set the new bound indicator **********
	V_lower_ = sigma;

	// ********** search every pixel with the new bounds for the currently selected pixel **********
	selectImagePixels(selected_x_, selected_y_);
}

/*
 * Slot (callback function) that is called, whenever the upper bound indicator for the V-channel is changed.
 */
void labelizer::LabelizerPlugin::sigmaVUbChanged(const double sigma)
{
	// ********** set the new bound indicator **********
	V_upper_ = sigma;

	// ********** search every pixel with the new bounds for the currently selected pixel **********
	selectImagePixels(selected_x_, selected_y_);
}

/*
 * Function that is called, whenever a new pixel is selected.
 */
void labelizer::LabelizerPlugin::newPixelSelected(const double x_coordinate, const double y_coordinate)
{
	// ********** store the new pixel coordinates **********
	selected_x_ = x_coordinate;
	selected_y_ = y_coordinate;
}

/*
 * Function that gets the HSV-value at a given pixel and finds every pixel in the image, that has a HSV-value in the defined range
 * around the value of the selected pixel. It marks such pixels in a binary image as white and every other pixel as black and
 * shows the resulting mask to the user.
 */
void labelizer::LabelizerPlugin::selectImagePixels(const double x_coordinate, const double y_coordinate)
{
	// ********** ensure that the click was inside the image **********
	if(x_coordinate<0 || y_coordinate<0 || x_coordinate>image_.cols || y_coordinate>image_.rows)
	{
		return;
	}

	// ********** convert the image to the HSV-space **********
	cv::Mat hsv_image=image_.clone();
	cv::cvtColor(image_, hsv_image, CV_BGR2HSV);

	// ********** get the HSV-value of the clicked pixel **********
	cv::Vec3b pixel_value = hsv_image.at<cv::Vec3b>(y_coordinate, x_coordinate);

	// ********** get the lower and upper bound, depending on the defined sigma values **********
	cv::Scalar lower_values = cv::Scalar(pixel_value[0]-H_lower_, pixel_value[1]-S_lower_, pixel_value[2]-V_lower_);
	cv::Scalar upper_values = cv::Scalar(pixel_value[0]+H_upper_, pixel_value[1]+S_upper_, pixel_value[2]+V_upper_);

	// ********** go through the stored image and collect every pixel that is in the defined region around the clicked pixel-color **********
	cv::inRange(hsv_image, lower_values, upper_values, segmented_image_);

	// ********** display the segmented image **********
	QPixmap segmented_pic = QPixmap::fromImage(QImage(segmented_image_.data, segmented_image_.cols,
													  segmented_image_.rows, segmented_image_.step, QImage::Format_Indexed8));
	segmented_image_scene_->clear();
	segmented_image_scene_->addPixmap(segmented_pic);
	segmented_image_scene_->setSceneRect(segmented_pic.rect());
	ui_.segmented_frame->setScene(segmented_image_scene_);
}


/*
 * Export the labelizer plugin to allow rqt to load it.
 */
PLUGINLIB_EXPORT_CLASS(labelizer::LabelizerPlugin, rqt_gui_cpp::Plugin)
