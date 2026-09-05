#ifndef MULTIPLAYER_MULTIPLAYER_HANDLER_HPP_
#define MULTIPLAYER_MULTIPLAYER_HANDLER_HPP_

#include "deluxe_lib/core/core.hpp"

#include <iostream>
#include <memory>
#include <stdio.h>

#define MAX_CLIENTS 32

#define ENET_IMPLEMENTATION
#include <enet.h>

#include "deluxe_lib/network/multiplayer/IDataPacket.hpp"


class MultiplayerHandler
{
    DL_DELETE_COPY(MultiplayerHandler)

public:
    MultiplayerHandler(std::ostream& output_stream = std::cout);

public:
    void init();
    void destroy();

    void startServer();
    void startClient();

    template<class DATAPACKETCLASS>
    void sendPacket();

private:
    bool m_is_server = true;
    std::shared_ptr<ENetHost> host;
    std::ostream& output_stream;
};


template<class DATAPACKETCLASS> 
inline void MultiplayerHandler::sendPacket() 
{
    std::shared_ptr<IDataPacket> packet = std::make_shared<DATAPACKETCLASS>();

    packet;
}


#endif // MULTIPLAYER_MULTIPLAYER_HANDLER_HPP_