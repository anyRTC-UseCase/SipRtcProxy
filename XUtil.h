// XUtil.h: interface for the XUtil class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __XUTIL_H__
#define __XUTIL_H__

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <stdint.h>
#include <string>
#include <vector>

uint32_t XGetTimestamp(void);
int64_t XGetUtcTimestamp(void);
uint16_t XGetLastSequence(uint16_t usSequence1,uint16_t usSequence2);
uint32_t XGenerateSSRC(void);
void XSleep(uint32_t ulMS);

int XSplitChar(const std::string& source, char delimiter,std::vector<std::string>* fields);

#endif 