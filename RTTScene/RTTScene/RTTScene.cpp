
#include "RTTScene.h"


double scaleRate = 0.2;// 0.4;
double rS = 0.2;// -1.0 / 2.5; //rotation speed
double inc = 0.1;

float screenWidth = 1536;
float screenHeight = 864;

osg::MatrixTransform * transform;
osg::MatrixTransform * transform2;






void TextureUpdateCallback::operator()(osg::Node*, osg::NodeVisitor* nv)
{
	if (!texMat)
	{
		return;
	}

	if (nv->getFrameStamp())
	{

		double currTime = nv->getFrameStamp()->getSimulationTime();
		float s = currTime*rS;
		float r = currTime*scaleRate;

		texMat->setMatrix(osg::Matrix::translate(s, 0.0f, 0.0f));
		//texMat->setMatrix(osg::Matrix::scale(1.0/(1.0+r),1.0,1.0)*osg::Matrix::translate(((1.0-(1.0/(1.0+r)))-1.0)/2.0+0.5,0.0f,0.0f));
		//texMat->setMatrix(osg::Matrix::scale(1.0-r,1.0,1.0)*osg::Matrix::translate((r-1.0)/2.0+0.5,0.0f,0.0f));
	}
}



osg::ref_ptr<osg::Group> createRTTScene(float width, float height) //scene that will be rendered to texture
{
	osg::TessellationHints* sceneHints = new osg::TessellationHints;
	sceneHints->setDetailRatio(5.0f);
	/*sceneHints->setCreateBackFace(true);
	sceneHints->setCreateFrontFace(true);
	sceneHints->setCreateNormals(true);
	sceneHints->setCreateTop(true);
	sceneHints->setCreateBottom(true);*/
	//osg::Sphere* sph = new osg::Sphere(osg::Vec3(0.0f, 0.0f, 0.0f), 0.5f);
	//osg::ShapeDrawable* sceneSphere = new osg::ShapeDrawable(sph, sceneHints);

	osg::ref_ptr<osg::Geometry> sceneGeometry;
	/*sceneGeometry = osg::createTexturedQuadGeometry(osg::Vec3(),
	osg::Vec3(width, 0.0, 0.0),
	osg::Vec3(0.0, width, 0.0));*/


	//sceneGeometry = buildSphere(width / 2.0, 20, 20);

	osg::ref_ptr<osg::Sphere>  sceneSphere = new osg::Sphere(osg::Vec3d(0, 0, 0), width / 4.0);

	sceneHints->setCreateBackFace(true);
	sceneHints->setCreateFrontFace(true);
	sceneHints->setCreateNormals(true);
	sceneHints->setCreateTop(true);
	sceneHints->setCreateBottom(true);

	osg::ref_ptr<osg::ShapeDrawable> sceneDrawable = new osg::ShapeDrawable(sceneSphere, sceneHints);


	osg::StateSet* sceneStateset = new osg::StateSet();
	sceneStateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	osg::Image* image = new osg::Image();
	image = osgDB::readImageFile("../../images//numberline.bmp");

	//if (image)
	//{
	osg::Texture2D* sceneTexture = new osg::Texture2D(image);
	sceneTexture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
	//	sceneTexture->setWrap(osg::Texture::WRAP_R, osg::Texture::REPEAT);
	//	sceneTexture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
	sceneStateset->setTextureAttributeAndModes(0, sceneTexture, osg::StateAttribute::ON);
	osg::TexMat* texmat = new osg::TexMat;
	sceneStateset->setTextureAttributeAndModes(0, texmat, osg::StateAttribute::ON);

	sceneTexture->setUnRefImageDataAfterApply(true);
	//}

	osg::Geode* sceneGeode = new osg::Geode();
	sceneGeode->setStateSet(sceneStateset);
	sceneGeode->setCullingActive(false);
	//	sceneGeode->addDrawable(sceneGeometry.get());
	sceneGeode->addDrawable(sceneDrawable.get());
	//sceneGeode->setUpdateCallback(new TextureUpdateCallback(texmat));





	//	transform = new osg::MatrixTransform;
	//	transform->setMatrix(osg::Matrix());
	//	transform->addChild(sceneGeode2);
	osg::Group * sceneGroup = new osg::Group;
	sceneGroup->addChild(sceneGeode);
	//sceneGroup->addChild(transform);

	return sceneGroup;
}


osg::ref_ptr<osg::Camera> createTextureCamera(float width, float height, osg::ref_ptr<osg::Group> scene)
{
	osg::ref_ptr<osg::Camera> textureCamera = new osg::Camera;
	//textureCamera->setProjectionMatrix(osg::Matrix::ortho2D(0, width, 0, width));
	textureCamera->setViewMatrixAsLookAt(osg::Vec3d(0, width / 1.0, 0), osg::Vec3d(0, 0, 0), osg::Vec3d(0, 0, 1));
	//textureCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	textureCamera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	textureCamera->setViewport(0, 218, screenWidth, screenHeight);
	// Frame buffer objects are the best option
	//textureCamera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
	// We need to render to the texture BEFORE we render to the screen
	//textureCamera->setRenderOrder(osg::Camera::PRE_RENDER);
	// The camera will render into the texture that we created earlier
	//textureCamera->attach(osg::Camera::COLOR_BUFFER, renderTexture);
	// Add whatever children you want drawn to the texture
	textureCamera->addChild(scene);

	osg::Vec4 green = osg::Vec4(0, 1.0, 0.0, 1);
	textureCamera->setClearColor(green);

	return textureCamera;
}









osg::ref_ptr<osgViewer::Viewer> createViewer(osg::ref_ptr<osg::Camera> textureCamera)
{
	osg::Group* root = new osg::Group;

	root->addChild(textureCamera.get());




	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;

	osg::Vec4 backgroundColor = osg::Vec4(0, 0, 0, 1);
	viewer->getCamera()->setClearColor(backgroundColor);

	//viewer.getCamera()->setViewMatrixAsLookAt(osg::Vec3d(0, 20, 0), osg::Vec3d(0, 0, 0), up);
	//	viewer.getCamera()->setProjectionMatrixAsPerspective(25.0, 1920.0 / 1200.0, 0.1, 5);

	viewer->setSceneData(root);

	return viewer;
}

osg::ref_ptr<osgViewer::Viewer> setup()
{
	float width = screenWidth;
	float height = screenHeight;

	osg::ref_ptr<osg::Group> scene = createRTTScene(width, height);
	osg::ref_ptr<osg::Camera> textureCamera = createTextureCamera(width, height, scene);
	osg::ref_ptr<osgViewer::Viewer> viewer = createViewer(textureCamera);

	return viewer;
}


void run(osg::ref_ptr<osgViewer::Viewer> viewer)
{
	while (!viewer->done())
	{
		//transform->setMatrix(transform->getMatrix()*osg::Matrix::translate(5.0f, 0.0f, 0.0f));
	//	transform2->setMatrix(transform2->getMatrix()*osg::Matrix::rotate(0.02, osg::Vec3d(1.0, 0.0, 0.0)));
		viewer->frame();
	}
}



int main(int argc, char **argv)
{
	osg::ref_ptr<osgViewer::Viewer> viewer = setup();
	run(viewer);

	return 0;
}

