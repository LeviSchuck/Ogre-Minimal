#include <iostream>
#include <OGRE/OgreRoot.h>
#include <OGRE/OgreRenderSystem.h>
#include <OGRE/OgreRenderWindow.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreColourValue.h>
#include <OGRE/OgreCamera.h>
#include <OGRE/OgreViewport.h>
#include <OGRE/OgreEntity.h>
#include <OGRE/OgreWindowEventUtilities.h>
#include <cmath>
#include <time.h>
using namespace Ogre;
using namespace std;
int main(int argc, char **argv) {
  cout << "Hello, world!" << endl;
  //variables we work with
  Root *oRoot;
  RenderSystem *renderSys;
  RenderWindow *oWindow;
  SceneManager *oScene;
  Camera *oCam;
  Viewport *vp;
  //Now the code
  oRoot = new Root("","","log.txt");
  Ogre::LogManager::getSingleton().setLogDetail(Ogre::LL_NORMAL);
  Ogre::LogManager::getSingleton().createLog("log.txt", false, true);
  //Load GL
  String plugins = "/usr/local/lib/";
  oRoot->loadPlugin(plugins + "RenderSystem_GL");
  const RenderSystemList &availRenderers = oRoot->getAvailableRenderers();
  renderSys = (availRenderers.front());
  oRoot->setRenderSystem(renderSys);
  oRoot->initialise(false);
  //Prepare window information
  const size_t width = 1280;
  const size_t height = 720;
  const bool fullscreen = false;
  const bool vsync = false;
  NameValuePairList params;
  params["vsync"] = "false";
  //Prepare window
  oWindow = oRoot->createRenderWindow(
    "Barebones Ogre test",
    width,
    height,
    fullscreen,
    &params
  );
  //Apply window settings
  oWindow->setActive(true);
  oWindow->setAutoUpdated(false);
  
  //Prepare Scene manager
  oScene = oRoot->createSceneManager(Ogre::ST_GENERIC, "root::_sceneMgr");
  oScene->setAmbientLight(ColourValue(0.1,0.1,0.1));
  //prepare camera and viewport
  oCam = oScene->createCamera("root::_camera");
  oCam->setNearClipDistance(0.1);
  oCam->setFarClipDistance(1000);
  oCam->setAutoAspectRatio(true);
  oCam->setFOVy(Ogre::Degree(90));
  vp = oWindow->addViewport(oCam);
  vp->setBackgroundColour(ColourValue(0.5, 0.5, 0.5, 0));
  vp->setClearEveryFrame(false);
  renderSys->_setViewport(vp);
  //init the render system properly
  renderSys->_initRenderTargets();
  //Prepare materials/resources
  //Ogre::MaterialManager::getSingleton().remove("BaseWhite");
  //Ogre::MaterialManager::getSingleton().remove("BaseWhiteNoLighting");
  Ogre::ResourceGroupManager &rgm = Ogre::ResourceGroupManager::getSingleton();
  rgm.addResourceLocation("data", "FileSystem", "data", true);
  rgm.initialiseResourceGroup("data");
  //set anisotropy
  Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(16);
  Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_ANISOTROPIC);
  //Add something to the scene
  Ogre::Entity* ogreHead = oScene->createEntity("Head", "ogrehead.mesh");
  Ogre::SceneNode* headNode = oScene->getRootSceneNode()->createChildSceneNode();
  headNode->attachObject(ogreHead);
  //final settings on camera
  oCam->setPosition(Vector3(10,15,50));
  oCam->lookAt(headNode->getPosition());
  
  // Set ambient light
  oScene->setAmbientLight(Ogre::ColourValue(0.5, 0.5, 0.5));
  
  // Create a light
  Ogre::Light* l = oScene->createLight("MainLight");
  l->setPosition(-20,30,20);
  //set up some sort of delay
  timespec delayish;
  delayish.tv_sec = 0;
  delayish.tv_nsec = 1000*40;
  //I guess its time to run
  float lastfps = -1;
  for(int x = 0; x > -1; x++)
  {
    WindowEventUtilities::messagePump();
    //do stuff
    /*timespec meh;
    nanosleep(&delayish,&meh);*/
    oCam->setPosition(Vector3(
      10+sin(
	(float)x*3.0/720.0*3.14159
      )*20.0,
      15,
      50+sin(
	(float)x*3.0/720.0*3.14159+1.95
      )*25.0
      )
    );
  oCam->lookAt(headNode->getPosition());
    //begin
    oRoot->_fireFrameStarted();
    
    oRoot->_fireFrameRenderingQueued();
    for (size_t i = 0; i < oWindow->getNumViewports(); ++i)
    {
      Ogre::Viewport *vp = oWindow->getViewport(i);
      renderSys->_setViewport(vp);
      renderSys->clearFrameBuffer(
	Ogre::FBT_COLOUR | Ogre::FBT_DEPTH | Ogre::FBT_STENCIL,
	vp->getBackgroundColour());
    }
    //render
    oScene->_renderScene(oCam,vp,false);
    
    //end
    oRoot->_fireFrameEnded();
    oWindow->swapBuffers(vsync);
    //Do FPS stuff
    if(lastfps != oWindow->getLastFPS()){
      cout << "FPS changed to " << (lastfps = oWindow->getLastFPS()) << endl;
    }
  }
  //Clean up
  delete oRoot;
  return 0;
}
