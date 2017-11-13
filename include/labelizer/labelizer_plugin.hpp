#pragma once

// rqt includes
#include <rqt_gui_cpp/plugin.h>
// form include (package specific)
#include <labelizer/ui_labelizer_form.h>
// Qt includes
#include <QWidget>
#include <QImage>

namespace labelizer
{

class LabelizerPlugin : public rqt_gui_cpp::Plugin
{
	Q_OBJECT
public:
	/*
	 * default constructor
	 */
	LabelizerPlugin();

	/*
	 * initializer for the plugin
	 */
	virtual void initPlugin(qt_gui_cpp::PluginContext& context);

	/*
	 * shutdown function
	 */
	virtual void shutdownPlugin();

private:
	/*
	 * created GUI that is stored inside of src/labelizer_form.ui
	 */
	Ui::LabelizerWidgetWindow ui_;

	/*
	 * pointer pointing to a general QtWidget, in this special case the labelizer widget
	 */
	QWidget* widget_;
};

} // end of namespace labelizer
