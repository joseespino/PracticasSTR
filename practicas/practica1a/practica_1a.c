/*--------------------------------------------------------------------------
 	Practica: 1.a 															
 																			
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
DeclareTask(Avance);
DeclareTask(Final);
DeclareTask(Calibrar);
DeclareCounter(cnt1);
DeclareAlarm(alarm1);

/*------------------------------------------------------------------------*/
/* Definitions                                                            */
/*------------------------------------------------------------------------*/
int OriginalSpeedB = 50, speedB;
int OriginalSpeedC = 50, speedC;
	
/*------------------------------------------------------------------------*/
/* Function to be invoked from a category 2 interrupt                     */
/*------------------------------------------------------------------------*/
void user_1ms_isr_type2(){
	SignalCounter(cnt1);
}

/*------------------------------------------------------------------------*/
// Task information:                                                      
// -----------------                                                      
// Name    : Avance                                                       
// Priority: 1                                                            
// Typ     : EXTENDED TASK                                                
// Schedule: FULL														  
// Objective: Realizar el avance			                              
/*------------------------------------------------------------------------*/
TASK(Avance)
{
   	int time_out = systick_get_ms() + 6000 ;
   
	speedB = OriginalSpeedB;
	speedC = OriginalSpeedC;
    
    // Activa motores para comprobar si existe desvio en la navegacion
	nxt_motor_set_speed(NXT_PORT_B, speedB, 1);    
	nxt_motor_set_speed(NXT_PORT_C, speedC, 1);
	
	// Espera hasta que se agote el time_out
	while(systick_get_ms() < time_out);

	//CancelAlarm(alarm1);
	ActivateTask(Final);

    // Termina la tarea actual
    TerminateTask();
}


/*------------------------------------------------------------------------*/
// Task information:                                                      
// -----------------                                                      
// Name    : Final                                                       
// Priority: 3                                                            
// Typ     : EXTENDED TASK                                                
// Schedule: FULL														  
// Objective: Parar los motores del robot			                              
/*------------------------------------------------------------------------*/
TASK(Final)
{
    // Para los motores
    nxt_motor_set_speed(NXT_PORT_B, 0, 1);
    nxt_motor_set_speed(NXT_PORT_C, 0, 1);
	
	// Cancela la alarma
	CancelAlarm(alarm1);
   
    // Termina la tarea actual
	TerminateTask();

}
/*------------------------------------------------------------------------*/
// Task information:                                                      
// -----------------                                                      
// Name    : Calibrar                                                       
// Priority: 2                                                            
// Typ     : EXTENDED TASK                                                
// Schedule: FULL														  
// Objective: Se encargar de calibar las ruedas y que el 
//			  movimiento sea lineal			                              
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
