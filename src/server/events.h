#ifndef EVENTS_H
#define EVENTS_H

#include <stdio.h>
<<<<<<< HEAD

=======
>>>>>>> lobbi_server_logic
#include "global.h"
#include "internalIPC.h"
#include "logic.h"

<<<<<<< HEAD
=======
/*Функция вызывающая определенные события исходя из msg_type (в global.h), 
dest_type определяет от сервера или от клиента пришло событие, 
id - у клиента 0, у сервера его id, в buf скрыта структура описывающая сообщение*/
>>>>>>> lobbi_server_logic
void events(int dest_type, int id, int msg_type, void *buf);
/*Инициализация внутренних струтур лобби сервера*/

<<<<<<< HEAD
=======
void init();

>>>>>>> lobbi_server_logic
#endif //EVENTS_H
