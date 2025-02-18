/*
 * lcd.c
 *
 *  Created on: Oct 29, 2020
 *      Author: mathe
 */


#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "system.h"
#include "io.h"
#include "sys/alt_stdio.h"

#include "LCD.h"

void lcdInit()
{
	// Espera por 50 ms para o controlador terminar
	// de configurar o LCD.
	usleep(50 * 1000);
	// Controlador da clear, mas fazemos de novo no caso de reinicio do programa.
	clearLcd();
}

int lcdIsBusy()
{
	/**
	 * Verifica se o controlador ja terminou de executar o ultimo comando enviado.
	 */
	if (IORD(0x1a82100, 0))
		return 1;

	return 0;
}

void send(int bus_data)
{
	/**
	 * Envia os dados passados diretamente para o barramento bus_data exportado
	 * pelo qsys, alem de um pulso de enable para iniciar a transacao no controlador.
	 */

	IOWR(0x1a82100, 0, bus_data);
}

void clearLcd()
{
	/**
	 * Faz a "limpeza" do LCD.
	 * Escreve 0x20, o espaco, na memoria do CI do modulo do LCD e retorna o cursor
	 * para o endereco 0x00, escrevendo-o no endereco do Address Count do CI.
	 */

	alt_putstr("Mandando clear para interface\n");
	send(CLEAR_CMD);

	// Atentar para o tempo necessario para o controlador detectar a mudanca no
	// enable.

	// Espera ocupada. Controlador espera 50us por padrao, mas operacao de clear
	// leva 1,53ms de acordo com datasheet:
	// https://www.sparkfun.com/datasheets/LCD/ADM1602K-NSW-FBS-3.3v.pdf
	while (lcdIsBusy());

	// Esperar por mais 2ms. Tentando consertar problema da escrita apos o clear.
	// Esperar por mais 1,5ms nao deu certo.
	// Esperar por mais 2ms funcionou.
	usleep(2000);
}

void returnHomeLcd()
{
	/**
	 * Faz o cursor retornar ao primeiro byte do LCD.
	 *
	 * Escritas apos este comando serao feitas nos primeiros espacos do LCD.
	 *
	 * Exemplo:
	 *     Antes:
	 *         Valor no sensor
	 *
	 *     return home + escrita de "foobar"
	 *     Depois:
	 *         foobarno sensor
	 *               | cursor ficara no char apos o ultimo escrito.
	 */

	alt_putstr("Mandando return home para interface\n");
	send(RETURN_HOME_CMD);

	// Atentar para o tempo necessario para o controlador detectar a mudanca no
	// enable.
	while (lcdIsBusy()); // Espera ocupada.
	// Apesar de estar escrito no datasheet que return home tambem leva por volta
	// de 1,53ms, esse funciona sem a espera de 2ms, como o clear faz.
}

void writeCharLcd(char data)
{
	/**
	 * Escreve um caracter no LCD da placa.
	 *
	 * Exemplo:
	 *     writeStrLcd('O'); Imprimira O na primeira linha no caracter atual do LCD
	 *
	 * Escreve diretamente no registrador de interface gerado pelo qsys que e ligado
	 * no controlador do LCD.
	 * Apos isso, manda um pulso de enable de duracao de 1us para ativar a escrita.
	 */

	int waited = 0;

	send(WRITE_PREFIX | data);



	while (lcdIsBusy())
		waited = 1;
	if (waited != 1)
		alt_putstr("Algo de errado ocorreu durante a espera\n");
}

void writeStrLcd(char* data)
{
	/**
	 * Escreve uma string no LCD da placa.
	 *
	 * Exemplo:
	 *     writeStrLcd("Ola"); Imprimira Ola na primeira linha do LCD na placa.
	 */
	int i;
	int len = strlen(data);
	for (i = 0; i < len; i++)
	{
		writeCharLcd(data[i]);
	}
}

void setLineToWriteLcd(int line)
{
	/**
	 * Seta a linha que se deve escrever no LCD.
	 * Se line for 1, escreve na primeira linha, se for 2, escreve na segunda.
	 */

	// Se for passado parametro errado, por padrao o que sera interpretado e que
	// se escrevera na primeira linha do LCD.
	int address = 0;

	if (line == 1)
	{
		// Se for na primeira linha do display, o datasheet ja mencionado na funcao
		// clear diz que precisamos escrever 0x00 no Address Count para DDRAM para
		// escrever no primeiro espaco da primeira linha.
		address = 0x00;
	}

	else if (line == 2)
	{
		// Se for na segunda linha do display, o datasheet ja mencionado na funcao
		// clear diz que precisamos escrever 0x40 no Address Count para DDRAM para
		// escrever no primeiro espaco da segunda linha.
		address = 0x40;
	}

	send(SET_DDRAM_ADDR_PREFIX | address);

	while(lcdIsBusy()); // Espera ocupada.
}

void updateValue(int value)
{
	/**
	 * Atualiza o valor lido pelo sensor no LCD.
	 *
	 * Escreve uma mensagem para o usuario, "Valor no sensor", na primeira linha
	 * e, na segunda, o valor lido.
	 */

	setLineToWriteLcd(1);
	writeStrLcd("Valor do sensor");

	// Valor lido pode ter ate 5 caracteres.
	char stringValue[5];

	setLineToWriteLcd(2);
	writeStrLcd("     "); // Limpamos a segunda linha.

	// Para voltarmos ao inicio da linha e escrevermos sobre onde limpamos.
	setLineToWriteLcd(2);
	sprintf(stringValue, "%d", value);
	writeStrLcd(stringValue);
}



