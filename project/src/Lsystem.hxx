/*! \file Lsystem.hxx
 *
 * These classes support drawing directional vectors and axes as lines to help with
 * debugging.
 *
 * \author Edouard Brooks
 */
 
#ifndef _LSYSTEM_HXX_
#define _LSYSTEM_HXX_

#include 	"cs237.hxx"
#include 	<vector>
#include 	<map>
using 		namespace std; 
using 		namespace cs237; 

struct StochasticProduct {
	float 		probability;
	string 		product;

	StochasticProduct (
		float 	prob,
		string 	prod) {
		this->probability = prob;
		this->product = prod;
	}
};

//! a class for drawing the XYZ axes
class LSystem {
	public:
		LSystem (string axiom, int length, float ang);
		~LSystem ();

		void 	UpdateVBO ();
	
		void 	tPush ();
		void 	tPop ();

		void 	DevelopAxiom ();
		void 	DerivationStep (string str);

		string 	GrabDerivationM (int m);

		void 	PullShape ();

		void 	Draw ();

		int 	NumDerivations 	() const { return this->n; }
		string 	Axiom 			() const { return this->axiom; }

	private:
		/* LINDENMAYER SYSTEM:
		 * 		- axiom,
		 * 		- number of derivations,
		 * 		- all stored derivations,
		 * 		- angle (in degrees).
		 * 		- production rules
		 */
		string										axiom;
		int 										n;
		vector<string> 								derivations;
		float 										angle;
		multimap<char, StochasticProduct*>	productions;

		/* VISUAL SYSTEM:
		 * 		- turtle's state,
		 * 		- stack set up for turtle position,
		 * 		- VAO id,
		 * 		- vector for all lines,
		 * 		- vector of line colors.
		 */
		mat4f 					tState;
		vector<mat4f>			tStateStack;

		quatf 					tOrientation;
		vector<quatf> 			tOrientationStack;

		GLuint 					_vaoId;
		GLuint 					buffers[2];
		vector<vec3f> 			_verts;
		vector<vec3f> 			_colors;

		/* NOTES:
		 	- Dictionary would be a good idea for the production rules.
		 	- Check CS237 shape creations to make the flowers.
		 	- Have a vector of transformation matrices to simulate a tree.
		 	- Add capability to request new derivation length.
		 */
};

#endif // !_LSYSTEM_HXX_