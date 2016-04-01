/*! \file LSystem.cxx
 *
 * \author Edouard Brooks
 */

#include "Lsystem.hxx"

#define DEBUG_FLAG       0

/* INITIALIZE LSYSTEM
 */
LSystem::LSystem (
	string 	axiom,
	int 	length,
	float 	ang
	) {

	this->axiom 	= axiom;
	this->n 		= length;
	this->angle 	= ang;

	// this->productions['X'] = "XX-X-X-X-X-X+X";

	// this->productions['X'] = "X+Y+";
	// this->productions['Y'] = "-X-Y";

	// this->productions['A'] = "[&FLA];;;;;[&FLA];;;;;;;[&FLA]";
	// this->productions['F'] = "S;;;;;F";
	// this->productions['S'] = "FL";

	// this->productions['F'] = "F[+F]F[-F]F";
	// vector<StochasticProduct*> r;
	// r.push_back(new StochasticProduct(1, "F[+F]F[-F]F"));

	this->productions.insert ( pair<char, StochasticProduct*>('F', new StochasticProduct(0.5f, "&F[+F]F[-;F]F")) );
	this->productions.insert ( pair<char, StochasticProduct*>('F', new StochasticProduct(0.5f, "F[+F&&+F]FF")) );

	derivations.push_back(this->axiom);
	this->DevelopAxiom();

	this->PullShape();

	CS237_CHECK( glGenVertexArrays (1, &this->_vaoId) );
	CS237_CHECK( glBindVertexArray (this->_vaoId) );

	CS237_CHECK( glGenBuffers (2, this->buffers) );

	// intialize the vertex buffer
	CS237_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
    CS237_CHECK( glBindBuffer (GL_ARRAY_BUFFER, this->buffers[0]) );
    CS237_CHECK( glBufferData (GL_ARRAY_BUFFER, sizeof(vec3f) * this->_verts.size(), &(this->_verts)[0], GL_DYNAMIC_DRAW) );
    CS237_CHECK( glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3f), 0) );
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
    this->PullShape();

	CS237_CHECK( glBindVertexArray (this->_vaoId) );

	CS237_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
	CS237_CHECK( glBindBuffer (GL_ARRAY_BUFFER, this->buffers[0]) );
	CS237_CHECK( glBufferSubData (GL_ARRAY_BUFFER, 0, sizeof(vec3f) * this->_verts.size(), &(this->_verts)[0]) );

	CS237_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

/* PUSH & POP
 * Handles the turtle's position matrices.
 */
void LSystem::tPush () {
	this->tStateStack.push_back(this->tState);
	this->tOrientationStack.push_back(this->tOrientation);
}

void LSystem::tPop () {
	this->tState = this->tStateStack.back();
	this->tStateStack.pop_back();

	this->tOrientation = this->tOrientationStack.back();
	this->tOrientationStack.pop_back();
}

/* DEVELOP AXIOM
 * 		Inputs: axiom (string), length (int).
 * 		Output: none.
 * Takes an axiom and derives it [length] times.
 */
void LSystem::DevelopAxiom () {
	if (DEBUG_FLAG)
		std::cout << "Developing Axiom" << std::endl;

	int i;
	for (i = 0; i < this->n; i++) {
		this->DerivationStep(this->derivations.back());
	}
}

void LSystem::DerivationStep (string str) {
	srand (time(NULL));
	string temp = str;
	char ch;
	float samp, distributionLoc, p;
	string swap;

	for (int i = 0; i < temp.length(); i++) {
		ch = temp.at(i);

		samp = ((float) rand() / (RAND_MAX));
		distributionLoc = 0.0f;

		for (auto it = productions.equal_range(ch).first; it != productions.equal_range(ch).second;  ++it) {

			p = it->second->probability;
			distributionLoc += p;

			cout << "Current r " << samp << " under " << distributionLoc << endl;

			if (samp <= distributionLoc) {
				swap = it->second->product;

				// if (DEBUG_FLAG)
					cout << "Accepted" << swap << endl;

				temp.replace(i, 1, swap);
				i += (swap).length() - 1;

				break;
			}
		}
	}

	this->derivations.push_back(temp);
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
	bool justDrew = false;

	this->_verts.clear();
	this->_colors.clear();

	this->tStateStack.clear();
	this->tOrientationStack.clear();

	this->tState = scale(1.0f);
	this->tOrientation = quatf(0.0, vec3f(0.0f, 0.0f, 0.0f));

	this->tPush();

	/* NOTES:
		- pop push.  *** DONE
		- Secondary dictionary of output rules.
	 */
	char 		letter;
	string 		der = this->derivations.back();
	vec3f startP;
	vec4f endP;
	vec4f offsetL = vec4f(1.0f, 0.0f, 0.0f, 1.0f);

	for (int i = 0; i < der.length(); i++) {
		letter = der.at(i);

		if (DEBUG_FLAG)
			cout << letter << endl;

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
				// if (justDrew) {
				// 	this->tOrientation = this->tOrientationStack.back();
				// 	this->tState = this->tStateStack.back();

				// 	justDrew = !justDrew;
				// }
				this->tOrientation = this->tOrientation * quatf(this->angle, vec3f(0.0f, 0.0f, 1.0f));
				break;
			case '-':
				this->tOrientation = this->tOrientation * quatf(-this->angle, vec3f(0.0f, 0.0f, 1.0f));
				break;

			case '&':
				this->tOrientation = this->tOrientation * quatf(this->angle, vec3f(0.0f, 1.0f, 0.0f));
				break;
			case '^':
				this->tOrientation = this->tOrientation * quatf(-this->angle, vec3f(0.0f, 1.0f, 0.0f));
				break;

			case ':':
				this->tOrientation = this->tOrientation * quatf(this->angle, vec3f(1.0f, 0.0f, 0.0f));
				break;
			case ';':
				this->tOrientation = this->tOrientation * quatf(-this->angle, vec3f(1.0f, 0.0f, 0.0f));
				break;

			case '|':
				this->tOrientation = this->tOrientation * quatf(180.0f, vec3f(0.0f, 0.0f, 1.0f));
				break;

			default: {
				// Note: Assume that any other character is a line character. Will have to adjust length, etc.
				// Warning: This assumes that there are only characters from the alphabet.

				// if (this->_verts.size() != 0)
				// 	startP = this->_verts.back();
				// else
				startP = vec3f(this->tState * vec4f(0.0f, 0.0f, 0.0f, 1.0f));
				this->_verts.push_back( startP );
				// End point.
				endP =  this->tState * this->tOrientation.toMat4x4() * offsetL;
				this->_verts.push_back( vec3f(endP) );

				this->_colors.push_back(vec3f(0.0f, 0.0f, 0.0f));
				this->_colors.push_back(vec3f(0.0f, 0.0f, 0.0f));

				// Change the state to the new turtle position.
				this->tState = translate( vec3f(endP) );

				justDrew = !justDrew;
				break;
			}
		}
	}
}

/* DRAW
 */
void LSystem::Draw () {

	this->UpdateVBO();
	for (auto it = this->_verts.begin(); it != this->_verts.end();  ++it) {
		*it = 3.0f * (*it);

		if (DEBUG_FLAG)
			cout << *it << endl;
	}

	if (DEBUG_FLAG)
		std::cout << this->derivations.back() << std::endl;

	CS237_CHECK( glBindVertexArray (this->_vaoId) );
	CS237_CHECK( glDrawArrays (GL_LINES, 0, this->_verts.size()) );
}