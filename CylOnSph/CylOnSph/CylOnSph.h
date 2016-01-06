#ifndef CYLONSPH_H
#define CYLONSPH_H

#include <osg/TexMat>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osg/ShapeDrawable>
#include <osg/Texture2D>
#include <osgGA/GUIEventHandler>
#include <osg/MatrixTransform>
#include <opencv2/core/core.hpp>


class TextureUpdateCallback : public osg::NodeCallback
{
private:
	osg::ref_ptr<osg::TexMat> texMat;

public:
	TextureUpdateCallback(osg::ref_ptr<osg::TexMat> tm) : texMat(tm){}
	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);
};

class CylOnSph
{
private:
	/*
	double viewWidth;
	double viewHeight;
	double xOffset;
	double yOffset;
	double radius;
	double sideDistance, centerDistance, depth;
	double camHorLoc, camVertLoc;
	double sideCull, centerCull, sideVPOffset, centerVPOffset;

	osg::ref_ptr<osgViewer::Viewer> viewer;

	const char* imageFileName;
	const char* displayFile;*/

	/*osg::ref_ptr<osg::Group> createShapes();
	void buildSphere(const double radius, const unsigned int rings, const unsigned int sectors, osg::ref_ptr<osg::Group> group);
	void setView();*/
	//void setup();

public:

	/*double angle;
	osg::ref_ptr<osg::Group> sphereGroup;
	osg::ref_ptr<osgViewer::Viewer> getViewer();
	void setVisible(bool v);*/

	/*FlyWorld(char *imgFile, char *settings, double w, double h, double x, double r) :
		imageFileName(imgFile), displayFile(settings), viewWidth(w), viewHeight(h), xOffset(x), yOffset(0), radius(r), sideDistance(r + 10.0), centerDistance(r + 10.0),
		camHorLoc(0), camVertLoc(r*-1.0), sideCull(0), centerCull(0), depth(0), sideVPOffset(0), centerVPOffset(0)
	{
		viewer = new osgViewer::Viewer();
		setup();
	}*/
};
#endif