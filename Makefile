# Copyright (c) 1999 Apple Computer, Inc.  All rights reserved.
#  

TARGET = ./SRProxy
C++ = g++
CC = gcc
INCLUDE_FLAG = -include
INCPATH = -I./include
LIBPATH = -lpthread -lz -L./so -lXUdpRpc -lARtcStrm -lSipCall -lXAudCodecSo -lARtmKit -lRtxpChan -lboringssl
CCFLAGS += -Wall -D_GNU_SOURCE -DNDEBUG -O2 -DPOSIX -DNO_DL -DFEATURE_ENABLE_SSL -DLOGGING=1 -D__STDC_FORMAT_MACROS -D__STDC_CONSTANT_MACROS -std=c++11
LFLAGS	= -w -lrt -ldl 

# EACH DIRECTORY WITH HEADERS MUST BE APPENDED IN THIS MANNER TO THE CCFLAGS

C++FLAGS = $(CCFLAGS)

CFILES = 

CPPFILES = \
	./src/config.cpp \
    ./src/main.cpp \
    ./src/RtcCallToSip.cpp \
    ./src/RtcCall.cpp \
    ./src/SipCallToRtc.cpp \
    ./src/SipRtcMgr.cpp \
    ./src/SipRtcStats.cpp \
    ./src/Timer10Ms.cpp \
    ./src/XAutoLock.cpp \
    ./src/XCritSec.cpp \
    ./src/XThreadBase.cpp \
    ./src/XUtil.cpp
		
CCFILES = 
		

all: $(TARGET)

$(TARGET): $(CFILES:.c=.o) $(CCFILES:.cc=.o) $(CPPFILES:.cpp=.o)
	$(CXX) $(LFLAGS) $(CFILES:.c=.o) $(CCFILES:.cc=.o) $(CPPFILES:.cpp=.o) -o $(TARGET) $(LIBPATH)

install: 
	install -m 664 SRProxy ../out/Linux

clean:
	rm -f $(TARGET) $(CFILES:.c=.o) $(CCFILES:.cc=.o) $(CPPFILES:.cpp=.o)

.SUFFIXES: .cpp .cc .c .o

.cpp.o:
	$(C++) -c -o $*.o $(DEFINES) $(C++FLAGS) $*.cpp $(INCPATH)
	
.cc.o:
	$(C++) -c -o $*.o $(DEFINES) $(C++FLAGS) $*.cc $(INCPATH)

.c.o:
	$(CC) -c -o $*.o $(DEFINES) $(CCFLAGS) $*.c $(INCPATH)

