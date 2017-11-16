// class header include
#include <labelizer/mouse_q_scene.hpp>

// Plugin-export libraries --> needed when building a rqt-plugin in c++
#include <pluginlib/class_list_macros.h>

/*
 * Constructor setting up the parent widget of this object.
 */
labelizer::MouseQScene::MouseQScene(QObject* parent)
	: QGraphicsScene(parent)
{
	// initially set up the click-coordinates
	x_ = 0;
	y_ = 0;
}

/*
 * Function that handles the event that the left mouse-button is pressed inside an image.
 */
void labelizer::MouseQScene::mousePressEvent(QGraphicsSceneMouseEvent* ev)
{
	// ********** store the coordinates of the click **********
	x_ = ev->scenePos().x();
	y_ = ev->scenePos().y();

	// ********** emmit the signal that will publish the coordinates to other QObjects **********
	emit imageClicked(x_, y_);
}

/*
 * Function to return the stored x-coordinate of the last click.
 */
double labelizer::MouseQScene::getXCoordinate()
{
	return x_;
}

/*
 * Function to return the stored y-coordinate of the last click.
 */
double labelizer::MouseQScene::getYCoordinate()
{
	return y_;
}
