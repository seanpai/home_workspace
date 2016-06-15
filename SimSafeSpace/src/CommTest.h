#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

const static unsigned int STARTFRAME = (0xABCDABCD);
const static unsigned int ENDFRAME = (0x12341234);
const static unsigned int ACKFRAME = (0x0EC00EC0);

typedef struct CommTestFrame
{
	unsigned int StartFrame;
	unsigned int nCounter;
	char DummyData[1012];
	unsigned int EndFrame;
} CommTestFrame;

#ifdef __cplusplus
}
#endif