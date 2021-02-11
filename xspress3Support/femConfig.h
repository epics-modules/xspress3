/*
 * femConfig.h
 *
 *  Created on: Aug 3, 2011
 *      Author: mt47
 * hacked for Linux: WIH 19/11/2013
 */

#ifndef FEMCONFIG_H_
#define FEMCONFIG_H_

// Header for config structure, verified before it is used to configure FEM
#define CONFIG_MAGIC_WORD			0xFACE

// Struct for storage of FEM parameters (normally kept in EEPROM)
struct fem_config
{
	// Header (2 bytes)
	u_int16_t header;

	// Networking (18 bytes)
	u_int8_t net_mac[6];
	u_int8_t net_ip[4];
	u_int8_t net_nm[4];
	u_int8_t net_gw[4];

	// For hardware monitoring (2 bytes)
	u_int8_t temp_high_setpoint;
	u_int8_t temp_crit_setpoint;

	// Versioning information (8 bytes)
	u_int8_t sw_major_version;
	u_int8_t sw_minor_version;
	u_int8_t fw_major_version;
	u_int8_t fw_minor_version;
	u_int8_t hw_major_version;
	u_int8_t hw_minor_version;
	u_int8_t board_id;
	u_int8_t board_type;

	u_int8_t xor_checksum;

};


#endif /* FEMCONFIG_H_ */
