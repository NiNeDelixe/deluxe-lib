#ifndef MULTIPLAYER_MULTIPLAYER_HANDLER_HPP_
#define MULTIPLAYER_MULTIPLAYER_HANDLER_HPP_

#include "deluxe_lib/core/core.hpp"

#include <iostream>
#include <memory>
#include <stdio.h>

#define MAX_CLIENTS 32

#define ENET_IMPLEMENTATION
#include <enet.h>


class MultiplayerHandler
{
    DL_SIMPLE_DECLARE_CLASS(MultiplayerHandler)

public:
    MultiplayerHandler(std::ostream& output_stream = std::cout);

public:
    void init();
    void destroy();

    void startServer();
    void startClient();

private:
    bool m_is_server = true;
    std::shared_ptr<ENetHost> host;
    std::ostream& output_stream;
};

#endif // MULTIPLAYER_MULTIPLAYER_HANDLER_HPP_
