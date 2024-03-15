#ifndef _ERR_EX_H__
#define _ERR_EX_H__
#ifdef __cplusplus
extern "C"{
#endif
#include <stdint.h>
void Error_Handler(void);
void assert_failed(uint8_t *file, uint32_t line);
#ifdef __cplusplus
}
#endif
#endif //#ifndef _ERR_EX_H__
