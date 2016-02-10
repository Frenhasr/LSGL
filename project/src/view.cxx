/*! \file view.cxx
 *
 * \author Lamont Samuels 
 */
#include "view.hxx"

/* clip planes in world coordinates */
#define NEAR_Z		0.2f 
#define FAR_Z		100.0f
#define FOVY		65.0f	/* field-of-view in Y-dimension */

/*! \brief performs initializations needed before starting up the view*/ 
void View::Init (int w, int h) {
	this->camPos   = cs237::vec3f(1.0, 0.0, -3);
	this->camDir   = cs237::vec3f(0.0, 0.0, 1.0);
	this->camUp	   = cs237::vec3f(0.0, 1.0, 0.0);

	// this->camRot	= cs237::mat4f(1.0f);
	// this->camOffset	= 0;
	// float dist		= distance(this->camPos, this->camPos + this->camDir);
	// this->minOffset	= cs237::min(0.0f, MIN_DIST - dist);
	// this->maxOffset	= cs237::max(0.0f, MAX_DIST - dist);

    // {    
    //     this->_cam.setFOV (60.0);
    //     this->_cam.setNearFar (10.0, 1.5 * double(this->_map->CellWidth()) * double(this->_map->hScale()));
    //     this->Resize (wid, ht);
    //     this->_cam.generateFrustrum();
    // }

	this->width    = w;
	this->height   = h;
	this->shader   = nullptr; 
	this->drawAxes	= false;
}

//! \brief performs any initialization before using the view */	
void View::InitShaders() {
	if (this->shader == nullptr) {
		cs237::VertexShader vsh("../shaders/shader.vert");
		cs237::FragmentShader fsh("../shaders/shader.frag");
		this->shader = new cs237::ShaderProgram (vsh, fsh);
		if (this->shader == nullptr)
			exit (1);
	}
	
	//#2 Retrieve the uniform locations for the model and projection matrices 
	cameraLoc = shader->UniformLocation("camera"); 
	projectionLoc = shader->UniformLocation("projection");
}

//! \brief initialize the projection matrix based on the view state. */
// void View::InitProjMatrix() {
// 	this->projectionMat = cs237::perspective (
// 		FOVY,
// 		(float)this->width / (float)this->height,NEAR_Z, FAR_Z); 
// }

void View::InitProjMatrix () {
  // compute vertical field of view as per 5.3.1 of M3D
	float eInv = tan (cs237::radians(FOVY) * 0.5f);
	float a = float(this->height) / float(this->width);
	float beta = cs237::degrees(2.0 * atan (eInv * a));

	this->projectionMat = cs237::perspective (
		beta,
		1.0f / a,
		NEAR_Z,
		FAR_Z
		);

}

//! \brief initialize the model-view matrix based on the view state. */
void View::InitCamera() {		
	this->camera = cs237::lookAt(
						 this->camPos,
						 this->camPos + this->camDir,
						 this->camUp
						 );
}

/*
void View::InitViewMatrix ()
{
	cs237::mat4f mvMat = cs237::lookAt (
		this->camPos,
		this->camAt,
		this->camUp
		);

	// apply rotation followed by translation
	this->viewMat = mvMat * cs237::translate(cs237::vec3f(
		0.0f,
		0.0f, 
		this->camOffset)) * this->camRot;
}
//*/

/* rotate the camera around the look-at point by the given angle (in degrees)
 */
// void View::RotateLeft (float angle) {
// 	this->camRot = cs237::rotateY(-angle) * this->camRot;
// }

/* rotate the camera up by the given angle (in degrees)
 */
// void View::RotateUp (float angle) {
// 	this->camRot = cs237::rotateX(angle) * this->camRot;
// }

/* move the camera towards the look-at point by the given distance
 */
// void View::Move (float dist)
// {
// 	if (dist < 0.0f)  // move away from camera
// 		this->camOffset = cs237::max(this->minOffset, this->camOffset - dist);
// 	else // move toward camera
// 		this->camOffset = cs237::min(this->maxOffset, this->camOffset - dist);
// }

//! \brief init triangle mesh */
void View::InitTriangle() {
	this->tri = new struct Triangle();
	this->tri->Init();
	this->tri->posLoc = shader->AttributeLocation("position");
	this->tri->colorLoc = shader->AttributeLocation("color");

    this->tree = new LSystem ("X", 1);
	this->axes		= new Axes(1.5f);
}

void View::Render() {
	const GLfloat color[] = { 0.5f, 0.5f , 0.5f, 1.0f}; 
							
	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set the background to black 
	glClearBufferfv(GL_COLOR,0,color); 

	glEnable(GL_DEPTH_TEST);

	//Begin using the shader program 
	shader->Use();

	//Pass in the camera and projection matrices 
	cs237::setUniform(cameraLoc, camera);	
	cs237::setUniform(projectionLoc, projectionMat);
 	
	if (this->drawAxes) {
		this->axes->Draw(projectionMat, camera);
	}

	//Render the triangle. 
	//tri->Render();

    tree->Draw();

    // if (this->drawAxes) {
	// }
}
