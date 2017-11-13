#pragma once

#include <rqt_gui_cpp/plugin.h>
#include <QWidget>
#include <labelizer/ui_labelizer_form.h>

namespace labelizer
{

class LabelizerPlugin : public rqt_gui_cpp::Plugin
{
	Q_OBJECT
public:
	LabelizerPlugin();
	virtual void initPlugin(qt_gui_cpp::PluginContext& context);
	virtual void shutdownPlugin();
	virtual void saveSettings(qt_gui_cpp::Settings& plugin_settings, qt_gui_cpp::Settings& instance_settings) const;
	virtual void restoreSettings(const qt_gui_cpp::Settings& plugin_settings, const qt_gui_cpp::Settings& instance_settings);

private:
	Ui::MainWindow ui_;
	QWidget* widget_;
};
} // end of namespace labelizer
