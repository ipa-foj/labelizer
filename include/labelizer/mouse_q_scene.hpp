#pragma once

// rqt includes
#include <rqt_gui_cpp/plugin.h>

// Qt includes
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QGraphicsSceneMouseEvent>
#include <QEvent>

namespace labelizer
{

/**
 * @brief The MouseQScene class specifies a QGraphicsScene object that can show images in a GUI and track the mouse, when it is over the image.
 * It also can identify and return the coordinates of a mouse-click in an image.
 */
class MouseQScene : public QGraphicsScene
{
	Q_OBJECT

signals:

	/**
	 * @brief imageClicked is a signal that is emmitted, every time the image is clicked. It returns the x/y-coordinates of the click.
	 * @param x_coordinate: The x-coordinate of the click.
	 * @param y_coordinate: The y-coordinate of the click.
	 */
	void imageClicked(const double x_coordinate, const double y_coordinate);

protected:

	/**
	 * @brief x_ stores the x-coordinate of a mouse-click
	 */
	double x_;

	/**
	 * @brief y_ stores the y-coordinate of a mouse-click
	 */
	double y_;

public:
	/**
	 * @brief MouseQScene is the constructor setting up the widget this label belongs to.
	 * @param parent
	 */
	explicit MouseQScene(QObject* parent=0);

	/**
	 * @brief mousePressEvent overrides the general callback function, when a mouse button is pressed inside the defined label.
	 * @param ev: The object carrying the event, i.e. the mouse click.
	 */
	void mousePressEvent(QGraphicsSceneMouseEvent* ev);

	/**
	 * @brief getXCoordinate returns the stored x-coordinate of the last click.
	 * @return double, showing the x-coordinate.
	 */
	double getXCoordinate();

	/**
	 * @brief getYCoordinate returns the stored y-coordinate of the last click.
	 * @return double, showing the y-coordinate.
	 */
	double getYCoordinate();

};

} // end of namespace labelizer
