#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#endif
#include <GL/gl.h>

static int mstrcmp( char *str1, char *str2 )
{
    do
    {
        if( *str1 != *str2 ) return( 0 );
    }while( *++str1 && *++str2 );
    return( 1 );
}

static int esta( char *str1, char *str2 )
{
    do
    {
        if( mstrcmp(str1,str2) ) return( 1 );
    }
    while( *++str1 );
    return( 0 );
}

int EstaExtension( char *exten )
{
    char *str;
    str = (char*)glGetString( GL_EXTENSIONS );
    return( esta( str, exten ) );
}
