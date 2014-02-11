/*--------------------------------------------------------------------------
 	Practica: 2.b - Cliente 															
 																			
 	Autores:																
       - Francisco Arjona Lopez											
       - Cristobal Castro Villegas										
       - Jose Antonio Espino Palomares                                    
       - Antonio Osuna Caballero                                          
--------------------------------------------------------------------------*/

#include <string.h>
#include "kernel.h"
#include "kernel_id.h"
#include "ecrobot_interface.h"

#define MAX_MOV 200

/*------------------------------------------------------------------------*/
/* OSEK declarations                                                      */
/*------------------------------------------------------------------------*/
DeclareTask(Principal);
DeclareTask(Avance);
DeclareTask(Retroceso);
DeclareTask(GiroIzq);
DeclareTask(GiroDer);
DeclareTask(Final);


void ecrobot_device_initialize()
{
	// Inicializamos el bluetooth
   ecrobot_init_bt_slave("STR");
}
void ecrobot_device_terminate()
{
	// Finalizamos el bluetooth
   ecrobot_term_bt_connection();
}

/*------------------------------------------------------------------------*/
/* Function to be invoked from a category 2 interrupt                     */
/*------------------------------------------------------------------------*/
void user_1ms_isr_type2() { }


/*------------------------------------------------------------------------*/
/* Definitions                                                            */
/*------------------------------------------------------------------------*/
int i = 0;
int movimientos[MAX_MOV];

int OriginalSpeedB = 50, speedB = 50;
int OriginalSpeedC = 47, speedC = 47;

TASK(Principal)
{
	int n=0;
	
	systick_wait_ms(10000);
	systick_wait_ms(3000);

	nxt_motor_set_speed(NXT_PORT_B, 50, 1);
	nxt_motor_set_speed(NXT_PORT_C, 50, 1);

	systick_wait_ms(1100);
	
	nxt_motor_set_speed(NXT_PORT_B, 0, 1);
	nxt_motor_set_speed(NXT_PORT_C, 0, 1);

	
	while(n == 0)
		ecrobot_read_bt_packet(&n, sizeof(int));

	while(i!=n && i < MAX_MOV){
		movimientos[i]=0;
		while(movimientos[i] == 0)
			ecrobot_read_bt_packet(&movimientos[i], sizeof(int));
		i++;
	}

	// Ejecutamos cada uno de los movimientos
	int parada = 0;
	for(i=0; i < n && !parada; i++)
	{
      if(movimientos[i] == (unsigned char) 1)
         // Giro hacia la izquierda
         ActivateTask(GiroIzq);
      else if(movimientos[i] == (unsigned char) 2)
         // Giro hacia la derecha
         ActivateTask(GiroDer);
      else if(movimientos[i] == (unsigned char) -1)
         // Fin de los movimientos
         parada = 1;
      else if(movimientos[i] > (unsigned char) 0)
         // Avance del robot
         ActivateTask(Avance);
	}

   // Activamos la tarea final
   ActivateTask(Final);
   
   // Terminar la tarea actual
   TerminateTask();
}

TASK(Avance)
{
	// Inicializamos contadores
	nxt_motor_set_count(NXT_PORT_B,0);
	nxt_motor_set_count(NXT_PORT_C,0);	

	// Activamos los motores
	nxt_motor_set_speed(NXT_PORT_B, OriginalSpeedB, 1);
	nxt_motor_set_speed(NXT_PORT_C, OriginalSpeedC, 1);
   
   
   int motorB = 1, motorC = 1;
	while(motorB || motorC){
		systick_wait_ms(100);	
		
		display_clear(1);
		display_goto_xy(0, 4);
		display_int(nxt_motor_get_count(NXT_PORT_B), 0);
		display_update();
		
		display_goto_xy(0, 5);
		display_int(nxt_motor_get_count(NXT_PORT_C), 0);
		display_update();
		
		display_goto_xy(0, 1);
		display_int(i, 0);
		display_update();
		
		if(nxt_motor_get_count(NXT_PORT_B)>= movimientos[i]){
			nxt_motor_set_speed(NXT_PORT_B, 0, 1);
			motorB = 0;
		}
		
		if(nxt_motor_get_count(NXT_PORT_C)>=movimientos[i]){
			nxt_motor_set_speed(NXT_PORT_C, 0, 1);	
			motorC = 0;
		}
		
		
		if(motorB && motorC){
			int rpmB = nxt_motor_get_count(NXT_PORT_B);
			int rpmC = nxt_motor_get_count(NXT_PORT_C);
			
			if(rpmB < rpmC){
				if(speedB<OriginalSpeedB+5)
					speedB++;
				else
					speedC--;
				nxt_motor_set_speed(NXT_PORT_B, speedB, 1);
				nxt_motor_set_speed(NXT_PORT_C, speedC, 1);		
			}
					
			if(rpmC < rpmB){
				if(speedC<OriginalSpeedC+5)
					speedC++;
				else
					speedB--;
				nxt_motor_set_speed(NXT_PORT_C, speedC, 1);
				nxt_motor_set_speed(NXT_PORT_B, speedB, 1);		
			}
		}
	}
  
	// Paramos los motores
	nxt_motor_set_speed(NXT_PORT_B, 0, 1);
	nxt_motor_set_speed(NXT_PORT_C, 0, 1);
   
	//i++;
	systick_wait_ms(500);

	// Terminar la tarea actual
	TerminateTask();
}

TASK(GiroDer)
{
	// Inicializamos contadores
	nxt_motor_set_count(NXT_PORT_B,0);
	nxt_motor_set_count(NXT_PORT_C,0);
		
	// Activamos motores para girar a la dcha
	nxt_motor_set_speed(NXT_PORT_B, -40, 1);
	nxt_motor_set_speed(NXT_PORT_C, 40, 1);
	
	int motorB = 1, motorC = 1;
	while (motorB || motorC){
		if(nxt_motor_get_count(NXT_PORT_B) <= -180){
			nxt_motor_set_speed(NXT_PORT_B, 0, 1);
			motorB=0;
		}
		
		if(nxt_motor_get_count(NXT_PORT_C) >= 180){
			nxt_motor_set_speed(NXT_PORT_C, 0, 1);
			motorC=0;
		}
	}

	systick_wait_ms(500);

	// Terminar la tarea actual
	TerminateTask();
}

TASK(GiroIzq)
{
	// Inicializamos contadores
	nxt_motor_set_count(NXT_PORT_B,0);
	nxt_motor_set_count(NXT_PORT_C,0);
		
	// Activamos motores para girar a la dcha
	nxt_motor_set_speed(NXT_PORT_B, 40, 1);
	nxt_motor_set_speed(NXT_PORT_C, -40, 1);
	
	int motorB = 1, motorC = 1;
	while (motorB || motorC){
		if(nxt_motor_get_count(NXT_PORT_B) >= 180){
			nxt_motor_set_speed(NXT_PORT_B, 0, 1);
			motorB=0;
		}
		
		if(nxt_motor_get_count(NXT_PORT_C) <= -180){
			nxt_motor_set_speed(NXT_PORT_C, 0, 1);
			motorC=0;
		}
	}

	systick_wait_ms(500);	
	// Terminar la tarea actual
	TerminateTask();
}

TASK(Final)
{
   // Apagamos el sistema
   ShutdownOS(0);
   
   // Terminar la tarea actual
   TerminateTask();
}
