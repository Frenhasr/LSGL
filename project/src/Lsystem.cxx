/*! \file LSystem.cxx
 *
 * \author Edouard Brooks
 */

#include "Lsystem.hxx"


/* INITIALIZE LSYSTEM
 */
LSystem::LSystem (
	string 	axiom,
	int 	length
	) {

	this->axiom 	= axiom;
	this->n 		= length;
	this->angle 	= 90;
	// derivations 	= new vector<string> ();
	// turtleTMat	 	= new vector<cs237::mat4f> ();

	this->tState = translate(vec3f(0.0f, 0.0f, 0.0f));
	this->tPush();

	derivations.push_back(this->axiom);
	this->DevelopAxiom();

	this->_verts.clear();
	this->_colors.clear();
	this->PullShape();

	CS237_CHECK( glGenVertexArrays (1, &this->_vaoId) );
	CS237_CHECK( glBindVertexArray (this->_vaoId) );

	CS237_CHECK( glGenBuffers (2, this->buffers) );

	// intialize the vertex buffer
	CS237_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
    CS237_CHECK( glBindBuffer (GL_ARRAY_BUFFER, this->buffers[0]) );
    CS237_CHECK( glBufferData (GL_ARRAY_BUFFER, sizeof(vec3d) * this->_verts.size(), &(this->_verts)[0], GL_DYNAMIC_DRAW) );
    CS237_CHECK( glVertexAttribPointer (0, 3, GL_DOUBLE, GL_FALSE, sizeof(vec3d), 0) );
    CS237_CHECK( glEnableVertexAttribArray (0) );

	// initialize the color buffer
	CS237_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
    CS237_CHECK( glBindBuffer (GL_ARRAY_BUFFER, this->buffers[1]) );
    CS237_CHECK( glBufferData (GL_ARRAY_BUFFER, sizeof(vec3f) * this->_colors.size(), &(this->_colors)[0], GL_DYNAMIC_DRAW) );
    CS237_CHECK( glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, sizeof(vec3f), 0) );
    CS237_CHECK( glEnableVertexAttribArray (1) );

    CS237_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

LSystem::~LSystem () {
	CS237_CHECK( glDeleteVertexArrays (1, &this->_vaoId) );
}

void LSystem::UpdateVBO ()
{

	this->_verts.clear();
	this->_colors.clear();
    this->PullShape();

	CS237_CHECK( glBindVertexArray (this->_vaoId) );

	CS237_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
	CS237_CHECK( glBindBuffer (GL_ARRAY_BUFFER, this->buffers[0]) );
	CS237_CHECK( glBufferSubData (GL_ARRAY_BUFFER, 0, sizeof(vec3d) * this->_verts.size(), &(this->_verts)[0]) );

	CS237_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

/* PUSH & POP
 * Handles the turtle's position matrices.
 */
void LSystem::tPush () {
	this->tStateStack.push_back(this->tState);
}

void LSystem::tPop () {
	this->tState = this->tStateStack.back();
	this->tStateStack.pop_back();
}

/* DEVELOP AXIOM
 * 		Inputs: axiom (string), length (int).
 * 		Output: none.
 * Takes an axiom and derives it [length] times.
 */
void LSystem::DevelopAxiom () {
	std::cout << "Developing Axiom" << std::endl;
	int i;
	for (i = 0; i < this->n; i++) {
		this->DerivationStep(this->derivations.back());
	}
}

void LSystem::DerivationStep (string str) {
	string temp = str;
	//std::cout << "Entering Derivation Step with " << temp << " ";
	char ch;

	for (int i = 0; i < temp.length(); i++) {
		ch = temp.at(i);

		/* NOTES:
			- Replace with dictionary of production rules.
		 */
		switch (ch) {
			case 'X':
				temp.replace(i, 1, "X+Y+");
				i += strlen("X+Y+") - 1;
				break;

			case 'Y':
				temp.replace(i, 1, "-X-Y");
				i += strlen("-X-Y") - 1;
				// if ((float)rand() < 0.4) {
				// 	temp.replace(i, 1, "D[+XV]D[-XV]+X");
				// 	i += strlen("D[+XV]D[-XV]+X") - 1;
				// }
				// else {
				// 	temp.replace(i, 1, "D[-XV]D[+XV]-X");
				// 	i += strlen("D[-XV]D[+XV]-X") - 1;
				// }
				break;

			default:
				break;
		}
	}

	this->derivations.push_back(temp);

	//std::cout << "exiting with " << temp << std::endl;
}

/* GRAB Mth DERIVATION
 * 		Input: none.
 * 		Output: mth derivation.
 */
string LSystem::GrabDerivationM (
	int m
	) {
	if (m < 0) {
		return this->axiom;
	}
	else if (m <= this->n) {
		return this->derivations.at(m);
	}
	else {
		// NOTE: Fix this to be more efficient.
		this->n = m;
		this->DevelopAxiom();

		return this->derivations.back();
	}
}

/* PULL SHAPE
 * 		Input: none.
 * 		Output: none.
 * Retrieves the final derivation's vertices and colors.
 */
void LSystem::PullShape () {

	/* NOTES:
		- pop push.  *** DONE
		- Secondary dictionary of output rules.
	 */
	char 		letter;
	string 		der = this->derivations.back();

	for (int i = 0; i < der.length(); i++) {
		letter = der.at(i);

		switch (letter) {
			/* STACK OPERATIONS
			 * 		[ 	-> 	push current turtle state
			 * 		] 	-> 	pop a state and make it current turtle state
			 */
			case '[':
				tPush();
				break;
			case ']':
				tPop();
				break;

			/* TURTLE DRAWS
			 * 		D 	-> 	line
			 * 		X 	-> 	line
			 * 		V 	-> 	leaf
			 */
			case 'X':
			case 'Y':
				// Note: The current rotation is included, and would not affect the starting point.
				// Start point.
				this->_verts.push_back(vec3f(this->tState * vec4f(0.0f, 0.0f, 0.0f, 1.0f)));
				// End point.
				this->_verts.push_back(vec3f(this->tState * 2.0f * normalize(vec4f(1.0f, 1.0f, 1.0f, 1.0f))));

				this->_colors.push_back(vec3f(0.0f, 0.0f, 0.0f));
				this->_colors.push_back(vec3f(0.0f, 0.0f, 0.0f));

				// Change the state to the new turtle position.
				this->tState = this->tState * translate(2.0f * normalize(vec3f(1.0f, 1.0f, 1.0f)));
				break;
			// case 'V':
			// 	// Note: The current rotation is included, and would not affect the starting point.
			// 	// Start point.
			// 	this->_verts.push_back(vec3f(this->tState * vec4f(0.0f, 0.0f, 0.0f, 1.0f)));
			// 	// End point.
			// 	this->_verts.push_back(vec3f(this->tState * normalize(vec4f(1.0f, 1.0f, 1.0f, 1.0f))));
				
			// 	this->_colors.push_back(vec3f(1.0f, 0.0f, 0.0f));
			// 	this->_colors.push_back(vec3f(1.0f, 0.0f, 0.0f));

			// 	this->tState *= translate(normalize(vec3f(1.0f, 1.0f, 1.0f)));
			// 	break;

			/* TURTLE ORIENTATION:
			 *		+ 	-> 	turn left
			 * 		- 	-> 	turn right
			 * 		& 	-> 	pitch down
			 * 		^ 	-> 	pitch up
			 * 		: 	-> 	roll left
			 * 		; 	-> 	roll right
			 * 		| 	-> 	turn around
			 */
			case '+':
				this->tState = this->tState * cs237::quatf(this->angle, cs237::vec3f(0.0f, 0.0f, 1.0f)).toMat4x4();
				break;
			case '-':
				this->tState = this->tState * cs237::quatf(-this->angle, cs237::vec3f(0.0f, 0.0f, 1.0f)).toMat4x4();
				break;

			case '&':
				this->tState = this->tState * cs237::quatf(this->angle, cs237::vec3f(0.0f, 1.0f, 0.0f)).toMat4x4();
				break;
			case '^':
				this->tState = this->tState * cs237::quatf(-this->angle, cs237::vec3f(0.0f, 1.0f, 0.0f)).toMat4x4();
				break;

			case ':':
				this->tState = this->tState * cs237::quatf(this->angle, cs237::vec3f(1.0f, 0.0f, 0.0f)).toMat4x4();
				break;
			case ';':
				this->tState = this->tState * cs237::quatf(-this->angle, cs237::vec3f(1.0f, 0.0f, 0.0f)).toMat4x4();
				break;

			case '|':
				this->tState = this->tState * cs237::quatf(180.0f, cs237::vec3f(0.0f, 0.0f, 1.0f)).toMat4x4();
				break;

			default:
				break;
		}
	}
}

/* DRAW
 */
void LSystem::Draw () {
	std::cout << this->derivations.back() << std::endl;
	CS237_CHECK( glBindVertexArray (this->_vaoId) );
	CS237_CHECK( glDrawArrays (GL_LINES, 0, this->_verts.size()) );
}