#include "Globals.h"

bool registerOpenGLTexture(unsigned char *textureData,
                           unsigned int texWidth, unsigned int texHeight,
                           GLuint &textureHandle) {

    if( textureData == 0 ) {
        fprintf(stderr,"Cannot register texture; no data specified.\n");
        return false;
    } 


    glEnable( GL_TEXTURE_2D );
    glGenTextures( grassTexHandle, &textureHandle );
    glBindTexture( GL_TEXTURE_2D, grassTexHandle );
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
    return true;
}
void setupTextures() {

    grassTexHandle =
        SOIL_load_OGL_texture (
            "textures/grass.png",
            SOIL_LOAD_AUTO,
            SOIL_CREATE_NEW_ID,
            SOIL_FLAG_MIPMAPS
                | SOIL_FLAG_INVERT_Y
                | SOIL_FLAG_COMPRESS_TO_DXT
        );
	skyTexHandle =
        SOIL_load_OGL_texture (
            "textures/sky.jpg",
            SOIL_LOAD_AUTO,
            SOIL_CREATE_NEW_ID,
            SOIL_FLAG_MIPMAPS
                | SOIL_FLAG_INVERT_Y
                | SOIL_FLAG_COMPRESS_TO_DXT
        );
    // TODO #6: Register non-PPM

    registerOpenGLTexture( imageData, textureWidth, textureHeight, grassTexHandle );
	registerOpenGLTexture( skyData, skyWidth, skyHeight, skyTexHandle );

}