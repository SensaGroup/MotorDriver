#include "can.h"
#include <avr/io.h>
#include <avr/interrupt.h>


// unutrasnje promenljive koje sluze za komunikaciju
// svaka poruka sadrzi 8 bajtova podataka i identifikator poruke
static volatile canMsg rxBuffer[RX_BUFFER_SIZE];
static volatile unsigned char rxRdIndex = 0;
static volatile unsigned char rxWrIndex = 0;
static volatile unsigned char rxCounter = 0;

// Interrupt Service Routine
ISR(CANIT_vect)
{
	unsigned char tempPage = CANPAGE;
	unsigned char ch;

	// prolazim redom kroz MOb- ove i gledam da li se nesto dogadja
	for(ch = 0; ch < 15; ch++)
	{
		CANPAGE = ch << 4;

		// proveravam da li je uspesno primljen podatak (ako je MOb bio podesen kao prijemnik)
		// PRIJEMNI MOb- ovi uvek ostaju prijemni!
		if( (CANSTMOB >> RXOK) & 0x01)	//receive interrupt
		{
			// PRIJEMNI MOb- ovi uvek ostaju prijemni!
			CANCDMOB = (1 << CONMOB1); // podesavam MOb kao receiver
			CANSTMOB &= ~(1 << RXOK); // resetujem flag

			for(unsigned char i = 0; i < 8; i++)
				rxBuffer[rxWrIndex].data[i] = CANMSG;
			//altUart1Write(rxBuffer[rxWrIndex].data[0]);

			rxBuffer[rxWrIndex].msgIde = (CANIDT2 >> 5) | (CANIDT1 << 3);

			if(++rxWrIndex == RX_BUFFER_SIZE)
				rxWrIndex = 0;

			rxCounter++;

			//brPrimljenih1++;

			break;
		}

		// proveravam da li je uspesno poslat podatak (ako je MOb bio podesen kao predajnik)
		if( (CANSTMOB >> TXOK) & 0x01)	//transmit interrupt
		{
			CANCDMOB = 0; // disable mob
			CANSTMOB &= ~(1 << TXOK); // resetujem flag

			break;
		}
	}

	CANPAGE = tempPage;
	//CANGIT = CANGIT; // jako retardirano
}


/*********************************************************************************
* Function Name		: CAN_Init
* Description       : Funkcija inicijalizuje CAN magistralu podesavanjem
					  odgovarajucih registara kontrolera.
					  BAUD RATE = 250kbps
					  Podrazumeva se da je frekvencija kontrolera 10MHz
* Parameters        : None
* Return Value      : void
*********************************************************************************/
void CAN_Init(void)
{
	unsigned char ch;
	unsigned char data;

	// Reset the CAN controller
	CANGCON = 0x01;

	// idem od 0 do 14 MOba i resetujem ih, zato sto posle reseta MObovi imaju nedefinisano stanje
	for(ch = 0; ch < 15; ch++)
	{
		CANPAGE = ch << 4;

		CANSTMOB = 0;
		CANCDMOB = 0;

		CANIDT1 = 0;
		CANIDT2 = 0;
		CANIDT3 = 0;
		CANIDT4 = 0;
		CANIDM1 = 0;
		CANIDM2 = 0;
		CANIDM3 = 0;
		CANIDM4 = 0;

		// CANMSG je organizovan kao 8- bajtni FIFO red
		// koristi se auto-increment pa ce proci kroz svih 8 bajtova poruke-> organizovan kao FIFO
		for (data = 0; data < 8; data++)
			CANMSG = 0;
	}


	// Enable the CAN controller
	CANGCON = 0x02;

	/************ BIT TAJMING PODESAVANJA -> BAUD RATE = 100kbps  **********************************/

	//CANBT1 = 0x08; // Fcan = 2Mhz-> Tq = 0.5us
	CANBT1 = 0x01;
	CANBT2 = (1 << PRS2) | (1 << PRS0); // Propagation Time Segment-> 6xTq-> BILO 5
	CANBT3 = (1 << PHS22) | (1 << PHS21) /*| (1 << PHS20) */| (1 << PHS10) | (1 << PHS11) | (1 << PHS12); // Phase Segment 2-> 7xTq, Phase Segment 1-> 8xTq

	/// Sjw je podesen na 1... 1 + 5 + 6 + 8 = 20
	/************ BIT TAJMING PODESAVANJA -> BAUD RATE = 100kbps  **********************************/

	CANTCON = 0; // preskaler za CAN timer-> ne koristimo ga pa ova vrednost nije vazna

	// ukljucujem interrupte- ENIT-> Enable All Interrupts
	// ENRX-> Enable RX interrupt
	// ENTX-> Enable TX interrupt
	CANGIE = (1 << ENIT) | (1 << ENRX) | (1 << ENTX);
	// interrupti svih MOb- ova su ukljuceni
	CANIE1 = 0xFF >> 1; // glupo govno trazi da MSB mora biti nula prilikom upisivanja u registar
	CANIE2 = 0xFF;

	//sei();
}


/*********************************************************************************
* Function Name		: CAN_InitRxMob
* Description       : Funkcija inicijalizuje prvi slobodni MOb kao prijemnik.
					  Taj prijemni MOb ce primati samo poruke sa identifikatorom
					  ide. U slucaju neuspesne inicijalizacije MOb- a vraca 0
* Parameters        : unsigned int ide
* Return Value      : char info da li je inicijalizacija uspela
*********************************************************************************/
char CAN_InitRxMob(unsigned int ide)
{
	unsigned char ch;
	unsigned char tempPage = CANPAGE;

	while(((CANGSTA >> TXBSY) & 0x01) || ((CANGSTA >> RXBSY) & 0x01));

	// prolazim redom kroz MOb- ove, cim naidjem na slobodan MOb
	// uzimam ga
	for(ch = 0; ch < 15; ch++)
	{
		CANPAGE = ch << 4;

		// ako je MOb vec zauzet nastavi dalje
		if((CANCDMOB >> 6))
			continue;

		//uvek primam 8 bajtova
		CANCDMOB = (1 << DLC0) | (1 << DLC1) | (1 << DLC2) | (1 << DLC3);

		// podesavam identifikator
		CANIDT1 = (0x00FF & ide) >> 3;
		CANIDT2 = (ide & 0x00FF) << 5;

		// podesavam masku- gledam sve bitove
		CANIDM2 = 0x07 << 5;
		CANIDM1 = 0xFF;

		// podesavam MOb kao receiver
		CANCDMOB |= (1 << CONMOB1);
		sei();
		CANPAGE = tempPage;

		return 1;
	}

	CANPAGE = tempPage;

	return 0;
}


/*********************************************************************************
* Function Name		: CAN_checkRx
* Description       : Funkcija proverava da li ima primljenih podataka.
					  Ako ima vraca 1, u suprotnom vraca 0
* Parameters        : None
* Return Value      : char info da li ima primljenih podataka
*********************************************************************************/
char CAN_checkRX(void)
{
	if(rxCounter)
		return 1;

	return 0;
}


/*********************************************************************************
* Function Name		: CAN_read
* Description       : Funkcija cita 8- bajtni niz karaktera iz prijemnog bafera CAN
					  magistrale.
				      U slucaju da nema primljenih podataka funkcija ceka na
					  na podatak.
* Parameters        : canMsg *rxData
* Return Value      : void
*********************************************************************************/
void CAN_read(canMsg *rxData)
{
	// sve dok ne stigne neki podatak ostajem u funkciji

	// kako je task realizovan ovo je nepotrebno ali neka stoji
	while(rxCounter == 0);
	cli();
	//nema potrebe da iskljucujemo multitasking zato sto nema opasnosti da cu primiti pogresan podatak-> organizovano je kao FIFO

	// upisujem osam bajtova u prosledjeni bafer( argument funkcije, prenos po adresi)
	for(unsigned char i = 0; i < 8; i++)
		rxData->data[i] = rxBuffer[rxRdIndex].data[i];

	rxData->msgIde = rxBuffer[rxRdIndex].msgIde;

	if(++rxRdIndex == RX_BUFFER_SIZE)
		rxRdIndex = 0;


	rxCounter--;
	sei();
}


/*********************************************************************************
* Function Name		: CAN_write
* Description       : Funkcija upisuje 8 bajtova podataka u predajni bafer CAN
					  magistrale. Pored tih podataka, salje se i njen
					  identifikator messIdentificator.
* Parameters        : canMgs txData
* Return Value      : void
*********************************************************************************/
char CAN_write(canMsg txData)
{
	// Cekam sve dok je magistrala zauzeta-> da li je neophodno?
	while(((CANGSTA >> TXBSY) & 0x01) || ((CANGSTA >> RXBSY) & 0x01));
	//taskENTER_CRITICAL();
	unsigned char ch;
	unsigned char tempPage = CANPAGE;

	// ovo je mozda nepotrebno, razmisliti
	//taskENTER_CRITICAL();

	// trazim slobodan MOb
	for(ch = 0; ch < 15; ch++)
	{
		CANPAGE = ch << 4;

		if(!(CANCDMOB >> 6))
			break;
	}

	// nema slobodnih MOb- ova
	if(ch == 15)
	{
		//taskEXIT_CRITICAL();

		return 0;
	}

	//ova dva registra sadrze identifikator poruke koja ce se slati! efektivno to je adresa cvora koji ce primiti poruku
	CANIDT2 = (txData.msgIde & 0x0F) << 5;
	CANIDT1 = (txData.msgIde) >> 3;

	// DLC = 8
	CANCDMOB = 8;

	// upisujem 8 bajtova koje ce se slati u predajni bafer
	for(unsigned char i = 0; i < 8; i++)
		CANMSG = txData.data[i];

	// podesavam ga kao predajnik
	CANCDMOB |= (1 << CONMOB0);

	CANPAGE = tempPage;

	//taskEXIT_CRITICAL();

	return 1;
}