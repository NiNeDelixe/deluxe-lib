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
    DECLARE_CLASS(MultiplayerHandler)

public:
    void init(std::ostream& output_stream = std::cout);
    void destroy();

    void startServer(std::ostream& output_stream = std::cout);
    void startClient(std::ostream& output_stream = std::cout);

private:
    bool m_is_server = true;
    std::shared_ptr<ENetHost> host;
};

#endif // MULTIPLAYER_MULTIPLAYER_HANDLER_HPP_
