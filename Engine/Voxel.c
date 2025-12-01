#include "Voxel.h"
#include <GL/gl.h>

void CreateVoxel(int x, int y, int z, int size) {
    float s = size * 0.5f;

    glPushMatrix();
    glTranslatef(x, y, z);

    glBegin(GL_QUADS);

    // Front
    glVertex3f(-s, -s,  s);
    glVertex3f( s, -s,  s);
    glVertex3f( s,  s,  s);
    glVertex3f(-s,  s,  s);

    // Back
    glVertex3f( s, -s, -s);
    glVertex3f(-s, -s, -s);
    glVertex3f(-s,  s, -s);
    glVertex3f( s,  s, -s);

    // Right
    glVertex3f( s, -s,  s);
    glVertex3f( s, -s, -s);
    glVertex3f( s,  s, -s);
    glVertex3f( s,  s,  s);

    // Left
    glVertex3f(-s, -s, -s);
    glVertex3f(-s, -s,  s);
    glVertex3f(-s,  s,  s);
    glVertex3f(-s,  s, -s);

    // Top
    glVertex3f(-s,  s,  s);
    glVertex3f( s,  s,  s);
    glVertex3f( s,  s, -s);
    glVertex3f(-s,  s, -s);

    // Bottom
    glVertex3f(-s, -s, -s);
    glVertex3f( s, -s, -s);
    glVertex3f( s, -s,  s);
    glVertex3f(-s, -s,  s);

    glEnd();
    glPopMatrix();
}
