#include "TrainView.h"  
#include "ParticleAPI.h"
#include <QtMultimedia/QMediaPlayer>
#include <qmatrix.h>
#include <SOIL/SOIL.h>

TrainView::TrainView(QWidget* parent) :
	QGLWidget(parent)
{
	resetArcball();
	// QMediaPlayer testing (no restart version)
	//QMediaPlayer* player;
	//player = new QMediaPlayer;
	//player->setMedia(QUrl(QUrl::fromLocalFile("..\\..\\Roller Coaster\\x64\\Debug\\Stereopony-Tsukiakari No Michishirube.mp3")));
	//player->setVolume(50);
	//player->play();
	this->DIVIDE_LINE = 15;
	this->t_time = 0;
	this->trainSpeed = 30;
	this->tension = 0.7;
	this->carNum = 0;
	this->eyePosition = Pnt3f(50, 5, 0);
	this->centerPosition = Pnt3f(0, 5, 50);
	this->upPosition = Pnt3f(50, 10, 0);
}
TrainView::~TrainView()
{}
void TrainView::initializeGL()
{
	initializeOpenGLFunctions();
	//Create a triangle object
	triangle = new Triangle();
	//Initialize the triangle object
	triangle->Init();
	//Create a square object
	square = new Square();
	//Initialize the square object
	square->Init();
	//Initialize texture 
	skyBox = new CubeMap();
	skyBox->Init();
	initializeTexture();

	// Create a train object
	//this->trainModel = new Model("../../Models/Sci_fi_Train.obj", 30, Point3d(0.0, 5.0, 0.0));
	this->trainModel = new Model("../../Models/train2.obj", 30, Point3d(0.0, 5.0, 0.0));
	this->humanModel = new Model("../../Models/human.obj", 30, Point3d(0.0, 5.0, 0.0));
	//m_3DS.Init("../../Models/Sci_fi_Train.3ds");

	resetHeightMap();
}
void TrainView::initializeTexture()
{
	//Load and create a texture for square;'stexture
	QOpenGLTexture* texture = new QOpenGLTexture(QImage("./Textures/Tupi.bmp"));
	Textures.push_back(texture);
	texture = new QOpenGLTexture(QImage("../../Textures/Sci fi Train color.png"));
	Textures.push_back(texture);
	texture = new QOpenGLTexture(QImage("../../Textures/space_skybox_up.jpg"));
	Textures.push_back(texture);
}
void TrainView::resetArcball()
//========================================================================
{
	// Set up the camera to look at the world
	// these parameters might seem magical, and they kindof are
	// a little trial and error goes a long way
	arcball.setup(this, 40, 250, .2f, .4f, 0);
}

void TrainView::calcMinMax()
{
	maxHeight = 0;
	minHeight = heightMap[0][0];
	for (int x = 1; x < xSize; x++)
	{
		for (int z = 1; z < zSize; z++)
		{
			if (heightMap[x][z] > maxHeight)
			{
				maxHeight = heightMap[x][z];
			}
			if (heightMap[x][z] < minHeight)
			{
				minHeight = heightMap[x][z];
			}
		}
	}
}

void TrainView::resetHeightMap()
{
	memset(heightMap, 0, sizeof(heightMap[0][0]) * 1000 * 1000);
}

void TrainView::faultAlgorithm(int iterations)
{
	for (int i = 0; i < iterations; i++) {
		float v = static_cast <float> (rand());
		float a = sin(v);
		float b = cos(v);
		float d = sqrt(pow(xSize, 2) + pow(zSize, 2));
		float c = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX))* d - d / 2;
		for (int x = 0; x < xSize; x++) {
			for (int z = 0; z < zSize; z++) {
				if (a * x + b * z - c > 0) {
					heightMap[x][z] += 1;
				}
				else {
					heightMap[x][z] -= 1;
				}
			}
		}
	}
	calcMinMax();
}

void TrainView::drawTerrain() {
	int newX, newZ;
	for (int x = 0; x < xSize; x++)
	{
		for (int z = 0; z < zSize; z++)
		{
			int storeX1 = x - pos[2][0];  //store vars used to get last two verts
			int storeZ1 = z - pos[2][1];
			int storeX2 = x - pos[3][0];
			int storeZ2 = z - pos[3][1];
			if (quadStrip) {
				glBegin(GL_QUAD_STRIP);
			}
			else {
				glBegin(GL_TRIANGLE_STRIP);
			}
			for (int i = 3; i >= 0; i--)
			{
				if (i == 0) {
					newX = x - pos[i][0];
					newZ = z - pos[i][1];
					storeX1 = newX;
					storeZ1 = newZ;
				}
				else if (i == 1) {
					newX = x - pos[i][0];
					newZ = z - pos[i][1];
					storeX2 = newX;
					storeZ2 = newZ;
				}
				else if (i == 2) {
					newX = storeX2;
					newZ = storeZ2;
				}
				else {
					newX = storeX1;
					newZ = storeZ1;
				}
				float newY = heightMap[newX][newZ];
				float gradient = (newY - minHeight) / (maxHeight - minHeight);
				glColor3f(0, gradient, 0); //green
				glVertex3f(newX - 250, newY - 50, newZ - 250); //draw
			}
			glEnd();
		}
	}
}

GLuint ReadTexture(const char* texturePath) {
	GLuint texID;
	QImage textureImg;// = QImage(texturePath);
	textureImg.load(texturePath);
	QSize sss = textureImg.size();
	if (textureImg.isNull())
		qDebug() << "Error when loading texture img";
	textureImg = QGLWidget::convertToGLFormat(textureImg);

	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureImg.width(), textureImg.height(), 0, GL_RGBA,
		GL_UNSIGNED_BYTE, textureImg.bits());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);

	return texID;
}

void TrainView::drawSkyBox(int distance)
{
	vector<GLuint> skyboxVect;
	skyboxVect.push_back(ReadTexture("../../Textures/space_skybox_front.jpg"));
	skyboxVect.push_back(ReadTexture("../../Textures/space_skybox_left.jpg"));
	skyboxVect.push_back(ReadTexture("../../Textures/space_skybox_back.jpg"));
	skyboxVect.push_back(ReadTexture("../../Textures/space_skybox_right.jpg"));
	skyboxVect.push_back(ReadTexture("../../Textures/space_skybox_up.jpg"));
	skyboxVect.push_back(ReadTexture("../../Textures/space_skybox_down.jpg"));

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_BLEND);
	
	const int dis = distance;
	// Just in case we set all vertices to white.
	glColor4f(1, 1, 1, 1);

	// Render the front quad
	glBindTexture(GL_TEXTURE_2D, skyboxVect[0]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(dis, -dis, -dis);
	glTexCoord2f(1, 0); glVertex3f(-dis, -dis, -dis);
	glTexCoord2f(1, 1); glVertex3f(-dis, dis, -dis);
	glTexCoord2f(0, 1); glVertex3f(dis, dis, -dis);
	glEnd();

	// Render the left quad
	glBindTexture(GL_TEXTURE_2D, skyboxVect[1]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(dis, -dis, dis);
	glTexCoord2f(1, 0); glVertex3f(dis, -dis, -dis);
	glTexCoord2f(1, 1); glVertex3f(dis, dis, -dis);
	glTexCoord2f(0, 1); glVertex3f(dis, dis, dis);
	glEnd();

	// Render the back quad
	glBindTexture(GL_TEXTURE_2D, skyboxVect[2]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(-dis, -dis, dis);
	glTexCoord2f(1, 0); glVertex3f(dis, -dis, dis);
	glTexCoord2f(1, 1); glVertex3f(dis, dis, dis);
	glTexCoord2f(0, 1); glVertex3f(-dis, dis, dis);

	glEnd();

	// Render the right quad
	glBindTexture(GL_TEXTURE_2D, skyboxVect[3]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(-dis, -dis, -dis);
	glTexCoord2f(1, 0); glVertex3f(-dis, -dis, dis);
	glTexCoord2f(1, 1); glVertex3f(-dis, dis, dis);
	glTexCoord2f(0, 1); glVertex3f(-dis, dis, -dis);
	glEnd();

	// Render the top quad
	glBindTexture(GL_TEXTURE_2D, skyboxVect[4]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1); glVertex3f(-dis, dis, -dis);
	glTexCoord2f(0, 0); glVertex3f(-dis, dis, dis);
	glTexCoord2f(1, 0); glVertex3f(dis, dis, dis);
	glTexCoord2f(1, 1); glVertex3f(dis, dis, -dis);
	glEnd();

	// Render the bottom quad
	glBindTexture(GL_TEXTURE_2D, skyboxVect[5]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(-dis, -dis, -dis);
	glTexCoord2f(0, 1); glVertex3f(-dis, -dis, dis);
	glTexCoord2f(1, 1); glVertex3f(dis, -dis, dis);
	glTexCoord2f(1, 0); glVertex3f(dis, -dis, -dis);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void TrainView::paintGL()
{

	//*********************************************************************
	//
	// * Set up basic opengl informaiton
	//
	//**********************************************************************
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	// Set up the view port
	glViewport(0, 0, width(), height());
	// clear the window, be sure to clear the Z-Buffer too
	glClearColor(0, 0, 0.3f, 0);

	// we need to clear out the stencil buffer since we'll use
	// it for shadows
	glClearStencil(0);
	glEnable(GL_DEPTH);

	// Blayne prefers GL_DIFFUSE
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	// prepare for projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	setProjection();		// put the code to set up matrices here

	//######################################################################
	// TODO: 
	// you might want to set the lighting up differently. if you do, 
	// we need to set up the lights AFTER setting up the projection
	//######################################################################
	// enable the lighting
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// top view only needs one light
	if (this->camera == 1) {
		glDisable(GL_LIGHT1);
		glDisable(GL_LIGHT2);
	}
	else {
		glEnable(GL_LIGHT1);
		glEnable(GL_LIGHT2);
	}

	//*********************************************************************
	//
	// * set the light parameters
	//
	//**********************************************************************
	GLfloat lightPosition1[] = { 0,1,1,0 }; // {50, 200.0, 50, 1.0};
	GLfloat lightPosition2[] = { 1, 0, 0, 0 };
	GLfloat lightPosition3[] = { 0, -1, 0, 0 };
	GLfloat yellowLight[] = { 0.5f, 0.5f, .1f, 1.0 };
	GLfloat whiteLight[] = { 1.0f, 1.0f, 1.0f, 1.0 };
	GLfloat blueLight[] = { .1f,.1f,.3f,1.0 };
	GLfloat grayLight[] = { .3f, .3f, .3f, 1.0 };

	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition1);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteLight);
	glLightfv(GL_LIGHT0, GL_AMBIENT, grayLight);

	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition2);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, yellowLight);

	glLightfv(GL_LIGHT2, GL_POSITION, lightPosition3);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, blueLight);

	//*********************************************************************
	// now draw the ground plane
	//*********************************************************************
	if (!isGen) {
		faultAlgorithm(400);
		isGen = true;
	}
	//setupFloor();
	// enable blending
	glEnable(GL_BLEND);
	// enable read-only depth buffer
	glDepthMask(GL_FALSE);
	// set the blend function to what we use for transparency
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	// set back to normal depth buffer mode (writable)
	glDepthMask(GL_TRUE);
	// disable blending
	glDisable(GL_BLEND);
	glFlush();
	glDisable(GL_LIGHTING);
	drawSkyBox(400);
	drawTerrain();
	//drawFloor(200, 10);


	//*********************************************************************
	// now draw the object and we need to do it twice
	// once for real, and then once for shadows
	//*********************************************************************
	glEnable(GL_LIGHTING);
	setupObjects();

	drawStuff();

	// this time drawing is for shadows (except for top view)
	if (this->camera != 1) {
		setupShadows();
		drawStuff(true);
		unsetupShadows();
	}

	//Get modelview matrix
	glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);
	//Get projection matrix
	glGetFloatv(GL_PROJECTION_MATRIX, ProjectionMatrex);

	// Sky Box
	//this->skyBox->Begin();
	//glActiveTexture(GL_TEXTURE0);
	//GLuint skyBoxTextureID;
	//glGenTextures(1, &skyBoxTextureID);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, skyBoxTextureID);
	//int skyWidth, skyHeight;
	//unsigned char* skyBox;
	//skyBox = SOIL_load_image("..\\..\\Textures\\space_skybox_up.jpg", &skyWidth, &skyHeight, 0, SOIL_LOAD_RGB);
	//glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, skyWidth, skyHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, skyBox);
	//skyBox = SOIL_load_image("..\\..\\Textures\\space_skybox_down.jpg", &skyWidth, &skyHeight, 0, SOIL_LOAD_RGB);
	//glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, skyWidth, skyHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, skyBox);
	//skyBox = SOIL_load_image("..\\..\\Textures\\space_skybox_left.jpg", &skyWidth, &skyHeight, 0, SOIL_LOAD_RGB);
	//glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, skyWidth, skyHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, skyBox);
	//skyBox = SOIL_load_image("..\\..\\Textures\\space_skybox_right.jpg", &skyWidth, &skyHeight, 0, SOIL_LOAD_RGB);
	//glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, skyWidth, skyHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, skyBox);
	//skyBox = SOIL_load_image("..\\..\\Textures\\space_skybox_front.jpg", &skyWidth, &skyHeight, 0, SOIL_LOAD_RGB);
	//glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, skyWidth, skyHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, skyBox);
	//skyBox = SOIL_load_image("..\\..\\Textures\\space_skybox_back.jpg", &skyWidth, &skyHeight, 0, SOIL_LOAD_RGB);
	//glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, skyWidth, skyHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, skyBox);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	//
	//glDepthMask(GL_FALSE);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, skyBoxTextureID);
	//this->skyBox->shaderProgram->setUniformValue("skybox", 0);
	//this->skyBox->Paint(ProjectionMatrex, ModelViewMatrex);
	//glDepthMask(GL_TRUE);
	//this->skyBox->End();

	//Call triangle's render function, pass ModelViewMatrex and ProjectionMatrex
	triangle->Paint(ProjectionMatrex, ModelViewMatrex);

	//we manage textures by Trainview class, so we modify square's render function
	square->Begin();
	//Active Texture
	glActiveTexture(GL_TEXTURE0);
	//Bind square's texture
	Textures[0]->bind();
	//pass texture to shader
	square->shaderProgram->setUniformValue("Texture", 0);
	//Call square's render function, pass ModelViewMatrex and ProjectionMatrex
	square->Paint(ProjectionMatrex, ModelViewMatrex);
	square->End();

	// Model.h draw *.obj file
	//Model* mTest = new Model("..\\..\\Roller Coaster\\x64\\Debug\\arrow.obj", 100, Point3d(0.0, 0.0, 0.0));
	//mTest->render(false, false);

	// draw *.3ds file failed
	//this->m_3DS.Draw();

	// Particle testing
	if (this->isrun) {
		ParticleAPI::ProcessParticles();
		ParticleAPI::DrawParticles();
	}
}

//************************************************************************
//
// * This sets up both the Projection and the ModelView matrices
//   HOWEVER: it doesn't clear the projection first (the caller handles
//   that) - its important for picking
//========================================================================
void TrainView::
setProjection()
//========================================================================
{
	// Compute the aspect ratio (we'll need it)
	float aspect = static_cast<float>(width()) / static_cast<float>(height());

	// Check whether we use the world camp
	if (this->camera == 0) {
		arcball.setProjection(false);
		update();
		// Or we use the top cam
	}
	else if (this->camera == 1) {
		float wi, he;
		if (aspect >= 1) {
			wi = 110;
			he = wi / aspect;
		}
		else {
			he = 110;
			wi = he * aspect;
		}

		// Set up the top camera drop mode to be orthogonal and set
		// up proper projection matrix
		glMatrixMode(GL_PROJECTION);
		glOrtho(-wi, wi, -he, he, 200, -200);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glRotatef(-90, 1, 0, 0);
		update();
	}
	// Or do the train view or other view here
	//####################################################################
	// TODO: 
	// put code for train view projection here!	
	//####################################################################
	else {
		glMatrixMode(GL_PROJECTION);
		gluPerspective(120, 1, 1, 200); glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		this->centerPosition.normalize();
		this->upPosition.normalize();
		gluLookAt(this->eyePosition.x, this->eyePosition.y + 5, this->eyePosition.z,
			this->eyePosition.x + this->centerPosition.x,
			this->eyePosition.y + this->centerPosition.y + 5,
			this->eyePosition.z + this->centerPosition.z,
			this->upPosition.x, this->upPosition.y + 5, this->upPosition.z);
#ifdef EXAMPLE_SOLUTION
		trainCamView(this, aspect);
#endif
		update();
	}
}

QMatrix4x3 matrixMultiply(QMatrix4x3 a, QMatrix4x4 b) {
	float temp[3][4];

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 4; j++) {
			temp[i][j] = 0.0f;
			for (int k = 0; k < 4; k++) {
				temp[i][j] += a(i,k) * b(k,j);
			}
		}
	}

	return QMatrix4x3((float*)temp);
}

Vector3 matrixVect4Multiply(QMatrix4x3 m, vector<float> vect) {
	float temp[3];

	for (int i = 0; i < 3; i++) {
		temp[i] = 0.0f;
		for (int k = 0; k < 4; k++) {
			temp[i] += m(i,k) * vect[i];
		}
	}

	return Vector3(temp[0], temp[1], temp[2]);
}

//************************************************************************
//
// * this draws all of the stuff in the world
//
//	NOTE: if you're drawing shadows, DO NOT set colors (otherwise, you get 
//       colored shadows). this gets called twice per draw 
//       -- once for the objects, once for the shadows
//########################################################################
// TODO: 
// if you have other objects in the world, make sure to draw them
//########################################################################
//========================================================================
void TrainView::drawStuff(bool doingShadows)
{
	// Draw the control points
	// don't draw the control points if you're driving 
	// (otherwise you get sea-sick as you drive through them)

	if (this->camera != 2) {
		for (size_t i = 0; i < this->m_pTrack->points.size(); ++i) {
			if (!doingShadows) {
				if (((int)i) != selectedCube)
					glColor3ub(240, 60, 60);
				else
					glColor3ub(240, 240, 30);
			}
			this->m_pTrack->points[i].draw();
		}
		update();
	}
	// draw the track
	//####################################################################
	// TODO: 
	// call your own track drawing code
	//####################################################################
	Spline splineType = (Spline)curve;

	for (size_t i = 0; i < m_pTrack->points.size(); ++i) {
		// pos
		Pnt3f cp_pos_p1 = m_pTrack->points[i].pos;
		Pnt3f cp_pos_p2 = m_pTrack->points[(i + 1) % m_pTrack->points.size()].pos;
		// orient
		Pnt3f cp_orient_p1 = m_pTrack->points[i].orient;
		Pnt3f cp_orient_p2 = m_pTrack->points[(i + 1) % m_pTrack->points.size()].orient;

		float percent = 1.0f;
		float t = 0;
		Pnt3f qt, qt0, qt1, orient_t;
		Vector3 vect;

		// initialize
		switch (splineType)
		{
		case TrainView::Spline::Linear:
			qt = (1.0 - t) * cp_pos_p1 + t * cp_pos_p2;
			break;
		case TrainView::Spline::CardinalCubic:
			vect = this->getCardinalGMTmatrix(i, t, this->tension, 1);
			qt.x = vect.x;	qt.y = vect.y;	qt.z = vect.z;
			break;
		case TrainView::Spline::CubicBSpline:
			vect = this->getBSplineGMTmatrix(i, t, 1);
			qt.x = vect.x;	qt.y = vect.y;	qt.z = vect.z;
			break;
		default:
			break;
		}
		percent = 1.0f / this->DIVIDE_LINE;
		
		for (size_t j = 0; j < DIVIDE_LINE; j++) {
			qt0 = qt;

			switch (splineType)
			{
			case TrainView::Spline::Linear:
				orient_t = (1.0 - t) * cp_orient_p1 + t * cp_orient_p2;
				break;
			case TrainView::Spline::CardinalCubic:
				vect = this->getCardinalGMTmatrix(i, t, this->tension, 2);
				orient_t.x = vect.x;
				orient_t.y = vect.y;
				orient_t.z = vect.z;
				break;
			case TrainView::Spline::CubicBSpline:
				vect = this->getBSplineGMTmatrix(i, t, 2);
				orient_t.x = vect.x;
				orient_t.y = vect.y;
				orient_t.z = vect.z;
				break;
			default:
				break;
			}
			t += percent;

			switch (splineType)
			{
			case TrainView::Spline::Linear:
				qt = (1.0 - t) * cp_pos_p1 + t * cp_pos_p2;
				break;
			case TrainView::Spline::CardinalCubic:
				vect = this->getCardinalGMTmatrix(i, t, this->tension, 1);
				qt.x = vect.x;
				qt.y = vect.y;
				qt.z = vect.z;
				break;
			case TrainView::Spline::CubicBSpline:
				vect = this->getBSplineGMTmatrix(i, t, 1);
				qt.x = vect.x;
				qt.y = vect.y;
				qt.z = vect.z;
				break;
			default:
				break;
			}
			qt1 = qt;

			orient_t.normalize();
			Pnt3f cross_t = (qt1 - qt0) * orient_t;
			cross_t.normalize();
			cross_t = cross_t * 2.5f;

			glLineWidth(4);
			glBegin(GL_LINES);
				if (!doingShadows) {
					glColor3ub(32, 32, 64);
				}
				if (this->track == 0) {
					glVertex3f(qt0.x, qt0.y, qt0.z);
					glVertex3f(qt1.x, qt1.y, qt1.z);
				}
				else {
					// inline
					glVertex3f(qt0.x + cross_t.x, qt0.y + cross_t.y, qt0.z + cross_t.z);
					glVertex3f(qt1.x + cross_t.x, qt1.y + cross_t.y, qt1.z + cross_t.z);
					// outline
					glVertex3f(qt0.x - cross_t.x, qt0.y - cross_t.y, qt0.z - cross_t.z);
					glVertex3f(qt1.x - cross_t.x, qt1.y - cross_t.y, qt1.z - cross_t.z);
				}
			glEnd();
			glLineWidth(1);
			if ((this->track == 1 && j % 2 == 0) || this->track == 2) {
				// track
				glBegin(GL_POLYGON);
				if (!doingShadows) {
					glColor3ub(100, 85, 0);
				}
				//glActiveTexture(GL_TEXTURE2);
				//this->Textures[2]->bind();
				//glTexCoord2f(1.0, 0.0);
				glVertex3f(qt0.x + 2 * cross_t.x, qt0.y + 2 * cross_t.y, qt0.z + 2 * cross_t.z);
				//glTexCoord2f(1.0, 1.0);
				glVertex3f(qt1.x + 2 * cross_t.x, qt1.y + 2 * cross_t.y, qt1.z + 2 * cross_t.z);
				//glTexCoord2f(0.0, 1.0);
				glVertex3f(qt1.x - 2 * cross_t.x, qt1.y - 2 * cross_t.y, qt1.z - 2 * cross_t.z);
				//glTexCoord2f(0.0, 0.0);
				glVertex3f(qt0.x - 2 * cross_t.x, qt0.y - 2 * cross_t.y, qt0.z - 2 * cross_t.z);
				glEnd();
			}
		}

	}

#ifdef EXAMPLE_SOLUTION
	drawTrack(this, doingShadows);
#endif

	// draw the train
	//####################################################################
	// TODO: 
	//	call your own train drawing code
	//####################################################################
	//glColor3f(1.0, 1.0, 1.0);
	//this->trainModel->render(false, false);
	this->drawTrain(this->t_time);

#ifdef EXAMPLE_SOLUTION
	// don't draw the train if you're looking out the front window
	if (!tw->trainCam->value())
		drawTrain(this, doingShadows);
#endif
}

void TrainView::drawTracks() {

}

void TrainView::
doPick(int mx, int my)
//========================================================================
{
	// since we'll need to do some GL stuff so we make this window as 
	// active window
	makeCurrent();

	// get the viewport - most reliable way to turn mouse coords into GL coords
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPickMatrix((double)mx, (double)(viewport[3] - my),
		5, 5, viewport);

	// now set up the projection
	setProjection();

	// now draw the objects - but really only see what we hit
	GLuint buf[100];
	glSelectBuffer(100, buf);
	glRenderMode(GL_SELECT);
	glInitNames();
	glPushName(0);


	// draw the cubes, loading the names as we go
	for (size_t i = 0; i < m_pTrack->points.size(); ++i) {
		glLoadName((GLuint)(i + 1));
		m_pTrack->points[i].draw();
	}

	// go back to drawing mode, and see how picking did
	int hits = glRenderMode(GL_RENDER);
	if (hits) {
		// warning; this just grabs the first object hit - if there
		// are multiple objects, you really want to pick the closest
		// one - see the OpenGL manual 
		// remember: we load names that are one more than the index
		selectedCube = buf[3] - 1;
	}
	else // nothing hit, nothing selected
		selectedCube = -1;
}

void TrainView::drawTrain(float t) {
	t *= this->m_pTrack->points.size();
	int i;
	for (i = 0; t > 1; t -= 1)
		i++;

	//pos
	Pnt3f cp_pos_p1 = this->m_pTrack->points[i].pos;
	Pnt3f cp_pos_p2 = this->m_pTrack->points[(i + 1) % m_pTrack->points.size()].pos;
	// orient
	Pnt3f cp_orient_p1 = this->m_pTrack->points[i].orient;
	Pnt3f cp_orient_p2 = this->m_pTrack->points[(i + 1) % m_pTrack->points.size()].orient;

	Spline splineType = (Spline)curve;
	Pnt3f qt, orient_t, qt0, qt1;
	Vector3 position;
	float percent = 1.0f / this->DIVIDE_LINE;

	switch (splineType)
	{
	case TrainView::Spline::Linear:
		qt = (1 - t) * cp_pos_p1 + t * cp_pos_p2;
		orient_t = (1 - t) * cp_orient_p1 + t * cp_orient_p2;
		break;
	case TrainView::Spline::CardinalCubic:
		position = this->getCardinalGMTmatrix(i, t, this->tension, 1);
		qt.x = position.x;	qt.y = position.y; qt.z = position.z;
		position = this->getCardinalGMTmatrix(i, t, this->tension, 2);
		orient_t.x = position.x;	orient_t.y = position.y; orient_t.z = position.z;
		break;
	case TrainView::Spline::CubicBSpline:
		position = this->getBSplineGMTmatrix(i, t, 1);
		qt.x = position.x;	qt.y = position.y; qt.z = position.z;
		position = this->getBSplineGMTmatrix(i, t, 2);
		orient_t.x = position.x;	orient_t.y = position.y; orient_t.z = position.z;
		break;
	default:
		break;
	}
	qt0 = qt;
	t += percent;
	switch (splineType)
	{
	case TrainView::Spline::Linear:
		qt = (1 - t) * cp_pos_p1 + t * cp_pos_p2;
		break;
	case TrainView::Spline::CardinalCubic:
		position = this->getCardinalGMTmatrix(i, t, this->tension, 1);
		qt.x = position.x;	qt.y = position.y; qt.z = position.z;
		break;
	case TrainView::Spline::CubicBSpline:
		position = this->getBSplineGMTmatrix(i, t, 1);
		qt.x = position.x;	qt.y = position.y; qt.z = position.z;
		break;
	default:
		break;
	}
	qt1 = qt;

	orient_t.normalize();
	Pnt3f cross_t = (qt1 - qt0) * orient_t;
	cross_t.normalize();
	cross_t = cross_t * 2.5f;

	// set train view parameter
	switch (splineType)
	{
	case TrainView::Spline::Linear:
		qt = (1 - (t + percent)) * cp_pos_p1 + (t + percent) * cp_pos_p2;
		break;
	case TrainView::Spline::CardinalCubic:
		position = this->getCardinalGMTmatrix(i, (t + percent), this->tension, 1);
		qt.x = position.x;	qt.y = position.y; qt.z = position.z;
		break;
	case TrainView::Spline::CubicBSpline:
		position = this->getBSplineGMTmatrix(i, (t + percent), 1);
		qt.x = position.x;	qt.y = position.y; qt.z = position.z;
		break;
	default:
		break;
	}
	this->eyePosition = qt;
	this->centerPosition = qt - qt1;
	this->upPosition = Pnt3f(qt.x, qt.y + 10, qt.z) - qt;

	// train head
	glColor3ub(0, 0, 0);
	glBegin(GL_QUADS);
		// back
		glVertex3f(qt0.x + cross_t.x, qt0.y + cross_t.y, qt0.z + cross_t.z);
		glVertex3f(qt0.x + cross_t.x, qt0.y + cross_t.y + 10, qt0.z + cross_t.z);			   
		glVertex3f(qt0.x - cross_t.x, qt0.y - cross_t.y + 10, qt0.z - cross_t.z);
		glVertex3f(qt0.x - cross_t.x, qt0.y - cross_t.y, qt0.z - cross_t.z);
		// up
		glVertex3f(qt0.x + cross_t.x, qt0.y + cross_t.y + 10, qt0.z + cross_t.z);
		glVertex3f(qt1.x + cross_t.x, qt1.y + cross_t.y + 10, qt1.z + cross_t.z);
		glVertex3f(qt1.x - cross_t.x, qt1.y - cross_t.y + 10, qt1.z - cross_t.z);
		glVertex3f(qt0.x - cross_t.x, qt0.y - cross_t.y + 10, qt0.z - cross_t.z);
		// front
		glVertex3f(qt1.x + cross_t.x, qt1.y + cross_t.y, qt1.z + cross_t.z);
		glVertex3f(qt1.x + cross_t.x, qt1.y + cross_t.y + 10, qt1.z + cross_t.z);
		glVertex3f(qt1.x - cross_t.x, qt1.y - cross_t.y + 10, qt1.z - cross_t.z);
		glVertex3f(qt1.x - cross_t.x, qt1.y - cross_t.y, qt1.z - cross_t.z);
		// down
		glVertex3f(qt0.x + cross_t.x, qt0.y + cross_t.y, qt0.z + cross_t.z);
		glVertex3f(qt1.x + cross_t.x, qt1.y + cross_t.y, qt1.z + cross_t.z);
		glVertex3f(qt1.x - cross_t.x, qt1.y - cross_t.y, qt1.z - cross_t.z);
		glVertex3f(qt0.x - cross_t.x, qt0.y - cross_t.y, qt0.z - cross_t.z);
		// left
		glVertex3f(qt0.x - cross_t.x, qt0.y - cross_t.y, qt0.z - cross_t.z);
		glVertex3f(qt0.x - cross_t.x, qt0.y - cross_t.y + 10, qt0.z - cross_t.z);
		glVertex3f(qt1.x - cross_t.x, qt1.y - cross_t.y + 10, qt1.z - cross_t.z);
		glVertex3f(qt1.x - cross_t.x, qt1.y - cross_t.y, qt1.z - cross_t.z);
		// right
		glVertex3f(qt0.x + cross_t.x, qt0.y + cross_t.y, qt0.z + cross_t.z);
		glVertex3f(qt0.x + cross_t.x, qt0.y + cross_t.y + 10, qt0.z + cross_t.z);
		glVertex3f(qt1.x + cross_t.x, qt1.y + cross_t.y + 10, qt1.z + cross_t.z);
		glVertex3f(qt1.x + cross_t.x, qt1.y + cross_t.y, qt1.z + cross_t.z);
	glEnd();

	int count = 0, tempI = 0;
	t -= percent;
	while (count < this->carNum) {
		t -= 2 * percent;
		if (t < 0) {
			t = 1.0;
			i -= 1;
			i = i < 0 ? m_pTrack->points.size()-1 : i;
		}
		cp_pos_p1 = this->m_pTrack->points[i].pos;
		cp_pos_p2 = this->m_pTrack->points[(i + 1) % m_pTrack->points.size()].pos;

		cp_orient_p1 = this->m_pTrack->points[i].orient;
		cp_orient_p2 = this->m_pTrack->points[(i + 1) % m_pTrack->points.size()].orient;
		switch (splineType)
		{
		case TrainView::Spline::Linear:
			qt = (1 - t) * cp_pos_p1 + t * cp_pos_p2;
			orient_t = (1 - t) * cp_orient_p1 + t * cp_orient_p2;
			break;
		case TrainView::Spline::CardinalCubic:
			position = this->getCardinalGMTmatrix(i, t, this->tension, 1);
			qt.x = position.x;	qt.y = position.y; qt.z = position.z;
			position = this->getCardinalGMTmatrix(i, t, this->tension, 2);
			orient_t.x = position.x;	orient_t.y = position.y; orient_t.z = position.z;
			break;
		case TrainView::Spline::CubicBSpline:
			position = this->getBSplineGMTmatrix(i, t, 1);
			qt.x = position.x;	qt.y = position.y; qt.z = position.z;
			position = this->getBSplineGMTmatrix(i, t, 2);
			orient_t.x = position.x;	orient_t.y = position.y; orient_t.z = position.z;
			break;
		default:
			break;
		}
		qt0 = qt;
		t += percent;
		switch (splineType)
		{
		case TrainView::Spline::Linear:
			qt = (1 - t) * cp_pos_p1 + t * cp_pos_p2;
			break;
		case TrainView::Spline::CardinalCubic:
			position = this->getCardinalGMTmatrix(i, t, this->tension, 1);
			qt.x = position.x;	qt.y = position.y; qt.z = position.z;
			break;
		case TrainView::Spline::CubicBSpline:
			position = this->getBSplineGMTmatrix(i, t, 1);
			qt.x = position.x;	qt.y = position.y; qt.z = position.z;
			break;
		default:
			break;
		}
		qt1 = qt;

		orient_t.normalize();
		Pnt3f cross_t = (qt1 - qt0) * orient_t;
		cross_t.normalize();
		cross_t = cross_t * 2.5f;
		if (tempI == 0) {
			// train head
			glColor3ub(120, 120, 120);
			glBegin(GL_QUADS);
				// back
				glVertex3f(qt0.x + cross_t.x, qt0.y + cross_t.y, qt0.z + cross_t.z);
				glVertex3f(qt0.x + cross_t.x, qt0.y + cross_t.y + 10, qt0.z + cross_t.z);
				glVertex3f(qt0.x - cross_t.x, qt0.y - cross_t.y + 10, qt0.z - cross_t.z);
				glVertex3f(qt0.x - cross_t.x, qt0.y - cross_t.y, qt0.z - cross_t.z);
				// up
				glVertex3f(qt0.x + cross_t.x, qt0.y + cross_t.y + 10, qt0.z + cross_t.z);
				glVertex3f(qt1.x + cross_t.x, qt1.y + cross_t.y + 10, qt1.z + cross_t.z);
				glVertex3f(qt1.x - cross_t.x, qt1.y - cross_t.y + 10, qt1.z - cross_t.z);
				glVertex3f(qt0.x - cross_t.x, qt0.y - cross_t.y + 10, qt0.z - cross_t.z);
				// front
				glVertex3f(qt1.x + cross_t.x, qt1.y + cross_t.y, qt1.z + cross_t.z);
				glVertex3f(qt1.x + cross_t.x, qt1.y + cross_t.y + 10, qt1.z + cross_t.z);
				glVertex3f(qt1.x - cross_t.x, qt1.y - cross_t.y + 10, qt1.z - cross_t.z);
				glVertex3f(qt1.x - cross_t.x, qt1.y - cross_t.y, qt1.z - cross_t.z);
				// down
				glVertex3f(qt0.x + cross_t.x, qt0.y + cross_t.y, qt0.z + cross_t.z);
				glVertex3f(qt1.x + cross_t.x, qt1.y + cross_t.y, qt1.z + cross_t.z);
				glVertex3f(qt1.x - cross_t.x, qt1.y - cross_t.y, qt1.z - cross_t.z);
				glVertex3f(qt0.x - cross_t.x, qt0.y - cross_t.y, qt0.z - cross_t.z);
				// left
				glVertex3f(qt0.x - cross_t.x, qt0.y - cross_t.y, qt0.z - cross_t.z);
				glVertex3f(qt0.x - cross_t.x, qt0.y - cross_t.y + 10, qt0.z - cross_t.z);
				glVertex3f(qt1.x - cross_t.x, qt1.y - cross_t.y + 10, qt1.z - cross_t.z);
				glVertex3f(qt1.x - cross_t.x, qt1.y - cross_t.y, qt1.z - cross_t.z);
				// right
				glVertex3f(qt0.x + cross_t.x, qt0.y + cross_t.y, qt0.z + cross_t.z);
				glVertex3f(qt0.x + cross_t.x, qt0.y + cross_t.y + 10, qt0.z + cross_t.z);
				glVertex3f(qt1.x + cross_t.x, qt1.y + cross_t.y + 10, qt1.z + cross_t.z);
				glVertex3f(qt1.x + cross_t.x, qt1.y + cross_t.y, qt1.z + cross_t.z);
			glEnd();
			glColor3ub(30, 30, 30);
			//this->humanModel->moveModel(30, Point3d((qt0.x + qt1.x) / 2, (qt0.y + qt1.y) / 2, (qt0.z + qt1.z) / 2));
			//this->humanModel->render(false, false);
			count++;
		}
		else {
			glColor3ub(255, 255, 255);
			glBegin(GL_LINES);
				glVertex3f(qt0.x, qt0.y + 3, qt0.z);
				glVertex3f(qt1.x, qt1.y + 3, qt1.z);
			glEnd();
		}
		tempI = (tempI + 1) % 2;
	}

	//glColor3ub(32, 32, 64);
	//this->trainModel->moveModel(20, Point3d(qt0.x + cross_t.x, qt0.y + cross_t.y + 3, qt0.z + cross_t.z));
	//this->trainModel->render();
}

// i, t, tension, type == 1 -> pos : type == 2 -> orient
Vector3 TrainView::getCardinalGMTmatrix(int i, float t, float tension, int type) {
	Vector3 resultVector;
	Pnt3f cp_p0;
	Pnt3f cp_p1;
	Pnt3f cp_p2;
	Pnt3f cp_p3;

	if (type == 2) {
		cp_p0 = this->m_pTrack->points[((i - 1) < 0 ? this->m_pTrack->points.size() - 1 : (i - 1))].orient;
		cp_p1 = this->m_pTrack->points[i].orient;
		cp_p2 = this->m_pTrack->points[(i + 1) % m_pTrack->points.size()].orient;
		cp_p3 = this->m_pTrack->points[(i + 2) % m_pTrack->points.size()].orient;
	}
	else {
		cp_p0 = this->m_pTrack->points[((i - 1) < 0 ? this->m_pTrack->points.size() - 1 : (i - 1))].pos;
		cp_p1 = this->m_pTrack->points[i].pos;
		cp_p2 = this->m_pTrack->points[(i + 1) % m_pTrack->points.size()].pos;
		cp_p3 = this->m_pTrack->points[(i + 2) % m_pTrack->points.size()].pos;
	}

	float gArr[12] =
	{
		cp_p0.x, cp_p1.x, cp_p2.x, cp_p3.x,
		cp_p0.y, cp_p1.y, cp_p2.y, cp_p3.y,
		cp_p0.z, cp_p1.z, cp_p2.z, cp_p3.z,
	};

	float mArr[16] = {
		-1.0f, 2.0f, -1.0f, 0.0f,
		2.0f / tension - 1.0f, -3.0f / tension + 1.0f, 0.0f, 1.0f / tension,
		-2.0f / tension + 1.0f, 3.0f / tension - 2.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 0.0f, 0.0f
	};
	vector<float> tN = { t * t * t, t * t, t, 1.0f };

	resultVector.x = ((mArr[0] * tN[0] + mArr[1] * tN[1] + mArr[2] * tN[2] + mArr[3]) * cp_p0.x +
		(mArr[4] * tN[0] + mArr[5] * tN[1] + mArr[6] * tN[2] + mArr[7]) * cp_p1.x +
		(mArr[8] * tN[0] + mArr[9] * tN[1] + mArr[10] * tN[2] + mArr[11]) * cp_p2.x +
		(mArr[12] * tN[0] + mArr[13] * tN[1] + mArr[14] * tN[2] + mArr[15]) * cp_p3.x)* tension;
	
	resultVector.y = ((mArr[0] * tN[0] + mArr[1] * tN[1] + mArr[2] * tN[2] + mArr[3]) * cp_p0.y +
		(mArr[4] * tN[0] + mArr[5] * tN[1] + mArr[6] * tN[2] + mArr[7]) * cp_p1.y +
		(mArr[8] * tN[0] + mArr[9] * tN[1] + mArr[10] * tN[2] + mArr[11]) * cp_p2.y +
		(mArr[12] * tN[0] + mArr[13] * tN[1] + mArr[14] * tN[2] + mArr[15]) * cp_p3.y)* tension;
	
	resultVector.z = ((mArr[0] * tN[0] + mArr[1] * tN[1] + mArr[2] * tN[2] + mArr[3]) * cp_p0.z +
		(mArr[4] * tN[0] + mArr[5] * tN[1] + mArr[6] * tN[2] + mArr[7]) * cp_p1.z +
		(mArr[8] * tN[0] + mArr[9] * tN[1] + mArr[10] * tN[2] + mArr[11]) * cp_p2.z +
		(mArr[12] * tN[0] + mArr[13] * tN[1] + mArr[14] * tN[2] + mArr[15]) * cp_p3.z) * tension;

	return resultVector;
}

// i, t, type == 1 -> pos : type == 2 -> orient
Vector3 TrainView::getBSplineGMTmatrix(int i, float t, int type) {
	Vector3 resultVector;
	Pnt3f cp_p0;
	Pnt3f cp_p1;
	Pnt3f cp_p2;
	Pnt3f cp_p3;

	if (type == 2) {
		cp_p0 = this->m_pTrack->points[((i - 1) < 0 ? this->m_pTrack->points.size() - 1 : (i - 1))].orient;
		cp_p1 = this->m_pTrack->points[i].orient;
		cp_p2 = this->m_pTrack->points[(i + 1) % m_pTrack->points.size()].orient;
		cp_p3 = this->m_pTrack->points[(i + 2) % m_pTrack->points.size()].orient;
	}
	else {
		cp_p0 = this->m_pTrack->points[((i - 1) < 0 ? this->m_pTrack->points.size() - 1 : (i - 1))].pos;
		cp_p1 = this->m_pTrack->points[i].pos;
		cp_p2 = this->m_pTrack->points[(i + 1) % m_pTrack->points.size()].pos;
		cp_p3 = this->m_pTrack->points[(i + 2) % m_pTrack->points.size()].pos;
	}

	float gArr[12] = {
		cp_p0.x, cp_p1.x, cp_p2.x, cp_p3.x,
		cp_p0.y, cp_p1.y, cp_p2.y, cp_p3.y,
		cp_p0.z, cp_p1.z, cp_p2.z, cp_p3.z,
	};

	float mArr[16] = {
		-1.0f, 3.0f, -3.0f, 1.0f,
		3.0f, -6.0f, 0.0f, 4.0f,
		-3.0f, 3.0f, 3.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 0.0f
	};
	vector<float> tN = { t * t * t, t * t, t, 1.0f };

	float bTension = 6 * this->tension;

	resultVector.x = ((mArr[0] * tN[0] + mArr[1] * tN[1] + mArr[2] * tN[2] + mArr[3]) * cp_p0.x +
		(mArr[4] * tN[0] + mArr[5] * tN[1] + mArr[6] * tN[2] + mArr[7]) * cp_p1.x +
		(mArr[8] * tN[0] + mArr[9] * tN[1] + mArr[10] * tN[2] + mArr[11]) * cp_p2.x +
		(mArr[12] * tN[0] + mArr[13] * tN[1] + mArr[14] * tN[2] + mArr[15]) * cp_p3.x) / bTension;

	resultVector.y = ((mArr[0] * tN[0] + mArr[1] * tN[1] + mArr[2] * tN[2] + mArr[3]) * cp_p0.y +
		(mArr[4] * tN[0] + mArr[5] * tN[1] + mArr[6] * tN[2] + mArr[7]) * cp_p1.y +
		(mArr[8] * tN[0] + mArr[9] * tN[1] + mArr[10] * tN[2] + mArr[11]) * cp_p2.y +
		(mArr[12] * tN[0] + mArr[13] * tN[1] + mArr[14] * tN[2] + mArr[15]) * cp_p3.y) / bTension;

	resultVector.z = ((mArr[0] * tN[0] + mArr[1] * tN[1] + mArr[2] * tN[2] + mArr[3]) * cp_p0.z +
		(mArr[4] * tN[0] + mArr[5] * tN[1] + mArr[6] * tN[2] + mArr[7]) * cp_p1.z +
		(mArr[8] * tN[0] + mArr[9] * tN[1] + mArr[10] * tN[2] + mArr[11]) * cp_p2.z +
		(mArr[12] * tN[0] + mArr[13] * tN[1] + mArr[14] * tN[2] + mArr[15]) * cp_p3.z) / bTension;

	return resultVector;
}
