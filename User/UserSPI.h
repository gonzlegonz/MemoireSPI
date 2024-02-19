/*
 * UserSPI.h
 *
 *  Created on: Feb 2, 2024
 *      Author: Garlan
 */

#ifndef USER_INC_USERSPI_H_
#define USER_INC_USERSPI_H_

/* Déclaration des biblitohèques */
#include "stdint.h"
#include "stdio.h"
#include "gpio.h"
#include "spi.h"

/* Déclaration de fonctions */

/* Fonctions principales/indépendantes */
uint8_t AT25X_Read(int);
void AT25X_Write(int);
//uint8_t AT25X_Read(int,int);
//void AT25X_Read_Tab(int,int);
//void AT25X_Write(int,int);

void AT25X_StatusRegisterLock(void);
void AT25X_SequentialProgram(void);
void AT25X_ReadModifyWrite(void);
void AT25X_Terminate(void);
void AT25X_Reset(void);
void AT25X_DeepPowerDown_UltraDeepPowerDown(int);
void AT25X_XiPMode(int);

// Pour l'utiliser, désactiver le bus SPI et configurer MCU_SI et MCU_CS en tant que GPIO_Output
// A utiliser seulement si non retour des valeurs voulues après tests
void AT25X_ResetTotal(void);

/*Sous-fonctions*/
int AT25X_DefAddr(int);
int AT25X_DefData(void);
void Transition(int);
void AT25X_WriteEnable(void);
void AT25X_VolatileWriteEnable(void);
void AT25X_WriteDisable(void);

/* Déclaration de la structure SPI */
struct Variables_SPI{
	/*A supprimer au besoin*/
	uint8_t SFDP[256];
	uint8_t Array[256];
	uint8_t SecReg[128];
	uint8_t StatReg[6*1];
	uint8_t JEDEC_ID[5];
	uint8_t DeviceID[2];
	/*Conserver*/
	uint8_t Buffer_Tx[256];
	uint8_t Buffer_Rx[256];
	uint32_t MUX[3*16];
	uint8_t Commande[5];
	uint8_t Blocklock;
	uint8_t SR2;
	uint8_t SR3;
};

/* Déclaration des defines */

/* Valeur (définissable) du nombre de dummy octets à envoyer avec certaines commandes */
#define DMY_OCT 	2

/* Valeur (définissable) des données à envoyer */
#define DATA_TX		256

/* Commandes d'écriture */
#define CMD_STATREG1_W 				0x01
#define CMD_STATREG2_W 				0x31
#define CMD_STATREG3_W				0x11
#define CMD_BytePage_PROG 			0x02
#define CMD_DLOUT_BytePage_PROG 	0xA2
#define CMD_BUF_W 					0x84	//!\ Les seuls bits pris en compte pour l'addresse sont les bits de A7 à A0
#define CMD_PROG_SECREG 			0x9B	//!\ Les seuls bits pris en compte pour l'addresse sont les bits de A8 à A0
											//!\ La seule addresse inutilisable est 0x000000 car le registre de sécurité est déjà programmé par le constructeur
/* A utiliser en test */
/*
#define CMD_STATREG1_W_IND 	0x7101
#define CMD_STATREG2_W_IND	0x7102
#define CMD_STATREG3_W_IND 	0x7103
#define CMD_STATREG4_W_IND 	0x7104
#define CMD_STATREG5_W_IND 	0x7105
#define CMD_STATREG6_W_IND 	0x7106
#define CMD_STATREG_W_IND 	CMD_STATREG1_W_IND | CMD_STATREG2_W_IND | CMD_STATREG3_W_IND | CMD_STATREG4_W_IND | CMD_STATREG5_W_IND | CMD_STATREG6_W_IND // Entrée de 0x01 à 0x06 pour déterminer le registre à écrire
*/
#define CMD_STATREG_W_IND 	0x71	// A utiliser en cas réel

/* Commandes de lecture */
#define CMD_STATREG1_R		0x05
#define CMD_STATREG2_R 		0x35
#define CMD_STATREG3_R 		0x15
#define CMD_JEDEC_ID_R 		0x9F
#define CMD_READ_ARR 		0x03
#define CMD_BlockLock_R 	0x3C
#define CMD_DLOUT_READ_ARR 	0x3B
#define CMD_BUF_R 			0xD4
#define CMD_MAN_DEV_ID_R 	0x90
#define CMD_SFDP_R 			0x5A
#define CMD_FAST_READ_ARR 	0x0B
#define CMD_SECREG_R 		0x4B 	//!\ Les seuls bits pris en compte pour l'addresse sont les bits de A8 à A0

/* A utiliser en test */
/*
#define CMD_STATREG1_R_IND 	0x7501
#define CMD_STATREG2_R_IND 	0x7502
#define CMD_STATREG3_R_IND 	0x7503
#define CMD_STATREG4_R_IND	0x7504
#define CMD_STATREG5_R_IND 	0x7505
#define CMD_STATREG6_R_IND 	0x7506
#define CMD_STATREG_R_IND 	CMD_STATREG1_R_IND | CMD_STATREG2_R_IND | CMD_STATREG3_R_IND | CMD_STATREG4_R_IND | CMD_STATREG5_R_IND | CMD_STATREG6_R_IND
*/
#define CMD_STATREG_R_IND 	0x75	// A utiliser en cas réel

/* Commandes de fonctions */

/* Commande Détection Batterie */
#define CMD_LOWBATDTKT	0xEF

/* Commande Sequential Programming */
#define CMD_SEQPROG	0xAF

/* Commande Read-Modify-Write */
#define CMD_RMW		0x0A

/* Commande Write Enable */
#define CMD_EN_W	0x06

/* Commande Write Disable */
#define CMD_DIS_W	0x04

/* Commande Volatile Write Enable */
#define CMD_EN_VW	0x50

/* Commandes Reset */
#define CMD_EN_RES	0x66
#define CMD_RES_DEV	0x99

/* Commandes Suspend/Resume Program/Erase */
#define CMD_PROGER_SUS	0xB0
#define CMD_PROGER_RESU	0xD0

/* Commandes Terminate */
#define CMD_TER		0xF0
#define CMD_TER_VRF	0xD0

/* Commandes Mode Deep Power-Down/Ultra-Deep Power-Down */
#define CMD_DPD		0xB9
#define CMD_UDPD	0x69
/* N'utiliser qu'un seul des deux defines en fonction de la commande souhaitée */
#define CMD_RESU_DPD_UDPD		0xAB
//#define CMD_RESU_DPD_wDevID	0xAB

/* Commandes Protection Globale */
#define CMD_GlobBlockLock	0x7E
#define CMD_GlobBlockUnlock	0x98

/* Commande Active Interrupt Status */
// A n'utiliser que si utilisation d'interruption externe
#define CMD_AIS_SPI_MOD0	0x25
//#define CMD_AIS_SPI_MOD3	0x25

/* Commandes Status Registers Lock */
#define CMD_STATREGLock			0x6F
#define CMD_STATREGLock_VRF1	0x4D
#define CMD_STATREGLock_VRF2	0x67

/* Commande Buffer to Main Memory Page Program without Erase */
#define CMD_BUF_MMPPwER	0x88	//!\ Les seuls bits pris en compte pour l'addresse sont les bits de A23 à A8

/* Commandes Erase */
#define CMD_Chip_ER	0xC7
#define CMD_4K_ER	0x20	//!\ Les seuls bits pris en compte pour l'addresse sont les bits de A23 à A12
#define CMD_32K_ER 	0x52 	//!\ Les seuls bits pris en compte pour l'addresse sont les bits de A23 à A15
#define CMD_64K_ER	0xD8	//!\ Les seuls bits pris en compte pour l'addresse sont les bits de A23 à A6
#define CMD_PAG_ER	0x81 	//!\ Les seuls bits pris en compte pour l'addresse sont les bits de A20 à A8

/* Commandes inutilisables car bloquage physique (3 pins de libre (SO, SI, /HoldReset) au lieu de 4 (SO, SI, /WP et /HoldReset) */
#define CMD_MAN_DEV_ID_QUAD_R	 	0x94
#define CMD_QUADOUT_READ_ARR 		0x6B
#define CMD_QUADOUT_BytePage_PROG 	0x32
#define CMD_SET_BW					0x77
#define CMD_XiP_READ_ARR 			0xEB
#define CMD_XiP_READ_ARR_DWA_ADDR 	0xE7

/* Commandes inutilisables car bloquage physique (Pin /WP non connectée au µC) */
#define CMD_IND_BlockLock 0x36
#define CMD_IND_BlockUnlock 0x39

/* Defines switch case fonctions principales */
/* Define pour les commandes retournant un tableau */
#define CMD_TAB		CMD_STATREG_R_IND || CMD_SFDP_R || CMD_READ_ARR || CMD_SECREG_R || CMD_JEDEC_ID_R || CMD_MAN_DEV_ID_R || CMD_DLOUT_READ_ARR || CMD_FAST_READ_ARR || CMD_MAN_DEV_ID_QUAD_R || CMD_QUADOUT_READ_ARR || CMD_XiP_READ_ARR || CMD_XiP_READ_ARR_DWA_ADDR

#endif /* USER_INC_USERSPI_H_ */
