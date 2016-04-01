/*! \file view.cxx
 *
 * \author Lamont Samuels 
 */
#include "view.hxx"

/* clip planes in world coordinates */
#define NEAR_Z		0.2f 
#define FAR_Z		500.0f
#define FOVY		65.0f	/* field-of-view in Y-dimension */

#define MIN_DIST	5.0f
#define MAX_DIST	1500.0f

#define DEBUG_FLAG       0

/* CONTROL MATRICES
 */
mat4f 	Rx, Ry, Rz;
bool 			rRx,
				rRy,
				rRz = false;

mat4f 	Tx, Ty, Tz;
bool 			rTx,
				rTy,
				rTz = false;

vec3f 	amx, amy, amz;
bool 			ramx,
				ramy,
				ramz = false;

// the multiplyer for calculating the step of each matrix

/*! \brief performs initializations needed before starting up the view*/ 
View::View (int w, int h, GLFWwindow *win) {
	glfwSetWindowUserPointer (win, this);
	this->win = win;

	this->stepsize = 5.0f;

	this->width    = w;
	this->height   = h;

	this->needsRedraw = true;

	this->camPos	= vec3f(1.0, 0, -9.0);
	this->camAt		= vec3f(0.0, 0.0, 0.0);
	this->camDir 	= normalize(vec3f(0.0, 0.0, 0.0) - this->camPos);
	this->camRight 	= vec3f(0.0, 0.0, 1.0);
	this->camUp 	= vec3f(0.0, 1.0, 0.0);//normalize(cross(this->camDir, this->camRight));

	this->camRot	= cs237::mat4f(1.0f);
	this->camOffset	= 0;

	float dist		= distance(this->camPos, this->camAt);
	this->minOffset	= cs237::min(0.0f, MIN_DIST - dist);
	this->maxOffset	= cs237::max(0.0f, MAX_DIST - dist);

	this->shader   = nullptr;

	this->drawAxes	= false;
}

void View::BindFramebuffer() {
	int fwid, fht; 
	CS237_CHECK (glBindFramebuffer (GL_FRAMEBUFFER, 0) );
	CS237_CHECK (glfwGetFramebufferSize (this->win, &fwid, &fht) );
	CS237_CHECK (glViewport (0, 0, fwid, fht) );

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
	viewMatLoc = shader->UniformLocation("viewMat"); 
	projMatLoc = shader->UniformLocation("projMat");
}


void View::InitProjMatrix () {
  // compute vertical field of view as per 5.3.1 of M3D
	float eInv = tan (cs237::radians(FOVY) * 0.5f);
	float a = float(this->height) / float(this->width);
	float beta = cs237::degrees(2.0 * atan (eInv * a));

	this->projMat = cs237::perspective (
		beta,
		1.0f / a,
		NEAR_Z,
		FAR_Z
		);

}


void View::InitViewMatrix () {
	this->viewMat = cs237::lookAt (
		camPos,
		this->camDir + this->camPos,
		this->camUp
		);
}



void View::upAxis(){
	if (!ramy) {
		amy 		= this->camUp;
		amy 		= normalize(amy);
		rRy = rTy 	= false;
		ramy 		= true;
	}
	if (!ramz) {
		amz 		= this->camDir;
		amz 		= normalize(amz);
		rRz = rTz 	= false;
		ramz 		= true;
	}
	if (!ramx) {
		amx 		= cross(amy,amz);
		amx 		= normalize(amx);
		rRx 		= rTx = false;
		ramx 		= true;
	}
 }



 /* ROTATE:
 	For each axis, the function checks if axes or matrices need updates. Then, it uses the mode to choose if rotating "forward" or "backward." Finally, it deactivates the other axes.
  */

/* VROTATE:
	You either rotate along the camDir (n, m) or along the 
 */

/*
void View::VRotate (AXES axis, int mode) {
	quatf rQ = quatf(0.0f, this->camDir);

	switch (axis) {
		case UP:
			rQ = quatf(mode * 30.0f, this->camUp);
			break;
		case FORWARD:
			rQ = quatf(mode * 30.0f, this->camDir);
			break;
		case RIGHT:
			rQ = quatf(mode * 30.0f, this->camRight);
			break;
		default:
			break;
	}

	this->camDir = vec3f(rQ.toMat4x4() * vec4f(this->camDir, 0.0f));
	this->camRight = vec3f(rQ.toMat4x4() * vec4f(this->camRight, 0.0f));
	this->camUp = vec3f(rQ.toMat4x4() * vec4f(this->camUp, 0.0f));
	this->camPos = vec3f(rQ.toMat4x4() * vec4f(this->camPos, 0.0f));
}
//*/
//*
void View::VRotate (AXES axis, int mode) {
	mat4f updateM;

	switch (axis) {
		case X_AXIS:
			if (!ramx) 
				upAxis();
			if (!rRx)
				Rx = cs237::rotate(3.14f * 10.0f * this->stepsize / 180.0f, amx);

			if (mode == -1) {
				updateM=Rx.inverse();
			}
			else {
				updateM = Rx;
			}

			ramy = ramz = false;
			break;

		case Y_AXIS:
			if (!ramy)
				upAxis();
			if (!rRy) 
				Ry = cs237::rotate(3.14f * 10.0f * this->stepsize / 180.0f, amy);
			if (mode == -1) {
				updateM = Ry.inverse();
			}
			else {
				updateM = Ry;
			}
			
			ramx = ramz = false;
			break;

		case Z_AXIS:
			if (!ramz)
				upAxis();
			if (!rRz)
				Rz = cs237::rotate(3.14f * 10.0f * this->stepsize / 180.0f, amz);
			if (mode == -1) {
				updateM = Rz.inverse();
			}
			else {
				updateM = Rz;
			}
			
			ramx = ramy = false;
			break;
	}

	this->camDir = vec3f(updateM * vec4f(this->camDir, 0.0f));
	this->camUp = vec3f(updateM * vec4f(this->camUp, 0.0f));
 }
//*/

 /* TRANS:
 	For each axis, the function checks if axes or matrices need updates. Then, it uses the mode to choose if  moving forward or backward. Finally, it deactivates the other axes.
  */
/*
 void View::VTranspose (AXES axis, int mode) {
 	mat4f tM;

 	switch (axis) {
 		case UP:
 			tM = translate(mode * this->stepsize * normalize(this->camUp));
 			break;
 		case FORWARD:
 			tM = translate(mode * this->stepsize * normalize(this->camUp));
 			break;
 		case RIGHT:
 			tM = translate(mode * this->stepsize * normalize(this->camRight));
 			break;
 		default:
 			break;
 	}

 	this->camPos 	= vec3f(tM * vec4f(this->camPos, 1.0f));
 	this->camAt 	= vec3f(tM * vec4f(this->camAt, 1.0f));
 	this->camDir = normalize(vec3f(0.0, 0.0, 0.0) - this->camPos);
 }
 //*/
//*
void View::VTranspose (AXES axis, int mode) {
	mat4f updateM;
	vec3f old = this->camPos;

	switch (axis) {
		case X_AXIS:
			if (!ramx)
				upAxis();
			if(!rTx)
				Tx = translate(amx * this->stepsize);
			if (mode == -1) {
				updateM = Tx.inverse();
			}
			else {
				updateM = Tx;
			}
			break;

		case Y_AXIS:
			if (!ramy)
				upAxis();
			if (!rTy) {
				Ty = translate(amy * this->stepsize);
			}
			if (mode == -1) {
				updateM = Ty.inverse();
			}
			else {
				updateM = Ty;
			}
			break;

		case Z_AXIS:
			if (!ramz)
				upAxis();
			if (!rTz) 
				Tz = translate(amz * this->stepsize);
			if (mode == -1) {
				updateM = Tz.inverse();
			}
			else {
				updateM = Tz;
			}
			break;
	}

	this->camPos = vec3f(updateM * vec4f(old, 1.0));
 }
//*/


//! \brief init triangle mesh */
void View::InitTriangle() {
	this->tri = new struct Triangle();
	this->tri->Init();
	this->tri->posLoc = shader->AttributeLocation("position");
	this->tri->colorLoc = shader->AttributeLocation("color");

    this->tree 		= new LSystem ("F", 5, 10.7f);
	this->axes		= new Axes(1.5f);
}

void View::Render() {
	this->InitViewMatrix();
	if (DEBUG_FLAG)
		cout << this->viewMat << endl;

	const GLfloat color[] = { 0.5f, 0.5f , 0.5f, 1.0f};

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearBufferfv(GL_COLOR, 0, color); 

	glEnable(GL_DEPTH_TEST);

	CS237_CHECK(glEnable(GL_LINE_SMOOTH));
	CS237_CHECK(glEnable(GL_POLYGON_SMOOTH));
	CS237_CHECK(glHint(GL_LINE_SMOOTH_HINT, GL_NICEST));
	CS237_CHECK(glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST));
	CS237_CHECK(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));

	//Begin using the shader program 
	shader->Use();

	//Pass in the camera and projection matrices 
	cs237::setUniform(viewMatLoc, viewMat);
	cs237::setUniform(projMatLoc, projMat);
 	
	if (this->drawAxes) {
		tri->Render();
		cout << "Toggled axes" << endl;
		this->axes->Draw(projMat, viewMat);
	}

	//Render the triangle. 

    tree->Draw();
}
