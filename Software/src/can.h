/**********************************************************************************
* File Name			: can.h
* Description       : Fajl je .h fajl i sadrzi deklaraciju funkcija za manipulaciju
                      CAN magistrale.
					  Magistrala radi na 250kbps, a podrazumeva se
					  da AT90CAN128 radi na 10Mhz.
					  Magistrala je podesena tako da uvek prima i salje 8 bajtova
**********************************************************************************/

/*
	Example init:
	CAN_Init();
	CAN_InitRxMob(MOTOR_CONTROL_CAN_ID);

	Example send:

	canMsg snd_message;
	memcpy(snd_message.data, data, 8);
	snd_message.msgIde = MAIN_BOARD_CAN_ID;

	if(CAN_write(snd_message) == 1) {
		// success
	}

	Example read:

	canMsg message;
	CAN_read(&message);				//this will wait for a receive
	first = message.data[0];
	if(first == 'A') {
		flip_led(1);
	}
*/

#ifndef CAN_H_
#define CAN_H_

// makroi za identifikatore pojedinacnih cvorova prikljucenih na magistralu
#define MOTOR_CONTROL_CAN_ID 0xD4
#define MAIN_BOARD_CAN_ID  	 0x04

#define RX_BUFFER_SIZE	50

typedef struct msg
{
	unsigned char data[8];
	unsigned int msgIde;
} canMsg;


/*********************************************************************************
* Function Name		: CAN_checkRx
* Description       : Funkcija proverava da li ima primljenih podataka.
					  Ako ima vraca 1, u suprotnom vraca 0
* Parameters        : None
* Return Value      : char info da li ima primljenih podataka
*********************************************************************************/
char CAN_checkRX(void);


/*********************************************************************************
* Function Name		: CAN_read
* Description       : Funkcija cita 8- bajtni niz karaktera iz prijemnog bafera CAN
					  magistrale.
				      U slucaju da nema primljenih podataka funkcija ceka na
					  na podatak.
* Parameters        : canMsg *rxData
* Return Value      : void
*********************************************************************************/
void CAN_read(canMsg *);


/*********************************************************************************
* Function Name		: CAN_write
* Description       : Funkcija upisuje 8 bajtova podataka u predajni bafer CAN
					  magistrale. Pored tih podataka, salje se i njen
					  identifikator messIdentificator.
* Parameters        : canMgs txData
* Return Value      : void
*********************************************************************************/
char CAN_write(canMsg);


/*********************************************************************************
* Function Name		: CAN_Init
* Description       : Funkcija inicijalizuje CAN magistralu podesavanjem
					  odgovarajucih registara kontrolera.
					  BAUD RATE = 250kbps
					  Podrazumeva se da je frekvencija kontrolera 10MHz
* Parameters        : None
* Return Value      : void
*********************************************************************************/
void CAN_Init(void);


/*********************************************************************************
* Function Name		: CAN_InitRxMob
* Description       : Funkcija inicijalizuje prvi slobodni MOb kao prijemnik.
					  Taj prijemni MOb ce primati samo poruke sa identifikatorom
					  ide. U slucaju neuspesne inicijalizacije MOb- a vraca 0
* Parameters        : unsigned int ide
* Return Value      : char info da li je inicijalizacija uspela
*********************************************************************************/
char CAN_InitRxMob(unsigned int);



#endif /* CAN_H_ */