#pragma once

// rqt includes
#include <rqt_gui_cpp/plugin.h>

// form include (package specific)
#include <labelizer/ui_labelizer_form.h>

// Qt includes
#include <QWidget>
#include <QImage>

// OpenCV includes
#include <cv_bridge/cv_bridge.h>
#include <opencv/cv.h>

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

private:
	/**
	 * @brief created GUI that is stored inside of src/labelizer_form.ui
	 */
	Ui::LabelizerWidgetWindow ui_;

	/**
	 * @brief A pointer pointing to a QtWidget, in this special case the labelizer widget.
	 */
	QWidget* widget_;

	/**
	 * @brief displayImage displays the image that is stored in the given absolute path in the GUI.
	 * @param image_path: A QString that carries the absolute path to the image that should be displayed.
	 */
	void displayImage(const QString& image_path);
};

} // end of namespace labelizer
