/*--------------------------------------------------------------------------
 	Practica: 2.a 															
 																			
 	Autores:																
       - Francisco Arjona Lopez											
       - Cristobal Castro Villegas										
       - Jose Antonio Espino Palomares                                    
       - Antonio Osuna Caballero                                          
--------------------------------------------------------------------------*/
#include "kernel.h"
#include "kernel_id.h"
#include "ecrobot_interface.h"

#define PULSADOR_CHOQUE NXT_PORT_S1
#define PULSADOR_FIN NXT_PORT_S2
#define SENSOR_ULTRASONIDOS NXT_PORT_S4


/*------------------------------------------------------------------------*/
/* OSEK declarations                                                      */
/*------------------------------------------------------------------------*/
DeclareTask(Giro);
DeclareTask(Principal);
DeclareTask(Pulsador);
DeclareCounter(Contador);
DeclareAlarm(alarm1);


/*------------------------------------------------------------------------*/
/* Definitions                                                            */
/*------------------------------------------------------------------------*/
int fin = 0; //indica el fin de la ejecucion
int pulsador_choque=0;
int OriginalSpeedB = 50, speedB = 50;
int OriginalSpeedC = 50, speedC = 50;

void ecrobot_device_initialize()
{
	// Inicializar los sensores activos
	ecrobot_init_sonar_sensor(SENSOR_ULTRASONIDOS);
}

void ecrobot_device_terminate()
{
	// Finalizar los sensores activos
	ecrobot_term_sonar_sensor(SENSOR_ULTRASONIDOS);
}


/*------------------------------------------------------------------------*/
/* Function to be invoked from a category 2 interrupt                     */
/*------------------------------------------------------------------------*/
void user_1ms_isr_type2(){

	SignalCounter(Contador); //pone en marcha el contador
}

/*------------------------------------------------------------------------*/
// Task information:                                                      
// -----------------                                                      
// Name    : Giro                                                       
// Priority: 2                                                            
// Typ     : EXTENDED TASK                                                
// Schedule: FULL														  
// Objective: Girar a la derecha y a la izquierda para comprobar la 
//				distancia y elegir la mejor opcion                              
/*------------------------------------------------------------------------*/
TASK(Giro)
{
		
	//Inicializamos para medir la distancia a uno y otro lado
	int distanciaDcha = 0, distanciaIzq = 0, media=0;
	int motorB = 1, motorC = 1;
	int i;
	
	/* Comprobamos qué distancia hay al girar a la derecha */    
	
	// Inicializamos contadores
	nxt_motor_set_count(NXT_PORT_B,0);
	nxt_motor_set_count(NXT_PORT_C,0);
		
	// Activamos motores para girar a la dcha
	nxt_motor_set_speed(NXT_PORT_B, -40, 1);
	nxt_motor_set_speed(NXT_PORT_C, 40, 1);
	
	while (motorB || motorC){
		if(nxt_motor_get_count(NXT_PORT_B) <= -180){
			nxt_motor_set_speed(NXT_PORT_B, 0, 1);
			motorB =0;
		}
		
		if(nxt_motor_get_count(NXT_PORT_C) >= 180){
			nxt_motor_set_speed(NXT_PORT_C, 0, 1);
			motorC =0;
		}
		
	}
	
	// Hacemos una pausa para comenzar a realizar las mediciones
	systick_wait_ms(500);

	// Obtenemos el valor de la distancia en la dirección derecha	
	for(i=0; i<10;i++){
		media += ecrobot_get_sonar_sensor(SENSOR_ULTRASONIDOS);
		systick_wait_ms(100);
	}
	distanciaDcha = media/10;


	
	
	/* Comprobamos ahora la distancia a la izquierda */
	nxt_motor_set_count(NXT_PORT_B, 0);
	nxt_motor_set_count(NXT_PORT_C, 0);
		
	// Activamos motores para girar a la izq
	nxt_motor_set_speed(NXT_PORT_B, 40, 1);
	nxt_motor_set_speed(NXT_PORT_C, -40, 1);
	
	motorB = motorC = 1;
	while (motorB || motorC){
		if(nxt_motor_get_count(NXT_PORT_B) >= 360){
			nxt_motor_set_speed(NXT_PORT_B, 0, 1);
			motorB=0;
		}
		
		if(nxt_motor_get_count(NXT_PORT_C) <= -360){
			nxt_motor_set_speed(NXT_PORT_C, 0, 1);
			motorC=0;
		}
	}


	// Obtener el valor de la distancia en la dirección izquierda
	
	media = 0;
	for(i =0; i<10;i++){
		media += ecrobot_get_sonar_sensor(SENSOR_ULTRASONIDOS);
		systick_wait_ms(100);
	}
	distanciaIzq = media/10;

				
	/* Si distancia derecha es mayor que la izquierda:
        - Volvemos a girar el robot hacia la derecha
      En caso contrario:
        - El robot no se gira porque la dirección a tomar es la izquierda
   	*/  
	if(distanciaDcha > distanciaIzq){
		// Inicializamos contadores
		nxt_motor_set_count(NXT_PORT_B,0);
		nxt_motor_set_count(NXT_PORT_C, 0);
			
		// Activamos motores para girar a la dcha
		nxt_motor_set_speed(NXT_PORT_B, -40, 1);
		nxt_motor_set_speed(NXT_PORT_C, 40, 1);
		
		motorB = 1, motorC = 1;
		while (motorB || motorC){
			if(nxt_motor_get_count(NXT_PORT_B) <= -360){
				nxt_motor_set_speed(NXT_PORT_B, 0, 1);
				motorB =0;
			}
			
			if(nxt_motor_get_count(NXT_PORT_C) >= 360){
				nxt_motor_set_speed(NXT_PORT_C, 0, 1);
				motorC =0;
			}
		}
	}
	
	// Terminar la tarea
	TerminateTask();
}


/*------------------------------------------------------------------------*/
// Task information:                                                      
// -----------------                                                      
// Name    : Principal                                                       
// Priority: 1                                                            
// Typ     : EXTENDED TASK                                                
// Schedule: FULL														  
// Objective: Avanzar hasta que se produzca algun choque.		                              
/*------------------------------------------------------------------------*/
TASK (Principal)
{
	//int pulsador_choque;
	
	systick_wait_ms(10000);	


	ecrobot_get_sonar_sensor(SENSOR_ULTRASONIDOS);

   /* La tarea Principal se ejecutará mientras la bandera de fin esté desactivada (igual a 0) */
   	
	do{
		// Avanza hasta que se toque alguno de los dos pulsadores delanteros
        nxt_motor_set_speed(NXT_PORT_B, OriginalSpeedB, 1);    
		nxt_motor_set_speed(NXT_PORT_C, OriginalSpeedC, 1);
	
		nxt_motor_set_count(NXT_PORT_B,0);
		nxt_motor_set_count(NXT_PORT_C,0);
		
		do{			
			if(pulsador_choque){
				nxt_motor_set_speed(NXT_PORT_B, 0, 1);    
				nxt_motor_set_speed(NXT_PORT_C, 0, 1);
				break;
			}
			
			int rpmB = nxt_motor_get_count(NXT_PORT_B);
			int rpmC = nxt_motor_get_count(NXT_PORT_C);
			
			if(rpmB < rpmC){
				if(speedB<OriginalSpeedB)
					speedB++;
				else
					speedC--;
				nxt_motor_set_speed(NXT_PORT_B, speedB, 1);
				nxt_motor_set_speed(NXT_PORT_C, speedC, 1);		
			} else if(rpmC < rpmB){
				if(speedC<OriginalSpeedC)
					speedC++;
				else
					speedB--;
				nxt_motor_set_speed(NXT_PORT_C, speedC, 1);
				nxt_motor_set_speed(NXT_PORT_B, speedB, 1);		
			}
			
			if(pulsador_choque){
				nxt_motor_set_speed(NXT_PORT_B, 0, 1);    
				nxt_motor_set_speed(NXT_PORT_C, 0, 1);
				break;
			}

			systick_wait_ms(300);			

		}while(pulsador_choque!=1 && fin !=1); 
   
	   if(fin != 1){				
			nxt_motor_set_speed(NXT_PORT_B, 0, 1);    
			nxt_motor_set_speed(NXT_PORT_C, 0, 1);
		     
			// Va hacia atrás un poco para empezar de nuevo el reconocimiento
			nxt_motor_set_speed(NXT_PORT_B, -50, 1);    
			nxt_motor_set_speed(NXT_PORT_C, -50, 1);
			systick_wait_ms(600);

			ActivateTask(Giro);
		}
	
	}while(fin!=1);
   
	
	// Paramos los motores antes de finalizar la tarea
	nxt_motor_set_speed(NXT_PORT_B, 0, 1);    
	nxt_motor_set_speed(NXT_PORT_C, 0, 1);
   
	// Terminar la tarea actual
	TerminateTask();
}

/*------------------------------------------------------------------------*/
// Task information:                                                      
// -----------------                                                      
// Name    : Pulsador                                                       
// Priority: 3                                                            
// Typ     : EXTENDED TASK                                                
// Schedule: FULL														  
// Objective: Comprobar si se ha pulsado el sensor de fin de laberinto		                              
/*------------------------------------------------------------------------*/
TASK (Pulsador){
	int pulsador_fin;
	
	// Obtenemos el valor del sensor de pulsación para ver si ha sido activado
	pulsador_fin = ecrobot_get_touch_sensor(PULSADOR_FIN);
	pulsador_choque = ecrobot_get_touch_sensor(PULSADOR_CHOQUE);


	/* Si ha sido activado:
            - Paramos los motores
            - Establecemos la bandera de fin a 1 para indicar que el roboto debe finalizar
      Si no ha sido activado:
            - El programa continua normalmente
   */
	if(pulsador_fin == 1){
		fin = 1;
	}

	// Terminar la tarea actual
	TerminateTask();
}
