#include  <math.h>
#include <stdio.h>

#include "misc.h"
#include "3dworld.h"
#include "mates.h"
#include "vbo.h"

#define ABS(X) (X<0?-X:X)
#define PI 3.1415927f

#define COS(X) cos(X*PI/180.0f)
#define SIN(X) sin(X*PI/180.0f)

#define   ONE    1.0f

void matriz_unidad(float matrix[4][4]) 
{
// inicializamos a la matriz identidad
    
   matrix[0][0] = ONE;
   matrix[0][1] = 0;
   matrix[0][2] = 0;
   matrix[0][3] = 0;
  
   matrix[1][0] = 0;
   matrix[1][1] = ONE;
   matrix[1][2] = 0;
   matrix[1][3] = 0;
  
   matrix[2][0] = 0;
   matrix[2][1] = 0;
   matrix[2][2] = ONE;
   matrix[2][3] = 0;
  
   matrix[3][0] = 0;
   matrix[3][1] = 0;
   matrix[3][2] = 0;
   matrix[3][3] = ONE;  
}

void translate_matrix (float xt, float yt,float zt,float matrix[4][4])
 {
  
    
   // Creamos una matriz de traslaci¢n que traslada un objetos a una 
   // distancia X de XT, Y de YT, y Z de ZT
   // a partir del origen de pantalla
    
   float mat[4][4];
   float tmat[4][4];
    
   tmat[0][0] = ONE;
   tmat[0][1] = 0;
   tmat[0][2] = 0;
   tmat[0][3] = 0;
  
   tmat[1][0] = 0;
   tmat[1][1] = ONE;
   tmat[1][2] = 0;
   tmat[1][3] = 0;
  
   tmat[2][0] = 0;
   tmat[2][1] = 0;
   tmat[2][2] = ONE;
   tmat[2][3] = 0;
  
   tmat[3][0] = xt ;
   tmat[3][1] = yt;
   tmat[3][2] = zt ;
   tmat[3][3] = ONE;
      
   // Concatenamos    
   multiplicar_matrices (mat, matrix, tmat);  
   copiar_matrices (matrix, mat);  
}

void scale_matrix (float xs, float ys, float zs,float matrix[4][4]) 
{
  
   float mat[4][4];
   float smat[4][4];
    
   // inicializamos la matriz de escalado
    
   smat[0][0] = xs;
   smat[0][1] = 0;
   smat[0][2] = 0;
   smat[0][3] = 0;
  
   smat[1][0] = 0;
   smat[1][1] = ys;
   smat[1][2] = 0;
   smat[1][3] = 0;
  
   smat[2][0] = 0;
   smat[2][1] = 0;
   smat[2][2] = zs;
   smat[2][3] = 0;
  
   smat[3][0] = 0;
   smat[3][1] = 0;
   smat[3][2] = 0;
   smat[3][3] = ONE;
      
   // concatenamos    
   multiplicar_matrices (mat, matrix,smat);  
   copiar_matrices (matrix, mat);  
}

void SwapYZMatrix(float matrix[4][4])
{
 float tmpmat[4][4];
 
 tmpmat[0][0] = matrix [0][0];
 tmpmat[0][1] = matrix [0][2];
 tmpmat[0][2] = matrix [0][1];
 tmpmat[0][3] = matrix [0][3];
 tmpmat[1][0] = matrix [2][0];	
 tmpmat[1][1] = matrix [2][2];
 tmpmat[1][2] = matrix [2][1];
 tmpmat[1][3] = matrix [2][3];	
 tmpmat[2][0] = matrix [1][0];
 tmpmat[2][1] = matrix [1][2];
 tmpmat[2][2] = matrix [1][1];
 tmpmat[2][3] = matrix [1][3];
 tmpmat[3][0] = matrix [3][0];	
 tmpmat[3][1] = matrix [3][2];
 tmpmat[3][2] = matrix [3][1];
 tmpmat[3][3] = matrix [3][3];	
 
 copiar_matrices(matrix,tmpmat);
}

void 
multiplicar_matrices (float result[4][4], float mat1[4][4], float mat2[4][4]) 
{   
   for (int i = 0; i < 4; i++)   
      for (int j = 0; j < 4; j++) {	
         result[i][j] = ((mat1[i][0] * mat2[0][j]) 
                        +(mat1[i][1] * mat2[1][j]) 
                        +(mat1[i][2] * mat2[2][j]) 
                        +(mat1[i][3] * mat2[3][j])) ; 
      }  
}


void copiar_matrices (float dest[4][4], float source[4][4]) 
{      
   for (int i = 0; i < 4; i++)
      for (int j = 0; j < 4; j++)
         dest[i][j] = source[i][j];  
}






void Mult_Vector_Matrix(Vector *v1, float matrix[4][4], Vector *result)
{
 float lx,ly,lz;

 lx = v1->x; 
 ly = v1->y;
 lz = v1->z;

 result->x = ( lx * matrix[0][0] + ly * matrix[1][0] 
                 + lz * matrix[2][0] + matrix[3][0]) ;

 result->y = ( lx * matrix[0][1] + ly * matrix[1][1] 
                 + lz * matrix[2][1] + matrix[3][1]) ;
     
 result->z = ( lx * matrix[0][2] + ly * matrix[1][2] 
                 + lz * matrix[2][2] + matrix[3][2]) ;    
}


void Mult_Vector_Matrix3(Vector *v1, float matrix[4][4], Vector *result)
{
 float lx,ly,lz;

 lx = v1->x; 
 ly = v1->y;
 lz = v1->z;

 result->x = ( lx * matrix[0][0] + ly * matrix[1][0] 
                 + lz * matrix[2][0]) ;

 result->y = ( lx * matrix[0][1] + ly * matrix[1][1] 
                 + lz * matrix[2][1]) ;
     
 result->z = ( lx * matrix[0][2] + ly * matrix[1][2] 
                 + lz * matrix[2][2]) ;    
}

void Mult_Vector4_Matrix(Vector4 *v1, float matrix[4][4], Vector4 *result)
{
 float lx,ly,lz,lw;

 lx = v1->x; 
 ly = v1->y;
 lz = v1->z;
 lw = v1->w;

 result->x = ( lx * matrix[0][0] + ly * matrix[1][0] 
                 + lz * matrix[2][0] + lw*matrix[3][0]) ;

 result->y = ( lx * matrix[0][1] + ly * matrix[1][1] 
                 + lz * matrix[2][1] + lw*matrix[3][1]) ;
     
 result->z = ( lx * matrix[0][2] + ly * matrix[1][2] 
                 + lz * matrix[2][2] + lw*matrix[3][2]) ;    
                 
 result->w = ( lx * matrix[0][3] + ly * matrix[1][3] 
                 + lz * matrix[2][3] + lw*matrix[3][3]) ;         
}


void Transpose(float matrix[4][4])
{
 float temp[4][4];

 for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
       temp[j][i] = matrix[i][j];  
 
 copiar_matrices(matrix,temp);
}



void calcula_normales_poligono(object_type *obj)
{
   float norma;
   Vector V,v1,v2;
   int i;

   // i es el n£mero del pol¡gono
   // IndexPointer[3*i] es el n£mero del primer v‚rtice (3*i+1 es el segundo...)
   // VertexPointer[3*IndexPointer[3*i]] es la X del primer v‚rtice
   // VertexPointer[3*IndexPointer[3*i]+1] es la Y del primer v‚rtice...
   
   for(i=0;i < obj->number_of_polygons;i++)
   {
       v1.x = obj->VertexPointer[3*obj->IndexPointer[3*i]] -
             obj->VertexPointer[3*obj->IndexPointer[3*i+1]];

       v1.y = obj->VertexPointer[3*obj->IndexPointer[3*i]+1] -
             obj->VertexPointer[3*obj->IndexPointer[3*i+1]+1];

       v1.z = obj->VertexPointer[3*obj->IndexPointer[3*i]+2] -
             obj->VertexPointer[3*obj->IndexPointer[3*i+1]+2];

       v2.x = obj->VertexPointer[3*obj->IndexPointer[3*i+2]] -
             obj->VertexPointer[3*obj->IndexPointer[3*i+1]];

       v2.y = obj->VertexPointer[3*obj->IndexPointer[3*i+2]+1] -
             obj->VertexPointer[3*obj->IndexPointer[3*i+1]+1];

       v2.z = obj->VertexPointer[3*obj->IndexPointer[3*i+2]+2] -
             obj->VertexPointer[3*obj->IndexPointer[3*i+1]+2];

       Prod_vec(&v1,&v2,&V);
       Normaliza(&V);

       obj->PolygonNormalPointer[3*i] = V.x;
       obj->PolygonNormalPointer[3*i+1] = V.y;
       obj->PolygonNormalPointer[3*i+2] = V.z;
   }
}

void AssignGLNormals(object_type *obj)
{
 int i,j;
 VertexRemapper *tmp;
 
 for(i=0,j=0;i<obj->number_of_vertices;i++)
 {
  tmp = obj->node[i];
    
  while(tmp!=NULL)
  {
   obj->GLNormalPointer[3*j] = obj->NormalPointer[3*i];
   obj->GLNormalPointer[3*j+1] = obj->NormalPointer[3*i+1];
   obj->GLNormalPointer[3*j+2] = obj->NormalPointer[3*i+2];  
   tmp=tmp->next;
   j++;
  } 	
 }	
}

void calcula_normales_vertice(object_type *obj)
{
   int i,j;
    Vector V;

    for(i=0;i<obj->number_of_vertices;i++)
    {
     // Inicializamos las normales de v‚rtice a 0
      obj->NormalPointer[3*i] = 0.0f;
      obj->NormalPointer[3*i+1] = 0.0f;
      obj->NormalPointer[3*i+2] = 0.0f;
    }
    for(i=0;i<obj->number_of_polygons;i++)
    {
     obj->NormalPointer[3*obj->IndexPointer[3*i]] += obj->PolygonNormalPointer[3*i];
     obj->NormalPointer[3*obj->IndexPointer[3*i]+1] += obj->PolygonNormalPointer[3*i+1];
     obj->NormalPointer[3*obj->IndexPointer[3*i]+2] += obj->PolygonNormalPointer[3*i+2];

     obj->NormalPointer[3*obj->IndexPointer[3*i+1]] += obj->PolygonNormalPointer[3*i];
     obj->NormalPointer[3*obj->IndexPointer[3*i+1]+1] += obj->PolygonNormalPointer[3*i+1];
     obj->NormalPointer[3*obj->IndexPointer[3*i+1]+2] += obj->PolygonNormalPointer[3*i+2];

     obj->NormalPointer[3*obj->IndexPointer[3*i+2]] += obj->PolygonNormalPointer[3*i];
     obj->NormalPointer[3*obj->IndexPointer[3*i+2]+1] += obj->PolygonNormalPointer[3*i+1];
     obj->NormalPointer[3*obj->IndexPointer[3*i+2]+2] += obj->PolygonNormalPointer[3*i+2];
    }
  
      
    for(i=0;i<obj->number_of_vertices;i++)
    {
       V.x=obj->NormalPointer[3*i];
       V.y=obj->NormalPointer[3*i+1];
       V.z=obj->NormalPointer[3*i+2];
       Normaliza(&V);

       obj->NormalPointer[3*i] = V.x;
       obj->NormalPointer[3*i+1] = V.y;
       obj->NormalPointer[3*i+2] = V.z;
    }
    
    // Como ahora tenemos que hay dos versiones de las normales de vértice (real y opengl)
    // llamamos a la función encargada de asignar las de opengl
    if(obj->vbufinfo.usingvertexbuf)
    {
     UpdateBuffers(obj,UPDATE_NORMALS);
    }
    else
    {
     AssignGLNormals(obj);
    }
}

float Prod_esc(Vector *v1,Vector *v2)
{
 return(v1->x*v2->x + v1->y*v2->y + v1->z*v2->z);
}

void Prod_vec(Vector *v1, Vector *v2, Vector *result)
{
 result->x = v1->y*v2->z - v1->z*v2->y;
 result->y = v1->z*v2->x - v1->x*v2->z;
 result->z = v1->x*v2->y - v1->y*v2->x;
}

void Prod_esc_vector(float escalar,Vector *v)
{
 v->x = escalar * v->x;
 v->y = escalar * v->y;
 v->z = escalar * v->z;
}

float Norma(Vector *v1)
{
 return(sqrt(v1->x*v1->x + v1->y*v1->y + v1->z*v1->z));
}

void Normaliza(Vector *v1)
{
 float norma=Norma(v1);

 if (fabs(norma)>0.0000001)
 {
     v1->x /= norma;
     v1->y /= norma;
     v1->z /= norma;
  }
 else
 {
     v1->x = 0.0f;
     v1->y = 0.0f;
     v1->z = 0.0f;
 }
}


void Invert_Matrix(float m[4][4])
{
  int i,j,k;               
  int pvt_i[4], pvt_j[4];            /* Locations of pivot elements */
  float pvt_val;               /* Value of current pivot element */
  float hold;                  /* Temporary storage */
  float determinat;            

  determinat = 1.0f;
  for (k=0; k<4; k++)  {
    /* Locate k'th pivot element */
    pvt_val=m[k][k];            /* Initialize for search */
    pvt_i[k]=k;
    pvt_j[k]=k;
    for (i=k; i<4; i++) {
      for (j=k; j<4; j++) {
        if (fabs(m[i][j]) > fabs(pvt_val)) {
          pvt_i[k]=i;
          pvt_j[k]=j;
          pvt_val=m[i][j];
        }
      }
    }

    /* Product of pivots, gives determinant when finished */
    determinat*=pvt_val;
    if (fabs(determinat)<0.000000000001f) {    
    	printf("Ooops\n");
      return;  /* Matrix is singular (zero determinant) */
    }

    /* "Interchange" rows (with sign change stuff) */
    i=pvt_i[k];
    if (i!=k) {               /* If rows are different */
      for (j=0; j<4; j++) {
        hold=-m[k][j];
        m[k][j]=m[i][j];
        m[i][j]=hold;
      }
    }

    /* "Interchange" columns */
    j=pvt_j[k];
    if (j!=k) {              /* If columns are different */
      for (i=0; i<4; i++) {
        hold=-m[i][k];
        m[i][k]=m[i][j];
        m[i][j]=hold;
      }
    }
    
    /* Divide column by minus pivot value */
    for (i=0; i<4; i++) {
      if (i!=k) m[i][k]/=( -pvt_val) ; 
    }

    /* Reduce the matrix */
    for (i=0; i<4; i++) {
      hold = m[i][k];
      for (j=0; j<4; j++) {
        if (i!=k && j!=k) m[i][j]+=hold*m[k][j];
      }
    }

    /* Divide row by pivot */
    for (j=0; j<4; j++) {
      if (j!=k) m[k][j]/=pvt_val;
    }

    /* Replace pivot by reciprocal (at last we can touch it). */
    m[k][k] = 1.0f/pvt_val;
  }

  /* That was most of the work, one final pass of row/column interchange */
  /* to finish */
  for (k=4-2; k>=0; k--) { /* Don't need to work with 1 by 1 corner*/
    i=pvt_j[k];            /* Rows to swap correspond to pivot COLUMN */
    if (i!=k) {            /* If rows are different */
      for(j=0; j<4; j++) {
        hold = m[k][j];
        m[k][j]=-m[i][j];
        m[i][j]=hold;
      }
    }

    j=pvt_i[k];           /* Columns to swap correspond to pivot ROW */
    if (j!=k)             /* If columns are different */
    for (i=0; i<4; i++) {
      hold=m[i][k];
      m[i][k]=-m[i][j];
      m[i][j]=hold;
    }
  }
  	
}



void CalcPlane(Point v[3], Plane *plane)
{
    int i;
    
    plane->a = v[0].y*(v[1].z-v[2].z) + v[1].y*(v[2].z-v[0].z) + v[2].y*(v[0].z-v[1].z);
    plane->b = v[0].z*(v[1].x-v[2].x) + v[1].z*(v[2].x-v[0].x) + v[2].z*(v[0].x-v[1].x);
    plane->c = v[0].x*(v[1].y-v[2].y) + v[1].x*(v[2].y-v[0].y) + v[2].x*(v[0].y-v[1].y);
    plane->d =-( v[0].x*(v[1].y*v[2].z - v[2].y*v[1].z) +
                         v[1].x*(v[2].y*v[0].z - v[0].y*v[2].z) +
                         v[2].x*(v[0].y*v[1].z - v[1].y*v[0].z) );
}
