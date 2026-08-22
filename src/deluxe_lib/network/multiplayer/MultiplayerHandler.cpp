#include "deluxe_lib/network/multiplayer/MultiplayerHandler.hpp"

void MultiplayerHandler::init(std::ostream& output_stream) 
{
    if (enet_initialize () != 0) 
    {
        output_stream << "An error occurred while initializing ENet." << std::endl;
        return;
    }

    startServer(output_stream);
    startClient(output_stream);
}

void MultiplayerHandler::destroy() 
{
    enet_host_destroy(host.get());
    enet_deinitialize();
}

void MultiplayerHandler::startServer(std::ostream& output_stream) 
{
    if (!m_is_server)
    {
        return;
    }

    ENetAddress address = {0};

    address.host = ENET_HOST_ANY; /* Bind the server to the default localhost.     */
    address.port = 7777; /* Bind the server to port 7777. */

    host = std::shared_ptr<ENetHost>(enet_host_create(&address, MAX_CLIENTS, 2, 0, 0));

    if (host == NULL) {
        output_stream << "An error occurred while trying to create an ENet server host." << std::endl;
        return;
    }

    output_stream << "Started a server..." << std::endl;

    ENetEvent event;

    /* Wait up to 1000 milliseconds for an event. (WARNING: blocking) */
    while (enet_host_service(host.get(), &event, 1000) > 0) {
        switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT:
                output_stream << "A new client connected from " /*<< std::hex << event.peer->address.host*/ << ":" << std::dec << event.peer->address.port << std::endl;
                /* Store any relevant client information here. */
                event.peer->data = (void*)"Client information";
                break;

            case ENET_EVENT_TYPE_RECEIVE:
                output_stream << "A packet of length " << event.packet->dataLength << " containing " << event.packet->data << " was received from " << event.peer->data << " on channel " << event.channelID << std::endl;
                /* Clean up the packet now that we're done using it. */
                enet_packet_destroy (event.packet);
                break;

            case ENET_EVENT_TYPE_DISCONNECT:
                output_stream << event.peer->data << " disconnected." << std::endl;
                /* Reset the peer's client information. */
                event.peer->data = NULL;
                break;

            case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
                output_stream << event.peer->data << " disconnected due to timeout." << std::endl;
                /* Reset the peer's client information. */
                event.peer->data = NULL;
                break;

            case ENET_EVENT_TYPE_NONE:
                break;
        }
    }


    return ;
    
}

void MultiplayerHandler::startClient(std::ostream& output_stream) 
{
    if (m_is_server)
    {
        return;
    }

    host = { 0 };
    host = std::shared_ptr<ENetHost>(enet_host_create(NULL /* create a client host */,
        1 /* only allow 1 outgoing connection */,
        2 /* allow up 2 channels to be used, 0 and 1 */,
        0 /* assume any amount of incoming bandwidth */,
        0 /* assume any amount of outgoing bandwidth */));
    if (host == NULL) {
        output_stream << "An error occurred while trying to create an ENet client host." << std::endl;
        exit(EXIT_FAILURE);
    }

    ENetAddress address = { 0 };
    ENetEvent event = {  };
    ENetPeer* peer = { 0 };
    /* Connect to some.server.net:1234. */
    enet_address_set_host(&address, "127.0.0.1");
    address.port = 7777;
    /* Initiate the connection, allocating the two channels 0 and 1. */
    peer = enet_host_connect(host.get(), &address, 2, 0);
    if (peer == NULL) {
        output_stream << "No available peers for initiating an ENet connection." << std::endl;
        exit(EXIT_FAILURE);
    }
    /* Wait up to 5 seconds for the connection attempt to succeed. */
    if (enet_host_service(host.get(), &event, 5000) > 0 &&
        event.type == ENET_EVENT_TYPE_CONNECT) {
        output_stream << "Connection to some.server.net:1234 succeeded." << std::endl;
    } else {
        /* Either the 5 seconds are up or a disconnect event was */
        /* received. Reset the peer in the event the 5 seconds   */
        /* had run out without any significant event.            */
        enet_peer_reset(peer);
        output_stream << "Connection to some.server.net:1234 failed." << std::endl;
    }

    // Receive some events
    enet_host_service(host.get(), &event, 5000);

    // Disconnect
    enet_peer_disconnect(peer, 0);

    uint8_t disconnected = false;
    /* Allow up to 3 seconds for the disconnect to succeed
    * and drop any packets received packets.
    */
    while (enet_host_service(host.get(), &event, 3000) > 0) {
        switch (event.type) {
        case ENET_EVENT_TYPE_RECEIVE:
            enet_packet_destroy(event.packet);
            break;
        case ENET_EVENT_TYPE_DISCONNECT:
            output_stream << "Disconnection succeeded." << std::endl;
            disconnected = true;
            break;
        }
    }

    // Drop connection, since disconnection didn't successed
    if (!disconnected) {
        enet_peer_reset(peer);
    }
}
