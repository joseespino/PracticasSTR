/*--------------------------------------------------------------------------
 	Practica: 1.c 															
 																			
 	Autores:																
       - Francisco Arjona Lopez											
       - Cristobal Castro Villegas										
       - Jose Antonio Espino Palomares                                    
       - Antonio Osuna Caballero                                          
--------------------------------------------------------------------------*/
#include "kernel.h"
#include "kernel_id.h"
#include "ecrobot_interface.h"
 
#define SONAR_PORT NXT_PORT_S1
#define PULSADOR1_PORT NXT_PORT_S2
 
 
/*------------------------------------------------------------------------*/
/* OSEK declarations                                                      */
/*------------------------------------------------------------------------*/
DeclareTask(Principal);
DeclareTask(Avance);
DeclareTask(Final);
DeclareTask( Comprueba_Distancia );
DeclareTask( Mitad_Potencia );
DeclareTask( Cuarto_Potencia );
DeclareTask( Retroceso );
DeclareTask( Giro_Dcha );
DeclareCounter(ContadorDistancia);

 
 
void ecrobot_device_initialize()
{
    // Inicializar los sensores activos
    ecrobot_init_sonar_sensor( NXT_PORT_S4 ); 
 
}
void ecrobot_device_terminate()
{
    // Finalizar los sensores activos
    ecrobot_term_sonar_sensor( NXT_PORT_S4 );
}
 
/*------------------------------------------------------------------------*/
/* Definitions                                                            */
/*------------------------------------------------------------------------*/
 
int OriginalSpeedB = 100, speedB;
int OriginalSpeedC = 100, speedC;

/*------------------------------------------------------------------------*/
/* Function to be invoked from a category 2 interrupt                     */
/*------------------------------------------------------------------------*/
void user_1ms_isr_type2(){
 
    //SignalCounter(Contador); 
    SignalCounter(ContadorDistancia); 
}
 
/*------------------------------------------------------------------------*/
// Task information:                                                      
// -----------------                                                      
// Name    : Avance                                                       
// Priority: 2                                                            
// Typ     : EXTENDED TASK                                                
// Schedule: FULL														  
// Objective: Realizar el avance		                              
/*------------------------------------------------------------------------*/
TASK(Avance)
{   	
    nxt_motor_set_speed(NXT_PORT_B, OriginalSpeedB, 1);    
    nxt_motor_set_speed(NXT_PORT_C, OriginalSpeedC, 1);
 
    // Terminar la tarea actual
    TerminateTask();
     
}

/*------------------------------------------------------------------------*/
// Task information:                                                      
// -----------------                                                      
// Name    : Mitad_Potencia                                                       
// Priority: 4                                                           
// Typ     : EXTENDED TASK                                                
// Schedule: FULL														  
// Objective: Reducir la potencia a la mitad		                              
/*------------------------------------------------------------------------*/
TASK( Mitad_Potencia )
{
    nxt_motor_set_speed(NXT_PORT_B, OriginalSpeedB/2, 1);    
    nxt_motor_set_speed(NXT_PORT_C, OriginalSpeedC/2, 1);
 
    // Terminar la tarea actual
    TerminateTask();
     
}
 
/*------------------------------------------------------------------------*/
// Task information:                                                      
// -----------------                                                      
// Name    : Cuarto_Potencia                                                       
// Priority: 5                                                           
// Typ     : EXTENDED TASK                                                
// Schedule: FULL														  
// Objective: Reducir un cuarto la potencia 		                              
/*------------------------------------------------------------------------*/
TASK( Cuarto_Potencia )
{
    nxt_motor_set_speed(NXT_PORT_B, OriginalSpeedB/4, 1);    
    nxt_motor_set_speed(NXT_PORT_C, OriginalSpeedC/4, 1);
 
    // Terminar la tarea actual
    TerminateTask();
     
}

/*------------------------------------------------------------------------*/
// Task information:                                                      
// -----------------                                                      
// Name    : Principal                                                       
// Priority: 1                                                           
// Typ     : EXTENDED TASK                                                
// Schedule: FULL														  
// Objective: Activar la tarea avance 		                              
/*------------------------------------------------------------------------*/
TASK(Principal)
{   
     // Activar la tarea avance
    ActivateTask( Avance );
 
    // Terminar la tarea actual
    TerminateTask();
     
}
 
/*------------------------------------------------------------------------*/
// Task information:                                                      
// -----------------                                                      
// Name    : Retroceso                                                       
// Priority: 6                                                           
// Typ     : EXTENDED TASK                                                
// Schedule: FULL														  
// Objective: Realizar el retroceso durante 1 seg. 		                              
/*------------------------------------------------------------------------*/
TASK( Retroceso )
{
     
    nxt_motor_set_speed(NXT_PORT_B, -100, 1);    
    nxt_motor_set_speed(NXT_PORT_C, -100, 1);
 
    systick_wait_ms(1000);
    
    // Activar la tarea giro_dcha
    ActivateTask( Giro_Dcha );
 
    // Terminar la tarea actual
    TerminateTask();
}

/*------------------------------------------------------------------------*/
// Task information:                                                      
// -----------------                                                      
// Name    : Giro_Dcha                                                       
// Priority: 7                                                           
// Typ     : EXTENDED TASK                                                
// Schedule: FULL														  
// Objective: Realizar el el giro a la derecha. 		                              
/*------------------------------------------------------------------------*/
TASK( Giro_Dcha )
{
    nxt_motor_set_count(NXT_PORT_C, 0);
     
    // Activamos motores  
    nxt_motor_set_speed(NXT_PORT_C, 40, 1);
    nxt_motor_set_speed(NXT_PORT_B, 0, 1);
     
    // Espera activa hasta hacer el giro
    while(nxt_motor_get_count(NXT_PORT_C)<350);
 
    // Terminar la tarea actual
    TerminateTask();
 
}
/*------------------------------------------------------------------------*/
// Task information:                                                      
// -----------------                                                      
// Name    : Comprueba_Distancia                                                       
// Priority: 10                                                          
// Typ     : EXTENDED TASK                                                
// Schedule: FULL														  
// Objective: Comprobar la distancia a la que se encuentra los obstaculos. 		                              
/*------------------------------------------------------------------------*/
TASK (Comprueba_Distancia)
{
    int contador = 1;
 
    int media = 0;
     
    int distancia;
 
   	int sensorPresion;
 
 	sensorPresion = ecrobot_get_touch_sensor( NXT_PORT_S1 );
	
	//Si se pulsa el sensor de presion	
	if( sensorPresion == 1 ){
 		
 		// Activar la tarea retroceso
 		ActivateTask( Retroceso );
 		
 		// Terminar la tarea actual
        TerminateTask();
		return;
     }
 	
 	//Se realizan 6 mediciones y se calcula la media
	while( contador < 6){
        distancia = ecrobot_get_sonar_sensor( NXT_PORT_S4 );
 
        media += distancia;
        contador++;
    }
 
    distancia = media / contador;
     
    if( distancia <= 100 && distancia > 20 ){
 		
 		// Activar la tarea Mitad_Potencia
		ActivateTask( Mitad_Potencia );
		
		// Terminar la tarea actual
    	TerminateTask();
	
    }
     
    else if( distancia <= 20 ){
   
        int sensorPresion;
 
        sensorPresion = ecrobot_get_touch_sensor( NXT_PORT_S1 );
         
        if( sensorPresion == 1 ){
 			
            // Activar la tarea retroceso
 			ActivateTask( Retroceso );
 		
 			// Terminar la tarea actual
        	TerminateTask();
        }
 
        else{
        	
            // Activar la tarea Cuarto_Potencia
            ActivateTask( Cuarto_Potencia );   
                 
  			// Terminar la tarea actual
            TerminateTask();
        }       
    }
     
    else{
    	// Activar la tarea Cuarto_Potencia
        ActivateTask( Avance );
        
        // Terminar la tarea actual
        TerminateTask();
    }
 
}
