/*--------------------------------------------------------------------------
 	Practica: 3.a 															
 																			
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
DeclareTask(Principal);
DeclareTask(Equilibrio);


void ecrobot_device_initialize()
{
	// Inicializar los sensores activos
	ecrobot_set_light_sensor_active(NXT_PORT_S1);
}
void ecrobot_device_terminate()
{
	// Finalizar los sensores activos
	ecrobot_set_light_sensor_inactive(NXT_PORT_S1);
}


/*------------------------------------------------------------------------*/
/* Definitions                                                            */
/*------------------------------------------------------------------------*/
int parada = 0;

/*------------------------------------------------------------------------*/
/* Function to be invoked from a category 2 interrupt                     */
/*------------------------------------------------------------------------*/
void user_1ms_isr_type2(){}

/*------------------------------------------------------------------------*/
// Task information:                                                      
// -----------------                                                      
// Name    : Equilibrio                                                       
// Priority: 2                                                            
// Typ     : EXTENDED TASK                                                
// Schedule: FULL														  
// Objective: Se encarga de mantener el robot en equilibrio                              
/*------------------------------------------------------------------------*/
TASK(Equilibrio)
{

	nxt_motor_set_count(NXT_PORT_C,0); 
	nxt_motor_set_count(NXT_PORT_B,0);

	while(1)
  	{
    	int nivel;
    
    	nivel=ecrobot_get_light_sensor(NXT_PORT_S1);

    	//Mostraremos por pantalla el valor del sensor
    	display_clear(0);
    	display_goto_xy(0,3);
    	display_int(nivel,1);
    	display_update();
    
	    //Este nivel ha sido elegido segun las condiciones dadas durante la realizacion de la practica.
	    if(nivel <640)
	    {
			if(nivel<620){
				nxt_motor_set_speed(NXT_PORT_B, -77, 0);
				nxt_motor_set_speed(NXT_PORT_C, -77, 0);
			}
			else{
			nxt_motor_set_speed(NXT_PORT_B, -68, 0);
			nxt_motor_set_speed(NXT_PORT_C, -68, 0);
			}
	    }
	    else if(nivel >655)
	    {
			if(nivel > 685){
				nxt_motor_set_speed(NXT_PORT_B, 75, 0);
				nxt_motor_set_speed(NXT_PORT_C, 75, 0);
			}
			else{
				nxt_motor_set_speed(NXT_PORT_B, 65, 0);
				nxt_motor_set_speed(NXT_PORT_C, 65, 0);
			}
	    
	    }
	    else
	    {
				nxt_motor_set_speed(NXT_PORT_B, 0, 0);
				nxt_motor_set_speed(NXT_PORT_C, 0, 0);
		}
		


  	}
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
// Objective: Activa la tarea equilibrio                              
/*------------------------------------------------------------------------*/
TASK(Principal)
{
  //
  ActivateTask(Equilibrio);
  
  // Terminar la tarea actual
  TerminateTask();
}
