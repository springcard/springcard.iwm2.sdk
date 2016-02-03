#ifndef __COMMON_ENDIAN_H__
#define __COMMON_ENDIAN_H__

#define SWAP_W(x)  (((x<<8)&0xFF00) | ((x>>8)&0x00FF))
#define SWAP_DW(x) (((x<<24)&0xFF000000) | ((x<<8)&0x00FF0000) | ((x>>8)&0x0000FF00) | ((x>>24)&0x000000FF))

#if defined(BIG_ENDIAN) && defined(LITTLE_ENDIAN)
  #error YOU CAN'T define both LITTLE_ENDIAN and BIG_ENDIAN
#endif

#ifdef LITTLE_ENDIAN
  #define LSB_FIRST_W(x)  (x)
  #define MSB_FIRST_W(x)  (SWAP_W(x))
  #define LSB_FIRST_DW(x) (x)
  #define MSB_FIRST_DW(x) (SWAP_DW(x))  
  
  #define HOST_TO_USB_W(x)  (x)
  #define USB_TO_HOST_W(x)  (x)
  #define HOST_TO_USB_DW(x) (x)
  #define USB_TO_HOST_DW(x) (x)  

  #define HOST_TO_NET_W(x)  (SWAP_W(x))
  #define NET_TO_HOST_W(x)  (SWAP_W(x))
  
#endif

#ifdef BIG_ENDIAN
  #define MSB_FIRST_W(x)  (x)
  #define LSB_FIRST_W(x)  (SWAP_W(x))
  #define MSB_FIRST_DW(x) (x)
  #define LSB_FIRST_DW(x) (SWAP_DW(x))  
  
  #define HOST_TO_USB_W(x)  (SWAP_W(x))
  #define USB_TO_HOST_W(x)  (SWAP_W(x))
  #define HOST_TO_USB_DW(x) (SWAP_DW(x))
  #define USB_TO_HOST_DW(x) (SWAP_DW(x))

  #define HOST_TO_NET_W(x)  (SWAP_W(x))
  #define NET_TO_HOST_W(x)  (SWAP_W(x))
#endif

#endif
