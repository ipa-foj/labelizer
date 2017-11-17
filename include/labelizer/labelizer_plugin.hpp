#pragma once

// rqt includes
#include <rqt_gui_cpp/plugin.h>

// package specific includes
#include <labelizer/mouse_q_scene.hpp>

// form include (package specific)
#include <labelizer/ui_labelizer_form.h>

// Qt includes
#include <QWidget>
#include <QImage>
#include <QMouseEvent>

// OpenCV includes
#include <cv_bridge/cv_bridge.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>

namespace labelizer
{

/**
 * @brief The LabelizerPlugin class implements a rqt-plugin that allows a user to select a region in an image that
 * best describes a shown color. This area is stored with the correpsonding label and can be used for training a
 * machine learning algorithm later.
 * @details The LabelizerPlugin shows different images to an user and asks to click anywhere in this image to show
 * where a specific color is located. This color will be shown in the top-right corner s.t. the user will have a
 * general idea of the color he is searching for.
 * @todo Documentation!!
 */
class LabelizerPlugin : public rqt_gui_cpp::Plugin
{
	Q_OBJECT
public:
	/**
	 * @brief The default constructor.
	 */
	LabelizerPlugin();

	/**
	 * @brief Function that initalizes the plugin and connects the signals and the slots, s.t. the functionality of
	 * the plugin is possible.
	 * @param context: Plugin context that carries the user-interface, which was created in labelizer_form.ui and
	 * allows an interaction of the user with the algorithms.
	 */
	virtual void initPlugin(qt_gui_cpp::PluginContext& context);

	/**
	 * @brief The shutdown function, which is needed for an rqt plugin, handles the shutdown for every object in this
	 * plugin.
	 */
	virtual void shutdownPlugin();

protected:
	/**
	 * @brief created GUI that is stored inside of src/labelizer_form.ui
	 */
	Ui::LabelizerWidgetWindow ui_;

	/**
	 * @brief A pointer pointing to a QtWidget, in this special case the labelizer widget.
	 */
	QWidget* widget_;

	/**
	 * @brief image_scene_ stores the scene that shows an image that should be labeled.
	 */
	MouseQScene* image_scene_;

	/**
	 * @brief segmented_image_scene_ stores the scene that shows the segmented image, based on the HSV-range.
	 */
	MouseQScene* segmented_image_scene_;

	/**
	 * @brief image_ stores the current displayed image as opencv-image, s.t. it can be handled easily.
	 */
	cv::Mat image_;

	/**
	 * @brief segmented_image_ stores the image mask, that shows every currently selected pixel (in white).
	 */
	cv::Mat segmented_image_;

	/**
	 * @brief H_lower_ allowed subtraction of the H-channel for searching for similar pixels.
	 */
	double H_lower_;

	/**
	 * @brief H_upper_ allowed addition of the H-channel for searching for similar pixels.
	 */
	double H_upper_;

	/**
	 * @brief S_lower_ allowed subtraction of the S-channel for searching for similar pixels.
	 */
	double S_lower_;

	/**
	 * @brief S_upper_ allowed addition of the S-channel for searching for similar pixels.
	 */
	double S_upper_;

	/**
	 * @brief V_lower_ allowed subtraction of the V-channel for searching for similar pixels.
	 */
	double V_lower_;

	/**
	 * @brief V_upper_ allowed addition of the V-channel for searching for similar pixels.
	 */
	double V_upper_;

	/**
	 * @brief selected_x_ stores the x-coordinate of the currently selected pixel.
	 */
	int selected_x_;

	/**
	 * @brief selected_y_ stores the y-coordinate of the currently selected pixel.
	 */
	int selected_y_;

	/**
	 * @brief number_of_files_ stores the number of valid files that have been downloaded for a color.
	 */
	double number_of_files_;

	/**
	 * @brief labeled_images_ stores the number of images that have already been labeled by the user.
	 */
	double labeled_images_;

	/**
	 * @brief image_index_ stores the index of the current image, that should be labeled.
	 */
	int image_index_;

	/**
	 * @brief displayImage displays the image that is stored in the given absolute path in the GUI.
	 * @param image_path: A QString that carries the absolute path to the image that should be displayed.
	 */
	void displayImage(const QString& image_path);

	/**
	 * @brief color_search_keyowrds_ is a vector that stores the keywords, which will be added to the color name
	 * for the google image search (e.g. food, cloth, object,...).
	 */
	std::vector<std::string> color_search_keyowrds_;

	/**
	 * The slots defined from here are used as callback-like functions, when e.g. pressing a button in the GUI.
	 */
protected slots:
	/**
	 * @brief downloadImages downloads images from google-images that come up when searching for the selected color.
	 */
	void downloadImages();

	/**
	 * @brief downloadAllImages downloads images from google-images that come up when searching for every given color.
	 */
	void downloadAllImages();

	/**
	 * @brief startLabeleingImages starts the labeling process for the currently selected color.
	 */
	void startLabeleingImages();

	/**
	 * @brief labelNextImage saves the currently labeled image and displays the next one.
	 */
	void labelNextImage();

	/**
	 * @brief saveNegativeLabelImage is called, whenever the user says that the to-be-searched color is not
	 * contained in the current image. The function then saves an empty mask, showing that no pixel belongs to
	 * the current color and loads the next valid image.
	 */
	void saveNegativeLabelImage();

	/**
	 * @brief sigmaHLbChanged is the slot (callback function) for setting the lower bound of the H-channel.
	 * @param sigma the new to-be-set bound.
	 */
	void sigmaHLbChanged(const double sigma);

	/**
	 * @brief sigmaHUbChanged is the slot (callback function) for setting the upper bound of the H-channel.
	 * @param sigma the new to-be-set bound.
	 */
	void sigmaHUbChanged(const double sigma);

	/**
	 * @brief sigmaSLbChanged is the slot (callback function) for setting the lower bound of the S-channel.
	 * @param sigma the new to-be-set bound.
	 */
	void sigmaSLbChanged(const double sigma);

	/**
	 * @brief sigmaSUbChanged is the slot (callback function) for setting the upper bound of the S-channel.
	 * @param sigma the new to-be-set bound.
	 */
	void sigmaSUbChanged(const double sigma);

	/**
	 * @brief sigmaVLbChanged is the slot (callback function) for setting the lower bound of the V-channel.
	 * @param sigma the new to-be-set bound.
	 */
	void sigmaVLbChanged(const double sigma);

	/**
	 * @brief sigmaVUbChanged is the slot (callback function) for setting the upper bound of the V-channel.
	 * @param sigma the new to-be-set bound.
	 */
	void sigmaVUbChanged(const double sigma);

	/**
	 * @brief newPixelSelected is the slot (callback function) that gets the x/y-coordinates of a pixel that is selected for the
	 * color search, i.e. the pixel that sets the original HSV-value for the range latter range selection.
	 * @param x_coordinate: double, storing the x-coordinate of the selected pixel
	 * @param y_coordinate: double, storing the y-coordinate of the selected pixel
	 */
	void newPixelSelected(const double x_coordinate, const double y_coordinate);

	/**
	 * @brief selectImagePixels is the slot (callback function) that gets the HSV-value of a current pixel and searches for pixels
	 * that are in the set range around this HSV-value. It marks such pixels in a binary image as white and every other pixel as
	 * black and shows the resulting mask to the user.
	 * @param x_coordinate: The x-coordinate of the current pixel.
	 * @param y_coordinate: The y-coordinate of the current pixel.
	 */
	void selectImagePixels(const double x_coordinate, const double y_coordinate);

};

} // end of namespace labelizer
