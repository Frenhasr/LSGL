 /*! \file cube.cxx
 *
 * \brief this file contains function definitions for creating and rendering a triangle
 *
 * \author Lamont Samuels 
 */ 

#include "triangle.hxx" 

void Triangle::Init() {
	// Generate VAO for mesh
	CS237_CHECK(glGenVertexArrays(1, &vaoId));
	CS237_CHECK(glBindVertexArray(vaoId));

	// Generate name for VBO and bind
	CS237_CHECK(glGenBuffers(1, &vbufId));
	CS237_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vbufId));
	
	CS237_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));
}


void Triangle::Render() {
	CS237_CHECK(glBindVertexArray(vaoId));

	CS237_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vbufId));

	CS237_CHECK(glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), ((GLvoid*)0)));
	CS237_CHECK(glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), (GLvoid*)sizeof(vertices[0].pos)));
	
	CS237_CHECK(glEnableVertexAttribArray(posLoc));
	CS237_CHECK(glEnableVertexAttribArray(colorLoc));

	CS237_CHECK(glDrawArrays(GL_TRIANGLES, 0, 3));
}
