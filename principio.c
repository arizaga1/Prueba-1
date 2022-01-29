#include "mbed.h"
//#include "TSISensor.h"
#include "ultrasonic.h"
#include "math.h"
  static int distance;
    int setpoint;

Ticker actualiza;
Serial pc(USBTX, USBRX); 
DigitalOut led1(LED1);
ultrasonic mu(PTC8, PTC9, .1, 1); 
float fmaxf(float x, float y);
float fminf(float x, float y);
//#LOS DATOS ESTAN NORMALIZADOS ENTRE -1 Y 1
#define n 5     //filas
#define m 4     //COLUMNAS
float conjunto[n][m]={{0.0,0.1,0.2,0.3},{0.1,0.2,0.3,0.4},{0.3,0.4,0.5,0.7},{0.4,0.5,0.6,0.7},{0.6,0.7,0.8,1.0}};
int inference[n][n]={{1, 3,  6,  6,  7}, {1, 3,  4,  6,  7}, {1, 3,  4,  6,  7}, {1, 3,  4,  1,  5}, {1, 1,  1,  1,  1}};
float salida[7]={};
DigitalOut myled(LED1);
//TSISensor tsi;
float* t;
float* dt;
float error1, derror1 ;
float* fusificar(float (*conjunto)[m], float x);
void mynow()
{
    int i;
    int j;
    int indice;
    float anterior;
    anterior=error1;

        pc.printf("distancia: %d \n\r", distance);
        error1=  float(setpoint- distance );
        derror1 = (error1-anterior)/0.5;
        t = fusificar( conjunto, error1 );
        dt = fusificar( conjunto,  derror1);
        for(i=0; i< 5 ; i++ )
         {
           for(j=0; j < 5 ; j++ )
            {
                indice = inference[i][j]; 
            salida[indice]= fmaxf(salida[indice],fminf(t[i],dt[j]));
            }
        }

 //    pc.printf("+++++++++ \n\r");
 //   for(j=0; j< n ; j++ )
  //      {    
   // pc.printf(" %f \n\r",t[j]);
    //    }
    }


int main() 
{
    pc.baud(115200);    
    pc.printf("Iniciando.... \n\r");
    wait(0.1);
    mu.startUpdates();//start measuring the distance
    //actualiza.attach(&mynow, 0.5);
    while(1) 
    {    
            mu.checkDistance();
        distance=mu.getCurrentDistance();
        
        pc.printf("$%d ;", distance);
        led1 = !led1;
        wait(0.1);
    }
}



float* fusificar(float (*conjuntos)[m], float x)
{

static float  resultado[n];
int i;



for(i=0; i< n ; i++ )
{    
  
        if (x <= conjuntos[i][0] && conjuntos[i][3] >= x)  //Valor fuera del conjunto
               { resultado[i]= 0;} 
                
        if (x > conjuntos[i][0] && conjuntos[i][1] > x)    //Valor en alguna de las pendientes de conjunto 
                { resultado[i] = (x - conjuntos[i][0])* (1/(conjuntos[i][1]-conjuntos[i][0]));}
               
        if (x > conjuntos[i][1] && conjuntos[i][2] > x)    //Valor en alguna de las pendientes de conjunto 
                {resultado[i]= 1;}
        if (x > conjuntos[i][2] && conjuntos[i][3] > x)    //Valor en alguna de las pendientes de conjunto 
                {resultado[i] = abs(x - conjuntos[i][3])*(1/(conjuntos[i][3]-conjuntos[i][2]));}
        
}

return(resultado);//(float*)
    
}

float fminf(float x, float y){
    /// Returns minimum of x, y ///
    return (((x)<(y))?(x):(y));
    }
    
    float fmaxf(float x, float y){
    /// Returns maximum of x, y ///
    return (((x)>(y))?(x):(y));
    }
