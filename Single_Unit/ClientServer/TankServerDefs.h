#ifndef __tankServerDefs
#define __tankServerDefs

#define CLIENT_DISCONNECT          0
#define CLIENT_FIND_SU_DIR         1
#define CLIENT_FIND_TANK           2
#define CLIENT_FIND_TANKS          3
#define CLIENT_FIND_POOLING        4
#define CLIENT_FIND_SPIKELIST      5
#define CLIENT_FIND_SPIKELISTS     6

#define SERVER_SU_DIR_FOUND       40
#define SERVER_SU_DIR_NOT_FOUND   41

#define SERVER_TANK_FOUND         50
#define SERVER_TANK_NOT_FOUND     51

#define SERVER_POOL_FOUND         60
#define SERVER_POOL_NOT_FOUND     61
#define SERVER_POOL_ERROR         62

#define SERVER_SPIKES_FOUND       70
#define SERVER_SPIKES_NOT_FOUND   71

#define SOCKET_PING              100

#endif