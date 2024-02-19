/*
 * UserSPI.c
 *
 *  Created on: Feb 7, 2024
 *      Author: Garlan
 */

#include "UserSPI.h"

struct Variables_SPI variable;

void AT25X_Write(int command)
{
	AT25X_WriteEnable();
	if(command == (CMD_STATREG1_W || CMD_STATREG2_W || CMD_STATREG3_W || CMD_STATREG_W_IND)) AT25X_VolatileWriteEnable();

	HAL_GPIO_WritePin(MCU_nCS_GPIO_Port, MCU_nCS_Pin, GPIO_PIN_RESET);
	variable.Commande[0] = command;
	switch(command){
		/* 3 octets d'addresse et pas de donnée */
		case CMD_PAG_ER:
		case CMD_4K_ER:
		case CMD_32K_ER:
		case CMD_64K_ER:
		case CMD_IND_BlockLock:
		case CMD_IND_BlockUnlock:
		case CMD_BUF_MMPPwER:
			//AT25X_DefAddr(command); // cas concret
			HAL_SPI_Transmit_IT(&hspi4, variable.Commande, 4);
			break;

		/* 3 octets d'addresse + (1 à 256) octet/s de donnée */
		case CMD_BytePage_PROG: // 3octets+data
		case CMD_DLOUT_BytePage_PROG:
		case CMD_QUADOUT_BytePage_PROG:
		case CMD_BUF_W:
		case CMD_PROG_SECREG:
			//AT25X_DefAddr(command); // cas concret
			//AT25X_DefData(); // cas concret
			HAL_SPI_Transmit_IT(&hspi4, variable.Commande, 4);
			HAL_SPI_Transmit_IT(&hspi4, variable.Buffer_Tx, DATA_TX);
			break;

		/* 3 octets d'addresse + 1 octet de wrap */
		case CMD_SET_BW:
			//AT25X_DefAddr(command); // cas concret
			//AT25X_DefData(); // cas concret
			HAL_SPI_Transmit_IT(&hspi4, variable.Commande, 4);
			HAL_SPI_Transmit_IT(&hspi4, variable.Buffer_Tx, 1);
			break;

		/* 1 octet d'addresse + 1 octet de donnée */
		case CMD_STATREG_W_IND:
			//AT25X_DefAddr(command); // cas concret
			//AT25X_DefData(); // cas concret
			HAL_SPI_Transmit_IT(&hspi4, variable.Commande, 2);
			HAL_SPI_Transmit_IT(&hspi4, variable.Buffer_Tx, 1);
			break;

		/* Pas d'addresse et de donnée */
		case CMD_Chip_ER:
		case CMD_PROGER_SUS:
		case CMD_PROGER_RESU:
		case CMD_GlobBlockLock:
		case CMD_GlobBlockUnlock:
			HAL_SPI_Transmit_IT(&hspi4, variable.Commande, 1);
			break;

		default:
			//AT25X_DefData(); // cas concret
			HAL_SPI_Transmit_IT(&hspi4, variable.Commande, 1);
			HAL_SPI_Transmit_IT(&hspi4, variable.Buffer_Tx, 1);
			break;
	}
	HAL_GPIO_WritePin(MCU_nCS_GPIO_Port, MCU_nCS_Pin, GPIO_PIN_SET);
}
/*
void AT25X_Write(int command, int address)
{
	uint8_t Addresses[3] = {-1};
	HAL_SPI_Transmit_IT(&hspi4, (uint8_t*)&command, sizeof(command)/sizeof(int));
	if(address != 0xff)
		{
			Addresses[0] = address >> 16;
			Addresses[1] = (address >> 8) & 0xff;
			Addresses[2] = address & 0xff;
			HAL_SPI_Transmit_IT(&hspi4, Addresses, sizeof(Addresses)/sizeof(uint8_t));
		}
	HAL_GPIO_TogglePin(MCU_nCS_GPIO_Port, MCU_nCS_Pin);
	HAL_GPIO_TogglePin(MCU_nCS_GPIO_Port, MCU_nCS_Pin);
}
*/
/*
uint8_t AT25X_Read(int command, int address)
{
	uint8_t Addresses[3] = {-1};
	HAL_SPI_Transmit_IT(&hspi4, (uint8_t*)&command, sizeof(command)/sizeof(int));
	if(address != 0xff)
		{
			Addresses[0] = address >> 16;
			Addresses[1] = (address >> 8) & 0xff;
			Addresses[2] = address & 0xff;
			HAL_SPI_Transmit_IT(&hspi4, Addresses, sizeof(Addresses)/sizeof(uint8_t));
		}
	HAL_SPI_Receive_IT(&hspi4, &variable.Blocklock, sizeof(variable.Blocklock));
	HAL_GPIO_TogglePin(MCU_nCS_GPIO_Port, MCU_nCS_Pin);
	HAL_GPIO_TogglePin(MCU_nCS_GPIO_Port, MCU_nCS_Pin);
	return variable.Blocklock;
}
*/
/*
void AT25X_Read_Tab(int command,int address)
{
	uint8_t Addresses[3] = {-1};
	if(address != 0xff)
	{
		Addresses[0] = address >> 16;
		Addresses[1] = (address >> 8) & 0xff;
		Addresses[2] = address & 0xff;
		HAL_SPI_Transmit_IT(&hspi4, Addresses, sizeof(Addresses)/sizeof(uint8_t));
	}
	switch(command)
	{
	case CMD_JEDEC_ID_R:
		HAL_SPI_Receive_IT(&hspi4, variable.JEDEC_ID, sizeof(variable.JEDEC_ID));
		break;
	case CMD_MAN_DEV_ID_R:
		HAL_SPI_Receive_IT(&hspi4, variable.DeviceID, sizeof(variable.DeviceID));
		break;
	case CMD_SECREG_R:
		HAL_SPI_Receive_IT(&hspi4, variable.SecReg, sizeof(variable.SecReg));
		break;
	case CMD_SFDP_R:
		HAL_SPI_Receive_IT(&hspi4, variable.SFDP, sizeof(variable.SFDP));
		break;
	default:
		HAL_SPI_Receive_IT(&hspi4, variable.Buffer_Rx, sizeof(variable.Buffer_Rx));
		break;
	}
}
*/

uint8_t AT25X_Read(int command){
	int i;
	for(i = 0; i < 256 ; i++){
		variable.Buffer_Rx[i] = 0;
	}

	variable.Commande[0] = command;

	HAL_GPIO_WritePin(MCU_nCS_GPIO_Port, MCU_nCS_Pin, GPIO_PIN_RESET);
//	Transition(command);
	switch(command){

	/* 3 octets d'addresse */
	case CMD_READ_ARR:
	case CMD_BlockLock_R:
		//AT25X_DefAddr(command); // cas concret
		HAL_SPI_Transmit_IT(&hspi4, variable.Commande, 4);
		break;

	/* 3 octets d'addresse + 1 octet inutile */
	case CMD_FAST_READ_ARR:
	case CMD_DLOUT_READ_ARR:
	case CMD_QUADOUT_READ_ARR:
	case CMD_BUF_R:
	case CMD_SECREG_R:
	case CMD_SFDP_R:
		//AT25X_DefAddr(command); // cas concret
		HAL_SPI_Transmit_IT(&hspi4, variable.Commande, 5);
		break;

	/* 1 octet d'addresse + 1 octet inutile */
	case CMD_STATREG_R_IND:
		//AT25X_DefAddr(command); // cas concret
		HAL_SPI_Transmit_IT(&hspi4, variable.Commande, 3);
		break;

	/* 3 octets inutiles */
	case CMD_MAN_DEV_ID_R:
		HAL_SPI_Transmit_IT(&hspi4, variable.Commande, 4);
		break;

	/* 4 octets inutiles */
	case CMD_MAN_DEV_ID_QUAD_R:
		HAL_SPI_Transmit_IT(&hspi4, variable.Commande, 5);
		break;

	/* 1 octet inutile */
	/*case CMD_AIS_SPI_MOD3:
	 * 		HAL_SPI_Transmit_IT(&hspi4, variable.Commande, 2);
	 * 		break;
	 */

	default:
		HAL_SPI_Transmit_IT(&hspi4, variable.Commande, 1);
		break;
	}
	HAL_SPI_Receive_IT(&hspi4, variable.Buffer_Rx, 256);
	HAL_GPIO_WritePin(MCU_nCS_GPIO_Port, MCU_nCS_Pin, GPIO_PIN_SET);
	switch(command){
	case CMD_SFDP_R:
		for(i = 0;i<sizeof(variable.SFDP);i++){
			variable.SFDP[i] = variable.Buffer_Rx[i];
		}
		return 0;
		break;

	case CMD_SECREG_R:
		for(i = 0;i<sizeof(variable.SecReg);i++){
			variable.SecReg[i] = variable.Buffer_Rx[i];
		}
		return 0;
		break;

	case CMD_JEDEC_ID_R:
		for(i = 0;i<sizeof(variable.JEDEC_ID);i++){
			variable.JEDEC_ID[i] = variable.Buffer_Rx[i];
		}
		return 0;
		break;

	case CMD_QUADOUT_READ_ARR:
	case CMD_XiP_READ_ARR:
	case CMD_XiP_READ_ARR_DWA_ADDR:
	case CMD_DLOUT_READ_ARR:
	case CMD_FAST_READ_ARR:
	case CMD_READ_ARR:
		for(i = 0;i<sizeof(variable.Array);i++){
			variable.Array[i] = variable.Buffer_Rx[i];
		}
		return 0;
		break;

	case CMD_MAN_DEV_ID_QUAD_R:
	case CMD_MAN_DEV_ID_R:
		for(i = 0;i<sizeof(variable.DeviceID);i++){
			variable.DeviceID[i] = variable.Buffer_Rx[i];
		}
		return 0;
		break;

	case CMD_STATREG_R_IND:
		for(i = 0;i<sizeof(variable.StatReg);i++){
			variable.StatReg[i] = variable.Buffer_Rx[i];
		}
		return 0;
		break;

	default:
		return variable.Buffer_Rx[0];
	}
}

void AT25X_Reset(void)
{
	variable.Commande[0] = (uint8_t)CMD_EN_RES;
	variable.Commande[1] = (uint8_t)CMD_RES_DEV;

	HAL_GPIO_WritePin(MCU_nCS_GPIO_Port, MCU_nCS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit_IT(&hspi4, variable.Commande, 1);
	HAL_GPIO_WritePin(MCU_nCS_GPIO_Port, MCU_nCS_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(MCU_nCS_GPIO_Port, MCU_nCS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit_IT(&hspi4, variable.Commande+1, 1);
	HAL_GPIO_WritePin(MCU_nCS_GPIO_Port, MCU_nCS_Pin, GPIO_PIN_SET);
}

void AT25X_ReadModifyWrite(void)
{
	variable.Commande[0] = CMD_RMW;

	if(AT25X_DefAddr(CMD_RMW)==ERROR)return;
	if(AT25X_DefData()==ERROR)return;
	AT25X_WriteEnable();
	HAL_GPIO_WritePin(MCU_nCS_GPIO_Port, MCU_nCS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit_IT(&hspi4, variable.Commande, 4);
	HAL_SPI_Transmit_IT(&hspi4, variable.Buffer_Tx, 256);
	HAL_GPIO_WritePin(MCU_nCS_GPIO_Port, MCU_nCS_Pin, GPIO_PIN_SET);
}

void AT25X_SequentialProgram(void)
{
	static int previous_address = -1;
	int address;

	if(AT25X_DefAddr(CMD_SEQPROG)==ERROR)return;
	address = variable.Commande[1] << 16 | variable.Commande[2] << 8 | variable.Commande[3];

	AT25X_WriteEnable();

	HAL_GPIO_WritePin(MCU_nCS_GPIO_Port, MCU_nCS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit_IT(&hspi4, (uint8_t*) CMD_SEQPROG, 1);
	if(previous_address != address)
	{
		HAL_SPI_Transmit_IT(&hspi4, variable.Commande, 3);
		previous_address = address;
	}
	HAL_SPI_Transmit_IT(&hspi4, variable.Buffer_Tx, 1);
	HAL_GPIO_WritePin(MCU_nCS_GPIO_Port, MCU_nCS_Pin, GPIO_PIN_SET);
}

void AT25X_XiPMode(int command)
{
	variable.Commande[0] = command;
	if(AT25X_DefAddr(command)==ERROR)return;
	if(command == CMD_XiP_READ_ARR){
		HAL_GPIO_WritePin(MCU_nCS_GPIO_Port, MCU_nCS_Pin, GPIO_PIN_RESET);
		HAL_SPI_Transmit_IT(&hspi4, variable.Commande, 4+DMY_OCT);
		HAL_SPI_Receive_IT(&hspi4, variable.Buffer_Rx, 256);
		HAL_GPIO_WritePin(MCU_nCS_GPIO_Port, MCU_nCS_Pin, GPIO_PIN_SET);
	}
	else if(command == CMD_XiP_READ_ARR_DWA_ADDR){
		HAL_GPIO_WritePin(MCU_nCS_GPIO_Port, MCU_nCS_Pin, GPIO_PIN_RESET);
		HAL_SPI_Transmit_IT(&hspi4, variable.Commande, 4+DMY_OCT);
		HAL_SPI_Receive_IT(&hspi4, variable.Buffer_Rx, 256);
		HAL_GPIO_WritePin(MCU_nCS_GPIO_Port, MCU_nCS_Pin, GPIO_PIN_SET);
	}
}

void AT25X_StatusRegisterLock(void)
{
	variable.Commande[0] = CMD_STATREGLock;
	variable.Commande[1] = CMD_STATREGLock_VRF1;
	variable.Commande[2] = CMD_STATREGLock_VRF2;

	HAL_GPIO_WritePin(MCU_nCS_GPIO_Port, MCU_nCS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit_IT(&hspi4, variable.Commande, 3);
	HAL_GPIO_WritePin(MCU_nCS_GPIO_Port, MCU_nCS_Pin, GPIO_PIN_SET);
}

void AT25X_Terminate(void)
{
	variable.Commande[0] = CMD_TER;
	variable.Commande[1] = CMD_TER_VRF;

	HAL_GPIO_WritePin(MCU_nCS_GPIO_Port, MCU_nCS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit_IT(&hspi4, variable.Commande, 2);
	HAL_GPIO_WritePin(MCU_nCS_GPIO_Port, MCU_nCS_Pin, GPIO_PIN_SET);
}

void AT25X_DeepPowerDown_UltraDeepPowerDown(int command)
{
	variable.Commande[0] = command;

	HAL_GPIO_WritePin(MCU_nCS_GPIO_Port, MCU_nCS_Pin, GPIO_PIN_RESET);
	switch(command)
	{
	case CMD_UDPD:
		HAL_SPI_Transmit_IT(&hspi4, variable.Commande, 1);
		break;
	case CMD_DPD:
		HAL_SPI_Transmit_IT(&hspi4, variable.Commande, 1);
		break;
	case CMD_RESU_DPD_UDPD:
		HAL_SPI_Transmit_IT(&hspi4, variable.Commande, 1);
		break;
//	case CMD_RESU_DPD_wDevID:
//		HAL_SPI_Transmit_IT(&hspi4, variable.Commande, 4); // Lance 3 octets sans conséquence après la commande
//		break;
	}
	HAL_GPIO_WritePin(MCU_nCS_GPIO_Port, MCU_nCS_Pin, GPIO_PIN_SET);
}

void AT25X_WriteEnable(void)
{
	variable.Commande[0] = CMD_EN_W;

	HAL_GPIO_WritePin(MCU_nCS_GPIO_Port, MCU_nCS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit_IT(&hspi4, variable.Commande, 1);
	HAL_GPIO_WritePin(MCU_nCS_GPIO_Port, MCU_nCS_Pin, GPIO_PIN_SET);
}

void AT25X_VolatileWriteEnable(void)
{
	variable.Commande[0] = CMD_EN_VW;

	HAL_GPIO_WritePin(MCU_nCS_GPIO_Port, MCU_nCS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit_IT(&hspi4, variable.Commande, 1);
	HAL_GPIO_WritePin(MCU_nCS_GPIO_Port, MCU_nCS_Pin, GPIO_PIN_SET);
}

void AT25X_WriteDisable(void)
{
	variable.Commande[0] = CMD_DIS_W;

	HAL_GPIO_WritePin(MCU_nCS_GPIO_Port, MCU_nCS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit_IT(&hspi4, variable.Commande, 1);
	HAL_GPIO_WritePin(MCU_nCS_GPIO_Port, MCU_nCS_Pin, GPIO_PIN_SET);
}

int AT25X_DefAddr(int command)
{
	int addr[6];
	if(command == (CMD_STATREG_W_IND || CMD_STATREG_R_IND))
	{
		printf("\033[2J\033[;HQuelle est le registre voulu (de 1 a 6) : ");
		scanf("%d",&addr[0]);
		if(addr[0] != ('0'||'1'||'2'||'3'||'4'||'5'||'6'||'7'||'8'||'9'))
		{
			printf("\033[2J\033[;HRetapez la derniere commande\033[2J\033[;H");
			return ERROR;
		}
		else
		{
			variable.Commande[1] = addr[0] - 0x30;
			return SUCCESS;
		}
	}
	else
	{
		switch(command)
		{
		case CMD_PROG_SECREG:
		case CMD_SECREG_R:
			printf("\033[2J\033[;HAttention seul vos trois derniers caracteres seront pris en compte dans la memoire\n\r");
			printf("Quelle est l'addresse voulue (tapez 6 caracteres) : 0x");
			break;
		case CMD_BUF_W:
		case CMD_BUF_R:
			printf("\033[2J\033[;HAttention seul vos deux derniers caracteres seront pris en compte dans la memoire\n\r");
			printf("Quelle est l'addresse voulue (tapez 6 caracteres) : 0x");
			break;
		case CMD_4K_ER:
		case CMD_32K_ER:
			printf("\033[2J\033[;HAttention seul vos trois premiers caracteres seront pris en compte dans la memoire\n\r");
			printf("Quelle est l'addresse voulue (tapez 6 caracteres) : 0x");
			break;
		case CMD_64K_ER:
			printf("\033[2J\033[;HAttention seul le dernier caractere ne sera pas pris en compte dans la memoire\n\r");
			printf("Quelle est l'addresse voulue (tapez 6 caracteres) : 0x");
			break;
		case CMD_BUF_MMPPwER:
		case CMD_PAG_ER:
			printf("\033[2J\033[;HAttention seul les deux derniers caracteres ne seront pas pris en compte dans la memoire\n\r");
			printf("Quelle est l'addresse voulue (tapez 6 caracteres) : 0x");
			break;
		default:
			printf("\033[2J\033[;HQuelle est l'addresse voulue (tapez 6 caracteres) : 0x");
			break;
		}
		for(int i = 0;i<6;i++)
		{
			scanf("%d", &addr[i]);
			switch(addr[i])
			{
			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
			case 'F':
				addr[i] -= 0x37;
				break;
			case 'a':
			case 'b':
			case 'c':
			case 'd':
			case 'e':
			case 'f':
				addr[i] -= 0x57;
				break;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				addr[i] -= 0x30;
				break;
			default:
				printf("\033[2J\033[;HRetapez la derniere commande\033[2J\033[;H");
				return ERROR;
				break;
			}
		}
		variable.Commande[1] = addr[0] << 4 | addr[1];
		variable.Commande[2] = addr[2] << 4 | addr[3];
		variable.Commande[3] = addr[4] << 4 | addr[5];
	}

	return SUCCESS;
}

int AT25X_DefData()
{
	printf("\033[2J\033[;HQuelle est la donnee a transmettre (tapez jusqu'a 512 caracteres sinon tapez Entree): 0x");
	int Data[512], i = 0, n = 0;
	while((Data[i] != '\n') || (i < 512))
	{
		scanf("%d",&Data[i]);
		switch(Data[i])
		{
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
			Data[i] -= 0x37;
			break;
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f':
			Data[i] -= 0x57;
			break;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			Data[i] -= 0x30;
			break;
		default:
			printf("\033[2J\033[;HRetapez la derniere commande\033[2J\033[;H");
			return ERROR;
			break;
		}
		i++;
	}
	while((Data[n] != '\n') || (i < 512) || (Data[n+1] != '\n'))
	{
		variable.Buffer_Tx[n] = Data[i] << 4 | Data[i+1];
		i += 2;
		n++;
	}
	return SUCCESS;
}

void Transition(int command)
{
	switch(command){
	/* 3 octets d'addresse */
	case CMD_READ_ARR:
	case CMD_BlockLock_R:
		//AT25X_DefAddr(command); // cas concret
		HAL_SPI_Transmit_IT(&hspi4, variable.Commande, 4);
		break;

	/* 3 octets d'addresse + 1 octet inutile */
	case CMD_FAST_READ_ARR:
	case CMD_DLOUT_READ_ARR:
	case CMD_QUADOUT_READ_ARR:
	case CMD_BUF_R:
	case CMD_SECREG_R:
	case CMD_SFDP_R:
		//AT25X_DefAddr(command); // cas concret
		HAL_SPI_Transmit_IT(&hspi4, variable.Commande, 5);
		break;

	/* 1 octet d'addresse + 1 octet inutile */
	case CMD_STATREG_R_IND:
		//AT25X_DefAddr(command); // cas concret
		HAL_SPI_Transmit_IT(&hspi4, variable.Commande, 3);
		break;

	/* 3 octets inutiles */
	case CMD_MAN_DEV_ID_R:
		HAL_SPI_Transmit_IT(&hspi4, variable.Commande, 4);
		break;

	/* 4 octets inutiles */
	case CMD_MAN_DEV_ID_QUAD_R:
		HAL_SPI_Transmit_IT(&hspi4, variable.Commande, 5);
		break;

	/* 1 octet inutile */
	/*case CMD_AIS_SPI_MOD3:
	 * 		HAL_SPI_Transmit_IT(&hspi4, variable.Commande, 2);
	 * 		break;
	 */

	default:
		HAL_SPI_Transmit_IT(&hspi4, variable.Commande, 1);
		break;
	}
	HAL_SPI_Receive_IT(&hspi4, variable.Buffer_Rx, 256);
}

void AT25X_ResetTotal(void)
{
	HAL_GPIO_WritePin(MCU_SI_GPIO_Port, MCU_SI_Pin, GPIO_PIN_RESET);
	for(int i = 0;i<8;i++)
	{
		HAL_GPIO_TogglePin(MCU_nCS_GPIO_Port, MCU_nCS_Pin);
	}
	for(int i = 0;i<2;i++)
	{
		HAL_GPIO_WritePin(MCU_nCS_GPIO_Port, MCU_nCS_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MCU_SI_GPIO_Port, MCU_SI_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MCU_nCS_GPIO_Port, MCU_nCS_Pin, GPIO_PIN_SET);

		HAL_GPIO_WritePin(MCU_nCS_GPIO_Port, MCU_nCS_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MCU_SI_GPIO_Port, MCU_SI_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(MCU_nCS_GPIO_Port, MCU_nCS_Pin, GPIO_PIN_SET);
	}
}
