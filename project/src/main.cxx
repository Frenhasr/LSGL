/*! \file main.c
 *
 * \brief this file contains the main function and the GLFW callbacks.
 *
 * \author John Reppy
 * \author Lamont Samuels 
 */

/* CMSC23700 Lab 1 sample code (Fall 2015)
 *
 * COPYRIGHT (c) 2015 John Reppy (http://www.cs.uchicago.edu/~jhr) & Lamont Samuels	(http://www.cs.uchicago.edu/~lamonts)
 * All rights reserved.
 */


#define ROTATE_ANGLE	3.0f		//!< angle of rotation in degrees
#define ZOOM_DIST       0.1f		//! distance to move camera

#include "cs237.hxx"
#include "view.hxx"
#include <unistd.h>

/* the viewer state */
View	* view = nullptr;

/*! \brief Set the viewport to the current size of the framebufffer.
 *		 We use the framebuffer size instead of the window size, because it
 *		 is different on Apple's retina displays.
 */


/***** Callback functions *****/

static void Error (int err, const char *msg) {
	std::cerr << "[GLFW ERROR " << err << "] " << msg << "\n" << std::endl;
}

/*! \brief Run the simulation and then redraw the animation.
 */

void Display (GLFWwindow *win) {
	View *view = (View *)glfwGetWindowUserPointer(win);

    view->Render ();
    glfwSwapBuffers (win);

    view->needsRedraw = false;
}

/*! \brief Window resize callback.
 *	\param wid the new width of the window.
 *	\param ht the new height of the window. 
 */
void Reshape (GLFWwindow *win, int wid, int ht)
{
    View *view = (View *)glfwGetWindowUserPointer(win);

    view->width = wid;
    view->height = ht;

  /* reset the viewport
   * We use the framebuffer size instead of the window size, because it
   * is different on Apple's retina displays.
   */
    int fbWid, fbHt;
    glfwGetFramebufferSize (win, &fbWid, &fbHt);
    glViewport(0, 0 , fbWid, fbHt);

  /* recompute the projection matrix */
    view->InitProjMatrix ();

} /* end of Reshape */

/*! \brief Keyboard-event callback.
 *	\param key The Unicode code point of the character.
 */
void Key (GLFWwindow *win, int key, int scancode, int action, int mods)
{
  // ignore releases, control keys, command keys, etc.
    if ((action != GLFW_RELEASE) || (mods & (GLFW_MOD_CONTROL|GLFW_MOD_ALT|GLFW_MOD_SUPER))) {
    	View *view = (View *)glfwGetWindowUserPointer(win);

		switch (key) {
			case GLFW_KEY_A:  // 'a' or 'A' ==> toggle axes mode
				view->drawAxes = !view->drawAxes;
				view->needsRedraw = true;
				break;
			case GLFW_KEY_Q:  // 'q' or 'Q' ==> quit
				glfwSetWindowShouldClose (win, true);

			case GLFW_KEY_I: //wasd movement
				view->VTranspose(Z_AXIS, 1);
				view->needsRedraw = true;
				break;
			case GLFW_KEY_J: //wasd movement
				view->VRotate(Y_AXIS, 1);
				view->needsRedraw = true;
				break;
			case GLFW_KEY_K: //wasd movement
				view->VTranspose(Z_AXIS, -1);
				view->needsRedraw = true;
				break;
			case GLFW_KEY_L: //wasd movement
				view->VRotate(Y_AXIS, -1);
				view->needsRedraw = true;
				break;
			case GLFW_KEY_U://strafe left
				view->VTranspose(X_AXIS, 1);
				view->needsRedraw = true;
				break;
			case GLFW_KEY_O://strafe right
				view->VTranspose(X_AXIS, -1);
				view->needsRedraw = true;
				break;
			case GLFW_KEY_Y: //move verticle up
				view->VTranspose(Y_AXIS, 1);
				view->needsRedraw = true;
				break;
			case GLFW_KEY_H://move verticle down
				view->VTranspose(Y_AXIS, -1);
				view->needsRedraw = true;
				break;
			case GLFW_KEY_N://rotate positive on z axis
				view->VRotate(Z_AXIS, 1);
				view->needsRedraw = true;
				break;
			case GLFW_KEY_M://rotate negative on z axis
				view->VRotate(Z_AXIS, -1);
				view->needsRedraw = true;
				break;
			case GLFW_KEY_COMMA://rotate positive on x axis
				view->VRotate(X_AXIS, 1);
				view->needsRedraw = true;
				break;
			case GLFW_KEY_PERIOD://rotate negative on x axis
				view->VRotate(X_AXIS, -1);
				view->needsRedraw = true;
				break;
			case GLFW_KEY_UP:
				view->stepsize *= 2.0;
				break;
			case GLFW_KEY_DOWN:
				view->stepsize /= 2.0;
				if (view->stepsize < 1.0)
					view->stepsize = 1.0;
				break;

			default: // ignore all other keys
				return;
		}
    }

} /* end of Key */

/*! \brief Visibility-change callback: sets the visibility state of the view.
 *	\param state the current state of the window; GL_TRUE for iconified, GL_FALSE otherwise.
 */
void Visible (GLFWwindow *win, int state)
{

} /* end of Visible */


/*! \brief the main function for the program
 *	\param argc the count of arguments in \a argv
 *	\param argv the array of command-line arguments.
 */
int main(int argc, const char **argv)
{
	glfwSetErrorCallback(Error);

	glfwInit();

	{
		int major;
		glfwGetVersion(&major, NULL, NULL);
		if (major < 3) {
			std::cerr << "CS237 requires GLFW 3.0 or later\n" << std::endl;
			exit(EXIT_FAILURE);
		}
	}

	glfwWindowHint (GLFW_RESIZABLE, GL_TRUE);
    glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow *window = glfwCreateWindow(
		800, 600,
		"CMSC 29700: Lindenmayer System Open Graphics Language", NULL, NULL
		);
    if (window == nullptr)
		exit (EXIT_FAILURE);

    glfwMakeContextCurrent (window);

  // Check the OpenGL version
    {
		GLint major, minor;
		glGetIntegerv (GL_MAJOR_VERSION, &major);
		glGetIntegerv (GL_MINOR_VERSION, &minor);
		if ((major < 4) || ((major == 4) && (minor < 1))) {
		    std::cerr << "CS237 requires OpenGL 4.1 or later; found " << major << "." << minor << std::endl;
		    exit (EXIT_FAILURE);
		}
    }



	View* view = new  View(800,600, window);
	
    view->BindFramebuffer ();

	view->InitShaders();
	view->InitProjMatrix();
	view->InitViewMatrix();
	view->InitTriangle();

	glfwSetWindowRefreshCallback (window, Display);
	glfwSetWindowSizeCallback (window, Reshape);
	glfwSetWindowIconifyCallback (window, Visible);
	glfwSetKeyCallback (window, Key);

	while (!glfwWindowShouldClose(window)) {
		if (view->needsRedraw) {
			Display(window);
		}
		glfwWaitEvents ();
	}

	glfwTerminate();

	return EXIT_SUCCESS;
} /* end of main */

