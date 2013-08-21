#ifndef _PORTADA_H_
#define _PORTADA_H_

void  *PORTADA_New( void );
void  PORTADA_Delete( void *me );
void  PORTADA_DoFrame( void *me, float t );

void PORTADA_SetMode( void *vme, int mode );
void PORTADA_SetFreq( void *vme, float freq );

#endif
