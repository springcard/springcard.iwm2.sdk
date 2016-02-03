#ifndef __IWM2_PROTO_MK2_H__
#define __IWM2_PROTO_MK2_H__

#define IWM2_MK2_PROTO_TYPE_SLAVE_TO_MASTER   0x80
#define IWM2_MK2_PROTO_TYPE_BLOCK_NUMBER_MASK 0x0F

#define IWM2_MK2_PROTO_TYPE_MASK              0x70

#define IWM2_MK2_PROTO_I_BLOCK                0x00
#define IWM2_MK2_PROTO_I_BLOCK_CHAINING       0x10

#define IWM2_MK2_PROTO_R_OK                   0x40
#define IWM2_MK2_PROTO_R_ACK                  0x50
#define IWM2_MK2_PROTO_R_NACK                 0x60
#define IWM2_MK2_PROTO_S_WAIT                 0x20
#define IWM2_MK2_PROTO_S_ENUM                 0x30

#define IWM2_MK2_PROTO_HELO                   0x40
#define IWM2_MK2_PROTO_HELO_OK                0x50
#define IWM2_MK2_PROTO_HELO_AUTH              0x70

#define IWM2_MK2_PROTO_I_S_BLOCK              0x20
#define IWM2_MK2_PROTO_I_S_BLOCK_CHAINING     0x30

#define IWM2_MK2_PROTO_VERSION                0x00

#define IWM2_MK2_CMD_GET_GLOBAL_STATUS        0x00

#define IWM2_MK2_CMD_GET_DEVICE_NAME          0x01
#define IWM2_MK2_CMD_GET_DEVICE_CAPABILITIES  0x02
#define IWM2_MK2_CMD_GET_DEVICE_SERNO         0x03

#define IWM2_MK2_CMD_PUT_DEVICE_NAME          0x01
#define IWM2_MK2_CMD_PUT_DEVICE_CAPABILITIES  0x02
#define IWM2_MK2_CMD_PUT_DEVICE_SERNO         0x03

#define IWM2_MK2_CMD_READ_INPUTS              0x04

#define IWM2_MK2_CMD_SET_RUNTIME_MODE         0x0A

#define IWM2_MK2_CMD_DO_RESET                 0x0B

#define IWM2_MK2_CMD_WRITE_CONFIG             0x0C
#define IWM2_MK2_CMD_ALTER_CONFIG             0x0D

#define IWM2_MK2_CMD_SET_OUTPUT               0x90
#define IWM2_MK2_CMD_SET_OUTPUT_W             0x9000

#define IWM2_MK2_CMD_CLEAR_OUTPUT             0xA0
#define IWM2_MK2_CMD_CLEAR_OUTPUT_W           0xA000

#define IWM2_MK2_CMD_SET_LEDS                 0xD0
#define IWM2_MK2_CMD_SET_LEDS_W               0xD000
#define IWM2_MK2_CMD_SET_BUZZER               0xD1
#define IWM2_MK2_CMD_SET_BUZZER_W             0xD100

#define IWM2_MK2_CMD_PUT_READER_HEAD          0x81

#define IWM2_MK2_CMD_PUT_TAMPERS              0x2F

#define IWM2_MK2_CMD_PUT_TAG                  0xB0
#define IWM2_MK2_CMD_PUT_TAG_INSERT_EJECT     0xB1
#define IWM2_MK2_CMD_PUT_PIN                  0xA0

#define IWM2_MK2_CMD_PUT_INPUT                0xC0

#define IWM_PACKET_PAYLOAD_SIZE               64

#endif
