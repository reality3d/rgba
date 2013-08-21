#ifndef _SPIRAL_H_
#define _SPIRAL_H_

void  *SPIRAL_New( void );
void  SPIRAL_Delete( void *me );
void  SPIRAL_DoFrame( void *me, float t );

void SPIRAL_SetSpeed( void *vme, float speed );

void SPIRAL_ChangeColor1( void *vme, float r, float g, float b, float a, float time );
void SPIRAL_ChangeColor2( void *vme, float r, float g, float b, float a, float time );

void SPIRAL_ChangeWidth1( void *vme, float w, float time );
void SPIRAL_ChangeWidth2( void *vme, float w, float time );

void SPIRAL_ChangeTable( void *vme, int ta, float time );

#endif
