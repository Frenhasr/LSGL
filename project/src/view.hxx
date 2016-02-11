/*! \file view.hxx
 *
 * Handles functions for window and rendering objects 
 * \author Lamont Samuels 
 */
#ifndef _VIEW_HXX_
#define _VIEW_HXX_

#include "cs237.hxx"
#include "triangle.hxx" 
#include "Lsystem.hxx" 
#include "axes.hxx"
using 		namespace std; 
using 		namespace cs237; 

enum RenderMode {
	WIREFRAME = 0,	//!< render scene as a wireframe
	NUM_RENDER_MODES	//!< == to the number of rendering modes
};

enum AXES {
	X_AXIS = 0,	
	Y_AXIS,	
	Z_AXIS	
};

struct View {
	public:
		GLFWwindow*				win;		//!< the application window
		cs237::ShaderProgram* 	shader;			//!< the shader program for triangle 
		
		cs237::mat4f 	camera; 					//!< the camera matrix 
		cs237::mat4f 	projectionMat; 			//!< the projection matrix  
			
		GLint 			viewMatLoc; 					    /*!< the uniform location for camera matrix in the shaders*/ 
		GLint 			projMatLoc; 					/*!< the uniform location for project view matrix in the shaders*/ 
		
		Triangle* 		tri;							/*! \brief a triangle to render to the screen */ 
		
		int 			width, height; 						//!< window dimensions
		
		/*! \brief Initialize the view */ 
		View(int w, int h, GLFWwindow* win); 		


		void VTranspose (AXES axis, int mode);

		void VRotate (AXES axis, int mode);

		void upAxis();

		/*! \brief init triangle mesh */
		void InitTriangle();  

		/*! \brief compiles the shaders */ 
		void InitShaders(); 
		  
  		/*! \brief initialize the projection matrix based on the view state. */
    	void InitProjMatrix ();

  		/*! \brief initialize the camera matrix based on the view state. */
    	void InitViewMatrix ();

		/*! \brief renders the triangle to the screen */
		void Render();

		void BindFramebuffer();

		bool			needsRedraw;
		bool			drawAxes;	//!< draw the axes when true
		float 			stepsize;

	private:
		LSystem* 		tree;

		class Axes		*axes;		//!< for drawing the world-space axes

		
		vec3f 			camPos;   					//!< camera position in world space 
    	vec3f 			camAt;					//!< camera direction in world space
		vec3f 			camUp;    					//!< camera up vector in world space 
		vec3f 			camDir;    					//!< camera up vector in world space 

		mat4f 			viewMat;
		mat4f 			projMat;

		mat4f			camRot;		//!< cumlative camera rotation
		float			camOffset;	//!< offset for camera from initial distance to lookAt point.
		float			minOffset;	//!< minimum allowed offset
		float			maxOffset;	//!< maximum allowed offset
}; 
#endif /* !_VIEW_HXX_*/ 