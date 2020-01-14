#ifndef TRAINVIEW_H  
#define TRAINVIEW_H  
#include <QGLWidget> 
#include <QtGui>  
#include <QtOpenGL>  
#include <GL/GLU.h>
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib") 
#include "Utilities/ArcBallCam.H"
#include "Utilities/3DUtils.H"
#include "Track.H"
#include "Triangle.h"
#include "Square.h"
#include "Model.h"
#include "3DSLoader.h"

class AppMain;
class CTrack;

//#######################################################################
// TODO
// You might change the TrainView in order to add different objects to
// be drawn, or adjust the lighting, or ...
//#######################################################################


class TrainView : public QGLWidget, protected QOpenGLFunctions_4_3_Core
{  
	Q_OBJECT  
public:  
	explicit TrainView(QWidget *parent = 0);  
	~TrainView();  
public:
	enum class Spline {
		Linear,
		CardinalCubic,
		CubicBSpline
	};
public:
	// overrides of important window things
	//virtual int handle(int);
	virtual void paintGL();

	// all of the actual drawing happens in this routine
	// it has to be encapsulated, since we draw differently if
	// we're drawing shadows (no colors, for example)
	void drawStuff(bool doingShadows=false);
	void drawTracks();

	void drawTrain(float);

	// setup the projection - assuming that the projection stack has been
	// cleared for you
	void setProjection();

	// Reset the Arc ball control
	void resetArcball();

	// pick a point (for when the mouse goes down)
	void doPick(int mx, int my);

	void initializeGL();
	void initializeTexture();

	Vector3 getCardinalGMTmatrix(int, float, float, int);
	Vector3 getBSplineGMTmatrix(int, float, int);

	void drawTerrain();
	void calcMinMax();
	void faultAlgorithm(int iterations);

public:
	ArcBallCam		arcball;			// keep an ArcBall for the UI
	int				selectedCube;		// simple - just remember which cube is selected
	CTrack*			m_pTrack;			// The track of the entire scene

	float t_time;
	unsigned int DIVIDE_LINE;
	int trainSpeed;
	float tension;
	int carNum;

	int camera;
	int curve;
	int track;
	bool isrun;
	Triangle* triangle;
	Square* square;
	GLfloat ProjectionMatrex[16];
	GLfloat ModelViewMatrex[16];
	QVector<QOpenGLTexture*> Textures;
	
	Model* trainModel;
	Model* humanModel;
	C3DSLoader m_3DS;

	Pnt3f eyePosition;
	Pnt3f centerPosition;
	Pnt3f upPosition;

	// Terrain Generation
	bool quadStrip = false;
	float heightMap[1000][1000];
	int xSize = 500, zSize = 500;
	bool isGen = false;
	float minHeight, maxHeight;
	//determine quad or tirangle positions
	int pos[4][2] = { {0,0}, {0,-1}, {-1,-1}, {-1,0} };
};  
#endif // TRAINVIEW_H  