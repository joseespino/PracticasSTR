/*--------------------------------------------------------------------------
 	Practica: 1.b 															
 																			
 	Autores:																
       - Francisco Arjona Lopez											
       - Cristobal Castro Villegas										
       - Jose Antonio Espino Palomares                                    
       - Antonio Osuna Caballero                                          
--------------------------------------------------------------------------*/
#include "kernel.h"
#include "kernel_id.h"
#include "ecrobot_interface.h"

/*------------------------------------------------------------------------*/
/* OSEK declarations                                                      */
/*------------------------------------------------------------------------*/
DeclareCounter(Contador);
DeclareTask(Retroceso);
DeclareTask(Avance);
DeclareTask(GiroIzq);
DeclareTask(GiroDer);
DeclareTask(Final);
DeclareTask(Calibrar);
DeclareTask(Principal);
DeclareAlarm(Alarma1);


/*------------------------------------------------------------------------*/
/* Definitions                                                            */
/*------------------------------------------------------------------------*/
int OriginalSpeedB = 50, speedB;
int OriginalSpeedC = 50, speedC;
int gradosMotorB = 0;
int gradosMotorC = 0;

/*------------------------------------------------------------------------*/
/* Function to be invoked from a category 2 interrupt                     */
/*------------------------------------------------------------------------*/
void user_1ms_isr_type2(){
	SignalCounter(Contador);
}

/*------------------------------------------------------------------------*/
// Task information:                                                      
// -----------------                                                      
// Name    : Avance                                                       
// Priority: 5                                                            
// Typ     : EXTENDED TASK                                                
// Schedule: FULL														  
// Objective: Realizar el avance		                              
/*------------------------------------------------------------------------*/
TASK(Avance)
{
	speedB = OriginalSpeedB;
   	speedC = OriginalSpeedC;

   // Activar servomotores para avanzar en linea recta	
   	nxt_motor_set_speed(NXT_PORT_B, speedB, 1);    
    nxt_motor_set_speed(NXT_PORT_C, speedC, 1);
	
	// grados de giro = (360 * recorrido) / (2 * PI * radio_ruedas)
	int gradosRecorrido = (360 * 53) / (2 * 3.1415 * 2.75) ;
	
	display_goto_xy(0,4);
	display_int (gradosRecorrido,5);
	display_update();
	
    // Espera hasta recorrer la distancia calculada
	while((gradosMotorB+nxt_motor_get_count(NXT_PORT_B)<gradosRecorrido) && (gradosMotorC+nxt_motor_get_count(NXT_PORT_C)<gradosRecorrido));

	
	// Cancela la alarma
	CancelAlarm(Alarma1);
    
    // Termina la tarea actual
    TerminateTask();

}

/*------------------------------------------------------------------------*/
// Task information:                                                      
// -----------------                                                      
// Name    : Retroceso                                                       
// Priority: 3
// Typ     : EXTENDED TASK                                                
// Schedule: FULL														  
// Objective: Realizar el retroceso		                              
/*------------------------------------------------------------------------*/
TASK(Retroceso)
{
	// Inicializar
	nxt_motor_set_count(NXT_PORT_C,0); 
	nxt_motor_set_count(NXT_PORT_B,0);
   
   	// Activar motores
   	nxt_motor_set_speed(NXT_PORT_B, -speedB, 1);    
    nxt_motor_set_speed(NXT_PORT_C, -speedC, 1);
	
	int gradosRecorrido = -((360 * 5) / (2 * 3.1415 * 2.75)) ;
	
   	// Espera activa hasta hacer el retroceso
	while(nxt_motor_get_count(NXT_PORT_B)>gradosRecorrido && nxt_motor_get_count(NXT_PORT_C)>gradosRecorrido); 


    // Termina la tarea actual
    TerminateTask();

}

/*------------------------------------------------------------------------*/
// Task information:                                                      
// -----------------                                                      
// Name    : GiroIzq                                                       
// Priority: 2
// Typ     : EXTENDED TASK                                                
// Schedule: FULL														  
// Objective: Realizar el giro a la izquierda.		                              
/*------------------------------------------------------------------------*/
TASK(GiroIzq)
{
	nxt_motor_set_speed(NXT_PORT_C, 0, 1);
	nxt_motor_set_speed(NXT_PORT_B, 0, 1);
	
	// Inicializar
    nxt_motor_set_count(NXT_PORT_C,0); 
	nxt_motor_set_count(NXT_PORT_B,0);
	
    // Activar servomotores para realizar un giro a la izquierda
    nxt_motor_set_speed(NXT_PORT_C, -speedC, 1);
	nxt_motor_set_speed(NXT_PORT_B, 0, 1);
	
	float recorridocm = (3.1415 * 10) / 2.0;
	// grados de giro = (360 * recorrido) / (2 * PI * radio_ruedas)
	int gradosRecorrido = -((360 * recorridocm) / (2 * 3.1415 * 2.75));
	
    // Espera hasta que se realice el giro a la izquierda
	while(nxt_motor_get_count(NXT_PORT_C)>gradosRecorrido); 

	
   // Terminar la tarea actual
    TerminateTask();
}

/*------------------------------------------------------------------------*/
// Task information:                                                      
// -----------------                                                      
// Name    : GiroDer                                                       
// Priority: 4
// Typ     : EXTENDED TASK                                                
// Schedule: FULL														  
// Objective: Realizar el giro a la derecha		                              
/*------------------------------------------------------------------------*/
TASK(GiroDer)
{
	nxt_motor_set_speed(NXT_PORT_C, 0, 1);
	nxt_motor_set_speed(NXT_PORT_B, 0, 1);
	
	// Inicializar
    nxt_motor_set_count(NXT_PORT_C,0); 
	nxt_motor_set_count(NXT_PORT_B,0);
	
    // Activar servomotores para realizar un giro a la derecha
    nxt_motor_set_speed(NXT_PORT_B, -speedC, 1);
	nxt_motor_set_speed(NXT_PORT_C, 0, 1);
	
	float recorridocm = (3.1415 * 11.5) / 2.0;
	// grados de giro = (360 * recorrido) / (2 * PI * radio_ruedas)
	int gradosRecorrido = -((360 * recorridocm) / (2 * 3.1415 * 2.75));
	
    // Espera hasta que se realice el giro a la derecha
	while(nxt_motor_get_count(NXT_PORT_B)>gradosRecorrido); 

	// Despierta la tarea de parada de motores
    ActivateTask(Final);
	
   // Terminar la tarea actual
    TerminateTask();


}

/*------------------------------------------------------------------------*/
// Task information:                                                      
// -----------------                                                      
// Name    : Final                                                       
// Priority: 7
// Typ     : EXTENDED TASK                                                
// Schedule: FULL														  
// Objective: Parar los motores	                              
/*------------------------------------------------------------------------*/
TASK(Final)
{
    // Para los motores
    nxt_motor_set_speed(NXT_PORT_B, 0, 1);
    nxt_motor_set_speed(NXT_PORT_C, 0, 1);

    // Termina la tarea actual
	TerminateTask();

}

/*------------------------------------------------------------------------*/
// Task information:                                                      
// -----------------                                                      
// Name    : Principal                                                       
// Priority: 1
// Typ     : EXTENDED TASK                                                
// Schedule: FULL														  
// Objective: Activar las tareas	                              
/*------------------------------------------------------------------------*/
TASK(Principal)
{
    // Activar tareas
    ActivateTask(Avance);
    ActivateTask(GiroIzq);
	ActivateTask(Retroceso);
    ActivateTask(GiroDer);

    // Terminar la tarea actual
	TerminateTask();
}

/*------------------------------------------------------------------------*/
// Task information:                                                      
// -----------------                                                      
// Name    : Calibrar                                                       
// Priority: 6
// Typ     : EXTENDED TASK                                                
// Schedule: FULL														  
// Objective: Realizar la calibracion de los motores	                              
/*------------------------------------------------------------------------*/
TASK(Calibrar)
{
	int rpmB = nxt_motor_get_count(NXT_PORT_B);
	int rpmC = nxt_motor_get_count(NXT_PORT_C);
	
	// Muestra las velocidades de ambos motores por pantalla
	display_goto_xy(0,0);
	display_int (rpmB, 3);
	display_goto_xy(6,0);
	display_int (rpmC,3);
	display_update();

	//Correccion de velocidad en los motores para su calibracion
	if(rpmB < rpmC){
		if(speedB<OriginalSpeedB+5) //Velocidad original (margen +5)
			speedB++;
		else
			speedC--;
		nxt_motor_set_speed(NXT_PORT_B, speedB, 1);
		nxt_motor_set_speed(NXT_PORT_C, speedC, 1);		
	}
	
	if(rpmC < rpmB){
		if(speedC<OriginalSpeedC+5) //Velocidad original (margen +5)
			speedC++;
		else
			speedB--;
		nxt_motor_set_speed(NXT_PORT_C, speedC, 1);
		nxt_motor_set_speed(NXT_PORT_B, speedB, 1);		
	}
		
	nxt_motor_set_count(NXT_PORT_B,0);
	nxt_motor_set_count(NXT_PORT_C,0);
    
    // Termina la tarea actual 
	TerminateTask();
}
