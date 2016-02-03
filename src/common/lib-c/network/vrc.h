/*
 *  common/lib-c/network/telecommande.h
 * -----------------------------------------------------------
 *  Project: ECPV2 / Télécommande
 *  Description : librairie multiplateforme de communication avec une Centrale.
 *                Fournit un ensemble de fonctions de haut niveau de lecture et
 *                d'écriture de données d'une centrale.
 *  Dernière modification: MAR : 07/03/2002 : maj de VRC_RESID_STATUS_ST :  4 champs de plus
 *  Copyright (c) 2000-2008 SpringCard - www.springcard.com
 */


#ifndef __VRC_H__
#define __VRC_H__

#include "noralsy/types.h"
#include "noralsy/vigik.h"

/* NORALSY */
#include "nvram.h"

/* L'année retournée par la Centrale est relative
   (sur un octet). Il faut donc faire un calcul
   VRC_ANNEE_DEBUT + annee_offset
*/
#define VRC_ANNEE_DEBUT   1900

typedef struct
{ /* les mots sont au format de la plateforme hôte */
  /* c'est transparent pour le programmeur, ne pas */
  /* se casser la tête pour l'inversion des octets */
  WORD max_resid_l; 
  WORD resid_l_presents;  
  WORD max_resid_c;
  WORD resid_c_presents;

  WORD max_repert;
  WORD repert_presents;

  WORD max_blacklist;
  WORD blacklist_presents;
} VRC_RESID_STATUS_ST;


typedef struct
{
  BYTE zero1; /* inutilisé */
  BYTE max_services;
  BYTE zero2; /* inutilisé */
  BYTE services_presents;
  WORD max_evenements;
  WORD evenement_courant;
}VRC_VIGIK_STATUS_ST;

typedef struct
{
  BYTE code[SVC_LEN];
  BYTE pub_key_version;
  BYTE pub_key_taille;

  BYTE jours;
  BYTE heures[3][2][2];       

  BYTE acl; 	/* Droit porte par porte : |0|0|0|0|p4|p3|p2|p1|*/
                              /*  20 octets */
} VRC_VIGIK_SERVICE_PARAM_ST;

typedef struct
{
  BYTE jour;    /* jour dans le mois de 1 a 31 */
  BYTE mois;    /* mois dans l'année de 1 a 12 */
  BYTE annee;   /* annee depuis 1900 */
  BYTE heure;   /* heure, de 0 a 23 */
  BYTE minute;  /* minute de 0 a 59 */
  BYTE seconde; /* secondes, de 0 a 59 */
  char saison;  /* 'E' si heure d'ete, 'H' si heure d'hiver */
} VRC_DATETIME_ST;

typedef struct
{
	BYTE tempo;
	BYTE woac_num;
	BYTE woac_perles;
	BYTE woac_config;		/* valeurs= WOAC_LIBRE, WOAC_LIBRE, WOAC_SORTIE, WOAC_DENIED */
	BYTE st_ouv;				/* semaine type ouverture auto */
	BYTE st_bp;					/* semaine type bouton poussoir */
} VRC_VIGIK_PORTE_ST;

BOOL VRC_Open();
void VRC_Close();


BYTE VRC_GetLastNackCode();

/* prototype d'une fonction callback d'affichage de la progresssion du chargement
   d'un lot d'informations */
typedef void VRC_ProgressionDisplayFuncType (WORD progression);
typedef VRC_ProgressionDisplayFuncType * VRC_ProgressionDisplayFuncPtr;

/* prototype d'une fonction callback de gestion de l'expiration  */
/* d'une session avec la centrale                                */                        
typedef void VRC_SessionTimeoutMgrFuncType(void);
typedef VRC_SessionTimeoutMgrFuncType * VRC_SetSessionTimeoutMgrFuncPtr;


/* installe une fonction callback de gestion de l'expiration */
/* d'une session avec la centrale.                           */
/* Il n'est pas obligatoire d'en installer une.              */
/* Généralement, cette fonction sert à revenir à l'écran     */
/* d'accueil.                                                */
void VRC_SetSessionTimeoutMgr(VRC_SetSessionTimeoutMgrFuncPtr func);

/* AUTHENTIFICATION */
/* ---------------- */

BOOL VRC_Authentificate(WORD  autPtr, char ** authBuf, BYTE * ack);

BOOL VRC_ChangePass(WORD oldPass, WORD newPass, BYTE * ack);

/*  Demande à la Centrale les données suivantes (SER_GETNEXT) d'une requête.
    Prérequis: une demande (SER_GET) a précédemment été envoyée.
    Note : Cette fonction est à utiliser avec précaution car vous pouvez
           passer en paramètre n'importe quel pointeur.
           Vous devez passer non pas un pointeur sur des données,
           mais l'adresse du pointeur sur le type de données que
           vous comptez recevoir.
*/

BOOL VRC_AskNext(void * adressOfPointerPtr, BYTE *datalen, BYTE * ack);


/* VRC_GetBaseMemory
   Lecture de la NVRAM, de l'adresse (ID*32) a l'adresse (ID*32+31)
   - Pour une NVRAM de  32ko, ID<1024
   - Pour une NVRAM de 128ko, ID<4096

  Met à jour bufferPtr avec l'adresse du buffer contenant les données.
*/

BOOL VRC_GetBaseMemory(BYTE ** bufferPtr, WORD id,BYTE * ack);

BOOL VRC_SetBaseMemory(BYTE * bufferPtr, WORD id,BYTE * ack);

/* VRC_GetBaseDateTime
  Lecture de l'heure de la Centrale.
*/

BOOL VRC_GetBaseDateTime(VRC_DATETIME_ST ** dtimPtr, BYTE * ack);

BOOL VRC_SetBaseDateTime(VRC_DATETIME_ST * dtimPtr, BYTE *ack);


/* tempos des gaches */
BOOL VRC_GetTempos(BYTE ** tempoPtr, BYTE * ack);
BOOL VRC_SetTempos(BYTE * tempoPtr, BYTE * ack);

/* horaires type */
BOOL VRC_GetHorType(ECPV_HOR_ST ** ecpvHorPtr, WORD id, BYTE * ack);
BOOL VRC_SetHorType(ECPV_HOR_ST * ecpvHorPtr, WORD id, BYTE * ack);

/* semaine type 
  NOTE: semaine type 0 : autorisé tous les jours 24h/24
                type 15 : interdit tous les jours 24h/24
*/
BOOL VRC_GetSemType(ECPV_SEM_ST ** ecpvSemPtr, WORD id, BYTE * ack);
BOOL VRC_SetSemType(ECPV_SEM_ST * ecpvSemPtr, WORD id, BYTE * ack);

/* code clavier */
BOOL VRC_GetCodeClavier(ECPV_CODE_ST ** ecpvCodPtr, WORD id, BYTE * ack);
BOOL VRC_SetCodeClavier(ECPV_CODE_ST * ecpvCodPtr, WORD id, BYTE * ack);

/* VRC_GetResidStatus                                                 */
/* statut général des résidents                                       */
/* ack est mis à jour avec SER_ACK si succès.                         */
/* Les 8 valeurs retournées, de 2 octets chacunes, sont au format de  */
/* la plateforme: la gestion du format des mots est transparente pour */
/* le programmeur.                                                    */
/* ------------------------------------------------------------------ */

BOOL VRC_GetResidStatus(VRC_RESID_STATUS_ST **residStatusPtr, BYTE * ack);


/* DONNEES DES RESIDENTS */
/* --------------------- */

/* données d'un résident long */
BOOL VRC_GetResidL(ECPV_RESL_ST ** ecpvResLPtr,WORD id, BYTE * ack);
BOOL VRC_SetResidL(ECPV_RESL_ST * ecpvResLPtr,WORD id, BYTE * ack);

/* données d'un résident court */
BOOL VRC_GetResidC(ECPV_RESC_ST ** ecpvResCPtr, WORD id, BYTE * ack);
BOOL VRC_SetResidC(ECPV_RESC_ST * ecpvResCPtr, WORD id, BYTE * ack);

/* STATUS VIGIK */
/* ------------ */

/* statut de Vigik (lecture seulement) */
BOOL VRC_GetVigikStatus(VRC_VIGIK_STATUS_ST ** vigikStatusPtr, BYTE * ack);


/* DONNEES DES SERVICES VIGIK */
/* -------------------------- */

/* demande des informations complètes sur un service Vigik 
  Note : la structure retournée vigikServicePtr est incomplète:
         la clé publique pub_key n'est pas renseignée car en lecture,
         elle s'avère inutile (donc on gagne du temps en ne la demandant
         pas à la Centrale ).
*/
BOOL VRC_GetVigikService(VIGIK_SERVICE_ST ** vigikServicePtr, WORD id, BYTE * ack);

/* Enregistre un service Vigik.
*/

BOOL VRC_SetVigikService(VIGIK_SERVICE_ST * vigikServicePtr, WORD id, BYTE * ack);

/* Retourne le nom d'un service Vigik.
   Paramètres:
    En entrée:
      id :          numéro du service Vigik
    En sortie:    
      nameBuffer:   nom du service (32 octets, pas forcément de '\0' de fin)
      ack :         la réponse de la centrale (SER_GETRESP ou NACK)
*/
BOOL VRC_GetVigikServiceName(BYTE ** nameBuffer, WORD id, BYTE * ack);

/*  Enregistre/Modifie uniquement le nom d'un service Vigik.
    Voir aussi: VRC_GetVigikService.
*/
BOOL VRC_SetVigikServiceName(BYTE * nameBuffer, WORD id, BYTE * ack);


/* paramètres d'un service Vigik */
/* ----------------------------- */

BOOL VRC_GetVigikServiceParam(VRC_VIGIK_SERVICE_PARAM_ST ** vigikServicePtr, WORD id, BYTE * ack);
BOOL VRC_SetVigikServiceParam(VRC_VIGIK_SERVICE_PARAM_ST * vigikServicePtr, WORD id, BYTE * ack);


/* signatures service Vigik */
/* ------------------------ */
/*  Lit uniquement la signature d'un service Vigik.
    Voir aussi: VRC_GetVigikService.
*/
BOOL VRC_GetVigikSign0(BYTE **  buffer32, WORD id, BYTE * ack);
BOOL VRC_GetVigikSign1(BYTE **  buffer32, WORD id, BYTE * ack);
BOOL VRC_GetVigikSign2(BYTE **  buffer32, WORD id, BYTE * ack);
BOOL VRC_GetVigikSign3(BYTE **  buffer32, WORD id, BYTE * ack);

/*  Enregistre/Modifie uniquement la signature d'un service Vigik.
    Voir aussi: VRC_SetVigikService.
*/

BOOL VRC_SetVigikSign0(BYTE *  buffer32, WORD id, BYTE * ack);
BOOL VRC_SetVigikSign1(BYTE *  buffer32, WORD id, BYTE * ack);
BOOL VRC_SetVigikSign2(BYTE *  buffer32, WORD id, BYTE * ack);
BOOL VRC_SetVigikSign3(BYTE *  buffer32, WORD id, BYTE * ack);


/* évènement Vigik */
BOOL VRC_GetVigikEvent(VIGIK_EVENEMENT_ST ** vigikEvenementPtr, WORD id, BYTE * ack);

/* REPERTOIRE */
/* ---------- */

/* Retourne les 2 messages d'accueil de la porte n°id.
   Les 2 messages sont alignés sur 2 lignes de 8 octets.
   Attention: pas de caractère zéro de fin de chaîne pour
   chacun des messages.
*/
BOOL VRC_GetRepertAcc(BYTE ** msgs, WORD id, BYTE * ack);

/* Enregistre le nouveau message d'accueil pour la porte n°id.
   Les 2 messages sont alignés sur 2 lignes de 8 octets.
   Attention: pas de caractère zéro de fin de chaîne pour
   chacun des messages.
*/   
BOOL VRC_SetRepertAcc(BYTE * msgs, WORD id, BYTE * ack);

/* retourne le masque pour la porte n°id.
   Attention: pas de caractère zéro de fin de chaîne.
*/
BOOL VRC_GetRepertMask(BYTE ** maskBuffer, WORD id, BYTE * ack);

/* Enregistre le nouveau masque pour la porte n°id.
   Attention: pas de caractère zéro de fin de chaîne.
   Vous devez donc fournir un masque de la taille LL_RPM (32 octets).
*/
BOOL VRC_SetRepertMask(BYTE * maskBuffer, WORD id, BYTE * ack);


/* Retourne les 8 messages de dialogue pour la porte n°id.
   Les 8 messages sont alignés sur 8 lignes de 16 octets.
   Attention: pas de caractère zéro de fin de chaîne pour
   chacun des messages.
    * msgs contiendra l'adresse du tampon de 128 octets contenant
   les messages de dialogues.
*/   
BOOL VRC_GetRepertDial(BYTE ** msgs, WORD id, BYTE * ack);

/* Enregistre les 8 messages de dialogue pour la porte n°id.
   Les 8 messages sont alignés sur 8 lignes de 16 octets.
   Attention: pas de caractère zéro de fin de chaîne pour
   chacun des messages.
*/   

BOOL VRC_SetRepertDial(BYTE * msgs, WORD id, BYTE * ack);

/* Retourne un élément ECPV_REPNOM_ST du répertoire.
    Paramètres : repNomBuffer : retourne le pointeur sur la structure
                   id : le numéro de l'élémént recherché
                 ack : retourne la réponse de la centrale (ACK, SER_GETRESP)
*/
BOOL VRC_GetRepertName(ECPV_REPNOM_ST ** repNomBuffer, WORD id, BYTE * ack);

/* Retourne un élément ECPV_REPNOM_ST du répertoire */
BOOL VRC_SetRepertName(ECPV_REPNOM_ST * repNomBuffer, WORD id, BYTE * ack);

/* WOAC */
/* ---- */

/* les numéros de portes pour WOAC */
BOOL VRC_GetWoacNum(BYTE ** numPortePtr, WORD id, BYTE * ack);
BOOL VRC_SetWoacNum(BYTE * numPortePtr, WORD id, BYTE * ack);

/* n° de série d'un élément id de la liste noire */
BOOL VRC_GetRepertBlackList(ECPV_BLACKLIST_ST ** blackListPtr, WORD id, BYTE * ack);
BOOL VRC_SetRepertBlackList(ECPV_BLACKLIST_ST * blackListPtr, WORD id, BYTE * ack);

BOOL VRC_GetBaseName(BYTE ** nameP, BYTE * ack);
BOOL VRC_GetBaseVersion(BYTE ** verP, BYTE * ack);
BOOL VRC_SetBaseName(BYTE * name, BYTE * ack);

BOOL VRC_GetPorteConfig(VRC_VIGIK_PORTE_ST ** portePtr, WORD id, BYTE * ack);
BOOL VRC_SetPorteConfig(VRC_VIGIK_PORTE_ST * portePtr, WORD id, BYTE * ack);

#endif
