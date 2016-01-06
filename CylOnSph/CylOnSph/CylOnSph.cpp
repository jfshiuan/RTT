// CylOnSph.cpp : Defines the entry point for the console application.
//
#include "CylOnSph.h"


double scaleRate = 0.2;// 0.4;
double rS = 0.2;// -1.0 / 2.5; //rotation speed
double inc = 0.1;

float screenWidth = 1200;
float screenHeight = 900;

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
		/*sceneHints->setDetailRatio(0.8f);
		sceneHints->setCreateBackFace(true);
		sceneHints->setCreateFrontFace(true);
		sceneHints->setCreateNormals(true);
		sceneHints->setCreateTop(true);
		sceneHints->setCreateBottom(true);*/
		//osg::Sphere* sph = new osg::Sphere(osg::Vec3(0.0f, 0.0f, 0.0f), 0.5f);
		//osg::ShapeDrawable* sceneSphere = new osg::ShapeDrawable(sph, sceneHints);

		osg::ref_ptr<osg::Geometry> sceneRect;
		sceneRect = osg::createTexturedQuadGeometry(osg::Vec3(),
			osg::Vec3(width, 0.0, 0.0),
			osg::Vec3(0.0, width, 0.0));







		osg::StateSet* sceneStateset = new osg::StateSet();
		sceneStateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
		osg::Image* image = new osg::Image();
		image = osgDB::readImageFile("images//numberline1.bmp");

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
		sceneGeode->addDrawable(sceneRect.get());
		sceneGeode->setUpdateCallback(new TextureUpdateCallback(texmat));




		osg::ref_ptr<osg::Geometry> bar;
		bar = osg::createTexturedQuadGeometry(osg::Vec3(),
			osg::Vec3(width / 5.0, 0.0, 0.0),
			osg::Vec3(0.0, width, 0.0));
		osg::Geode* sceneGeode2 = new osg::Geode();
		sceneGeode2->addDrawable(bar.get());


		transform = new osg::MatrixTransform;
		transform->setMatrix(osg::Matrix());
		transform->addChild(sceneGeode2);
		osg::Group * sceneGroup = new osg::Group;
		sceneGroup->addChild(sceneGeode);
		//sceneGroup->addChild(transform);

		return sceneGroup;
	}

	osg::ref_ptr<osg::Texture2D> createRenderTexture(float width, float height)
	{
		// Create the texture to render to
		osg::ref_ptr<osg::Texture2D> renderTexture = new osg::Texture2D;
		renderTexture->setTextureSize(width, height);
		renderTexture->setInternalFormat(GL_RGBA);
		renderTexture->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
		renderTexture->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);
		renderTexture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
		renderTexture->setWrap(osg::Texture::WRAP_R, osg::Texture::REPEAT);
		renderTexture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);

		return renderTexture;
	}

	osg::ref_ptr<osg::Camera> createTextureCamera(float width, float height,  osg::ref_ptr<osg::Group> scene, osg::ref_ptr<osg::Texture2D> renderTexture)
	{
		osg::ref_ptr<osg::Camera> textureCamera = new osg::Camera;
		textureCamera->setProjectionMatrix(osg::Matrix::ortho2D(0, width, 0, width));
		textureCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
		textureCamera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		textureCamera->setViewport(0, 0, width, height);
		// Frame buffer objects are the best option
		textureCamera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
		// We need to render to the texture BEFORE we render to the screen
		textureCamera->setRenderOrder(osg::Camera::PRE_RENDER);
		// The camera will render into the texture that we created earlier
		textureCamera->attach(osg::Camera::COLOR_BUFFER, renderTexture);
		// Add whatever children you want drawn to the texture
		textureCamera->addChild(scene);

		osg::Vec4 green = osg::Vec4(0, 1.0, 0.0, 1);
		textureCamera->setClearColor(green);

		return textureCamera;
	}

	osg::ref_ptr<osg::Geode> createScreenGeode(float width, float height, osg::ref_ptr<osg::Texture2D> renderTexture)
	{
		/*
		osg::ref_ptr<osg::Geometry> renderToGeometry;
		renderToGeometry = osg::createTexturedQuadGeometry(osg::Vec3(),
		osg::Vec3(width, 0.0, 0.0),
		osg::Vec3(0.0, height, 0.0));
		osg::ref_ptr<osg::Geode> renderToGeode = new osg::Geode;



		renderToGeode->addDrawable(renderToGeometry.get());

		osg::StateSet *renderToState = renderToGeode->getOrCreateStateSet();
		renderToState->setTextureAttributeAndModes(0, renderTexture, osg::StateAttribute::ON);
		renderToState->setMode(GL_LIGHTING, osg::StateAttribute::OFF);*/





		//SphereGeode
		osg::ref_ptr<osg::Geode> sphereGeode = new osg::Geode;
		osg::TessellationHints* hints2 = new osg::TessellationHints;
		osg::Sphere* sph2 = new osg::Sphere(osg::Vec3(width / 4.0f, width / 4.0f, 0.0f), width / 4.0f);
		osg::ShapeDrawable* sphere2 = new osg::ShapeDrawable(sph2, hints2);
		sphereGeode->addDrawable(sphere2);
		osg::StateSet *sphereState = sphereGeode->getOrCreateStateSet();
		sphereState->setTextureAttributeAndModes(0, renderTexture, osg::StateAttribute::ON);
		sphereState->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

		return sphereGeode;
	}

	osg::ref_ptr<osg::Camera> createOrthoCamera(float width, float height, osg::ref_ptr<osg::Geode> screenGeode)
	{
		osg::ref_ptr<osg::Camera> orthoCamera = new osg::Camera;

		// We don't want to apply perspective, just overlay using orthographic
		orthoCamera->setProjectionMatrix(osg::Matrix::ortho2D(-width / 2.0, width, -height / 2.0, height));

		orthoCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
		orthoCamera->setViewMatrix(osg::Matrix::identity());
		
		osg::Vec4 red = osg::Vec4(1.0, 0.0, 0.0, 1);
	

		osg::Vec3d up = osg::Vec3d(0, 0, 1); //up vector
		orthoCamera->setClearColor(red);

		//orthoCamera->setViewMatrixAsLookAt(osg::Vec3d(0, 0, 0), osg::Vec3d(0, 0, 0), up);
		//orthoCamera->setProjectionMatrixAsPerspective(25.0, 1920.0 / 1200.0, 0.1, 5);

		// Choose a good spot on the screen to overlay the quad
		float xPos = 100;
		float yPos = 300;

		orthoCamera->setViewport(xPos, yPos, width, height);


		// Make sure to render this after rendering the scene
		// in order to overlay the quad on top
		orthoCamera->setRenderOrder(osg::Camera::POST_RENDER);


		// Render only the quad
		transform2 = new osg::MatrixTransform;
		transform2->setMatrix(osg::Matrix());
		transform2->addChild(screenGeode.get());

		orthoCamera->addChild(transform2);

		return orthoCamera;
	}


	osg::ref_ptr<osgViewer::Viewer> createViewer(osg::ref_ptr<osg::Camera> textureCamera, osg::ref_ptr<osg::Camera> orthoCamera)
	{
		osg::Group* root = new osg::Group;

		root->addChild(textureCamera.get());
		root->addChild(orthoCamera.get());




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
		float width = screenWidth * 0.85f;
		float height = screenHeight * 0.75f;

		osg::ref_ptr<osg::Group> scene = createRTTScene(width, height);
		osg::ref_ptr<osg::Texture2D> renderTexture = createRenderTexture(width, height);
		osg::ref_ptr<osg::Camera> textureCamera = createTextureCamera(width, height, scene, renderTexture);
		osg::ref_ptr<osg::Geode> screenGeode = createScreenGeode(width, height, renderTexture);
		osg::ref_ptr<osg::Camera> orthoCamera = createOrthoCamera(width, height, screenGeode);
		osg::ref_ptr<osgViewer::Viewer> viewer = createViewer(textureCamera, orthoCamera);

		return viewer;
	}


	void run(osg::ref_ptr<osgViewer::Viewer> viewer)
	{
		while (!viewer->done())
		{
			//transform->setMatrix(transform->getMatrix()*osg::Matrix::translate(5.0f, 0.0f, 0.0f));
			transform2->setMatrix(transform2->getMatrix()*osg::Matrix::rotate(0.02, osg::Vec3d(1.0, 0.0, 0.0)));
			viewer->frame();
		}
	}



int main(int argc, char **argv)
{
	osg::ref_ptr<osgViewer::Viewer> viewer = setup();
	run(viewer);
	
	return 0;
}

