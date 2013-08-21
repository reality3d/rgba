#include <string.h> // memset()

static int count[256];
static int indice[256];

static void radix( int byte, int N, unsigned int *source, unsigned int *dest )
{
    int i;
    int b, c, d, e;
    int k, k1;

    memset( count, 0, sizeof(count) );

    for( i=0; i<N; i++ )
        {
        b = source[i*2+1];
        c = byte<<3;
        d = b>>c&0xff;
        count[d]++;
        }

    indice[0]=0;

    for( i=1; i<256; i++ )
        indice[i]=indice[i-1]+count[i-1];

    for( i=0; i<N; i++ )
        {
        b  = source[i*2+1];
        c  = byte*8;
        e  = b>>c&0xff;
        d  = indice[e]++;
        k  = i<<1;
        k1 = d<<1;
        dest[k1+0]=source[k+0];
        dest[k1+1]=source[k+1];
        }
}

void radixsort( unsigned int *source, unsigned int *temp, int n )
{
    radix( 0, n, source, temp );
    radix( 1, n, temp, source );
    radix( 2, n, source, temp );
    radix( 3, n, temp, source );
}
