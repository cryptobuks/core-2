
#include "../streams.h"
#include "../Tools/Log.h"
#include "NetworkMessageHandler.h"
#include "BlockCache.h"
#include "../TxPool.h"
#include "../BlockStore.h"
#include "Network.h"
#include "../Config.h"

void NetworkMessageHandler::handleNetworkMessage(NetworkMessage *networkMessage, PeerInterfacePtr recipient) {

    BanList& banList = BanList::Instance();
    Log(LOG_LEVEL_INFO) << "NetworkMessageHandler::handleNetworkMessage()";

    if(networkMessage->body_length() == 0) {
        Log(LOG_LEVEL_ERROR) << "Received empty networkMessage";
        banList.appendBan(recipient->getIp(), BAN_INC_FOR_INVALID_MESSAGE);
        delete networkMessage;
        return;
    }
    uint8_t commandType = (uint8_t)networkMessage->body()[0];
    CDataStream s(SER_DISK, 1);
    s.write(networkMessage->body(), networkMessage->body_length());

    Log(LOG_LEVEL_INFO) << "NetworkMessageHandler::handleNetworkMessage() commandType: " << commandType;
    switch (commandType) {
        case ASK_FOR_BLOCKS_COMMAND: {
            AskForBlocks *askForBlocks = new AskForBlocks();
            try {
                s >> *askForBlocks;
            } catch (const std::exception& e) {
                Log(LOG_LEVEL_ERROR) << "Error while deserializing ASK_FOR_BLOCKS_COMMAND from peer: " << recipient->getIp()
                                     << " terminated with exception: " << e.what();
                banList.appendBan(recipient->getIp(), BAN_INC_FOR_INVALID_MESSAGE);
            }
            NetworkMessageHandler::handleAskForBlocks(askForBlocks, recipient);
            delete askForBlocks;
            break;
        }
        case ASK_FOR_BLOCK_COMMAND: {
            AskForBlock *askForBlock = new AskForBlock();
            try {
                s >> *askForBlock;
            } catch (const std::exception& e) {
                Log(LOG_LEVEL_ERROR) << "Error while deserializing ASK_FOR_BLOCK_COMMAND from peer: " << recipient->getIp()
                                     << " terminated with exception: " << e.what();
                banList.appendBan(recipient->getIp(), BAN_INC_FOR_INVALID_MESSAGE);
            }
            NetworkMessageHandler::handleAskForBlock(askForBlock, recipient);
            free(askForBlock->blockHeaderHash.data());
            delete askForBlock;
            break;
        }
        case ASK_FOR_PEERS_COMMAND: {
            AskForPeers *askForPeers = new AskForPeers();
            try {
                s >> *askForPeers;
            } catch (const std::exception& e) {
                Log(LOG_LEVEL_ERROR) << "Error while deserializing ASK_FOR_PEERS_COMMAND from peer: " << recipient->getIp()
                                     << " terminated with exception: " << e.what();
                banList.appendBan(recipient->getIp(), BAN_INC_FOR_INVALID_MESSAGE);
            }
            NetworkMessageHandler::handleAskForPeers(recipient);
            delete askForPeers;
            break;
        }
        case ASK_FOR_BLOCKCHAIN_HEIGHT_COMMAND: {
            NetworkMessageHandler::handleAskForBlockchainHeight(recipient);
            break;
        }
        case ASK_FOR_BEST_BLOCK_HEADER_COMMAND: {
            NetworkMessageHandler::handleAskForBestBlockHeader(recipient);
            break;
        }
        case ASK_FOR_VERSION_COMMAND: {
            NetworkMessageHandler::handleAskForVersion(recipient);
            break;
        }
        case ASK_FOR_STATUS_COMMAND: {
            NetworkMessageHandler::handleAskForStatus(recipient);
            break;
        }
        case ASK_FOR_DONATION_ADDRESS_COMMAND: {
            NetworkMessageHandler::handleAskForDonationAddress(recipient);
            break;
        }
        case TRANSMIT_TRANSACTIONS_COMMAND: {
            TransmitTransactions *transmitTransactions = new TransmitTransactions();
            try {
                s >> *transmitTransactions;
            } catch (const std::exception& e) {
                Log(LOG_LEVEL_ERROR) << "Error while deserializing TRANSMIT_TRANSACTIONS_COMMAND from peer: " << recipient->getIp()
                                     << " terminated with exception: " << e.what();
                banList.appendBan(recipient->getIp(), BAN_INC_FOR_INVALID_MESSAGE);
            }
            NetworkMessageHandler::handleTransmitTransactions(transmitTransactions, recipient);
            delete transmitTransactions;
            break;
        }
        case TRANSMIT_BLOCKS_COMMAND: {
            TransmitBlock *transmitBlocks = new TransmitBlock();
            try {
                s >> *transmitBlocks;
            } catch (const std::exception& e) {
                Log(LOG_LEVEL_ERROR) << "Error while deserializing TRANSMIT_BLOCKS_COMMAND from peer: " << recipient->getIp()
                                     << " terminated with exception: " << e.what();
                banList.appendBan(recipient->getIp(), BAN_INC_FOR_INVALID_MESSAGE);
            }
            NetworkMessageHandler::handleTransmitBlocks(transmitBlocks, recipient);
            delete transmitBlocks;
            break;
        }
        case TRANSMIT_PEERS_COMMAND: {
            TransmitPeers *transmitPeers = new TransmitPeers();
            try {
                s >> *transmitPeers;
            } catch (const std::exception& e) {
                Log(LOG_LEVEL_ERROR) << "Error while deserializing TRANSMIT_PEERS_COMMAND from peer: " << recipient->getIp()
                                     << " terminated with exception: " << e.what();
                banList.appendBan(recipient->getIp(), BAN_INC_FOR_INVALID_MESSAGE);
            }
            NetworkMessageHandler::handleTransmitPeers(transmitPeers, recipient);
            delete transmitPeers;
            break;
        }
        case TRANSMIT_BLOCKCHAIN_HEIGHT_COMMAND: {
            TransmitBlockchainHeight *transmitBlockchainHeight = new TransmitBlockchainHeight();
            try {
                s >> *transmitBlockchainHeight;
            } catch (const std::exception& e) {
                Log(LOG_LEVEL_ERROR) << "Error while deserializing TRANSMIT_BLOCKCHAIN_HEIGHT_COMMAND from peer: " << recipient->getIp()
                                     << " terminated with exception: " << e.what();
                banList.appendBan(recipient->getIp(), BAN_INC_FOR_INVALID_MESSAGE);
            }
            NetworkMessageHandler::handleTransmitBlockchainHeight(transmitBlockchainHeight, recipient);
            delete transmitBlockchainHeight;
            break;
        }
        case TRANSMIT_BEST_BLOCK_HEADER_COMMAND: {
            TransmitBestBlockHeader *transmitBestBlockHeader = new TransmitBestBlockHeader();
            try {
                s >> *transmitBestBlockHeader;
            } catch (const std::exception& e) {
                Log(LOG_LEVEL_ERROR) << "Error while deserializing TRANSMIT_BEST_BLOCK_HEADER_COMMAND from peer: " << recipient->getIp()
                                     << " terminated with exception: " << e.what();
                banList.appendBan(recipient->getIp(), BAN_INC_FOR_INVALID_MESSAGE);
            }
            NetworkMessageHandler::handleTransmitBestBlockHeader(transmitBestBlockHeader, recipient);
            delete transmitBestBlockHeader;
            break;
        }
        case TRANSMIT_VERSION_COMMAND: {
            TransmitVersion *transmitVersion = new TransmitVersion();
            try {
                s >> *transmitVersion;
            } catch (const std::exception& e) {
                Log(LOG_LEVEL_ERROR) << "Error while deserializing TRANSMIT_VERSION_COMMAND from peer: " << recipient->getIp()
                                     << " terminated with exception: " << e.what();
                banList.appendBan(recipient->getIp(), BAN_INC_FOR_INVALID_MESSAGE);
            }
            NetworkMessageHandler::handleTransmitVersion(transmitVersion, recipient);
            delete transmitVersion;
            break;
        }
        case TRANSMIT_STATUS_COMMAND: {
            TransmitStatus *transmitStatus = new TransmitStatus();
            try {
                s >> *transmitStatus;
            } catch (const std::exception& e) {
                Log(LOG_LEVEL_ERROR) << "Error while deserializing TRANSMIT_STATUS_COMMAND from peer: " << recipient->getIp()
                                     << " terminated with exception: " << e.what();
                banList.appendBan(recipient->getIp(), BAN_INC_FOR_INVALID_MESSAGE);
            }
            NetworkMessageHandler::handleTransmitStatus(transmitStatus, recipient);
            delete transmitStatus;
            break;
        }
        case TRANSMIT_LEAVE_COMMAND: {
            NetworkMessageHandler::handleTransmitLeave(recipient);
            break;
        }
        case TRANSMIT_DONATION_ADDRESS_COMMAND: {
            TransmitDonationAddress *transmitDonationAddress = new TransmitDonationAddress();
            try {
                s >> *transmitDonationAddress;
            } catch (const std::exception& e) {
                Log(LOG_LEVEL_ERROR) << "Error while deserializing TRANSMIT_DONATION_ADDRESS_COMMAND from peer: " << recipient->getIp()
                                     << " terminated with exception: " << e.what();
                banList.appendBan(recipient->getIp(), BAN_INC_FOR_INVALID_MESSAGE);
            }
            NetworkMessageHandler::handleTransmitDonationAddress(transmitDonationAddress, recipient);
            delete transmitDonationAddress;
            break;
        }
        default:
            Log(LOG_LEVEL_ERROR) << "Received networkMessage with unknown commandType: " << commandType;
            break;
    }
    
    s.clear();
    delete networkMessage;
}

void NetworkMessageHandler::handleAskForBlocks(AskForBlocks *askForBlocks, PeerInterfacePtr recipient) {

    Chain &chain = Chain::Instance();
    BlockStore *blockStore = new BlockStore();

    uint64_t startBlockHeight = askForBlocks->startBlockHeight;
    uint64_t endBlockHeight = startBlockHeight + (askForBlocks->count - 1);


    Log(LOG_LEVEL_INFO) << "Peer asked for " << askForBlocks->count << " starting from " << askForBlocks->startBlockHeight;

    for(uint64_t blockHeight = startBlockHeight; blockHeight <= endBlockHeight; blockHeight++) {

        TransmitBlock* transmitBlock = new TransmitBlock();
        BlockHeader* blockHeader = chain.getBlockHeader(blockHeight);

        if(blockHeader != nullptr) {

            std::vector<unsigned char> headerHash = blockHeader->getHeaderHash();
            transmitBlock->block = blockStore->getRawBlockVector(headerHash);

            CDataStream s(SER_DISK, 1);
            s << *transmitBlock;

            NetworkMessage msg;
            msg.body_length(s.size());
            std::memcpy(msg.body(), s.data(), s.size());
            s.clear();
            msg.encode_header();

            recipient->deliver(msg);
        }
        
        delete blockHeader;
        delete transmitBlock;
    }
}

void NetworkMessageHandler::handleAskForBlock(AskForBlock *askForBlock, PeerInterfacePtr recipient) {

    BlockStore* blockStore = new BlockStore();

    TransmitBlock* transmitBlock = new TransmitBlock();

    if(askForBlock->blockHeaderHash.size() > 0) {
        Log(LOG_LEVEL_INFO) << "Peer asked for Block with hash" << askForBlock->blockHeaderHash;
        transmitBlock->block = blockStore->getRawBlockVector(askForBlock->blockHeaderHash);
    } else {
        Chain &chain = Chain::Instance();
        Log(LOG_LEVEL_INFO) << "Peer asked for Block with height" << askForBlock->blockHeight;
        BlockHeader* blockHeader = chain.getBlockHeader(askForBlock->blockHeight);
        if(blockHeader != nullptr) {
            std::vector<unsigned char> headerHash = blockHeader->getHeaderHash();
            transmitBlock->block = blockStore->getRawBlockVector(headerHash);
        }
    }

    if(transmitBlock->block.size() <= 0) {
        Log(LOG_LEVEL_INFO) << "Peer asked for Block that couldn't be located";
        return;
    }

    CDataStream s(SER_DISK, 1);
    s << *transmitBlock;

    free(transmitBlock->block.data());
    delete transmitBlock;
    
    NetworkMessage msg;
    msg.body_length(s.size());
    std::memcpy(msg.body(), s.data(), msg.body_length());
    msg.encode_header();

    recipient->deliver(msg);
}

void NetworkMessageHandler::handleAskForPeers(PeerInterfacePtr recipient) {
    Peers &peers = Peers::Instance();
    TransmitPeers transmitPeers;
    auto randomPeers = peers.getRandomPeers(8);
    for(auto peer: randomPeers) {
        transmitPeers.ipList.emplace_back(peer->getIp());
    }
    recipient->deliver(NetworkMessageHelper::serializeToNetworkMessage(transmitPeers));
}

void NetworkMessageHandler::handleAskForBlockchainHeight(PeerInterfacePtr recipient) {
    Chain &chain = Chain::Instance();

    TransmitBlockchainHeight* transmitBlockchainHeight = new TransmitBlockchainHeight();
    transmitBlockchainHeight->height = chain.getCurrentBlockchainHeight();
    if(chain.getBestBlockHeader() != nullptr) {
        transmitBlockchainHeight->bestHeaderHash = chain.getBestBlockHeader()->getHeaderHash();
    }

    recipient->deliver(NetworkMessageHelper::serializeToNetworkMessage(*transmitBlockchainHeight));
}

void NetworkMessageHandler::handleAskForBestBlockHeader(PeerInterfacePtr recipient) {

}

void NetworkMessageHandler::handleAskForVersion(PeerInterfacePtr recipient) {
    TransmitVersion* transmitVersion =  new TransmitVersion();
    transmitVersion->version = VERSION_16BITS;

    recipient->deliver(NetworkMessageHelper::serializeToNetworkMessage(*transmitVersion));
}

void NetworkMessageHandler::handleAskForStatus(PeerInterfacePtr recipient) {
    Config &config = Config::Instance();
    TransmitDonationAddress* transmitDonationAddress = new TransmitDonationAddress();
    transmitDonationAddress->donationAddress = config.getDonationAddress();

    recipient->deliver(NetworkMessageHelper::serializeToNetworkMessage(*transmitDonationAddress));
}

void NetworkMessageHandler::handleAskForDonationAddress(PeerInterfacePtr recipient) {

}

void NetworkMessageHandler::handleTransmitTransactions(TransmitTransactions *transmitBlocks, PeerInterfacePtr recipient) {
    TxPool &txPool = TxPool::Instance();

    for(Transaction tx : transmitBlocks->transactions) {
        txPool.appendTransaction(tx);
    }
}

void NetworkMessageHandler::handleTransmitBlocks(TransmitBlock *transmitBlocks, PeerInterfacePtr recipient) {
    BlockCache& blockCache = BlockCache::Instance();
    Chain& chain = Chain::Instance();
    Network& network = Network::Instance();
    Block block;

    try {
        CDataStream s(SER_DISK, 1);
        s.write((const char*)transmitBlocks->block.data(), transmitBlocks->block.size());
        s >> block;
    } catch (const std::exception& e) {
        Log(LOG_LEVEL_ERROR) << "Error while deserializing block from peer: " << recipient->getIp()
                             << ", terminated with exception: " << e.what()
                             << ", transmitted block size:" << (uint64_t)transmitBlocks->block.size()
                             << ", content:"
                             << transmitBlocks->block;
        BanList& banList = BanList::Instance();
        banList.appendBan(recipient->getIp(), BAN_INC_FOR_INVALID_BLOCK);
        return;
    }

    Log(LOG_LEVEL_INFO) << "received block: " << block.getHeader()->getHeaderHash() << ", height: " << block.getHeader()->getBlockHeight();

    //verify if we asked for the block
    if(!blockCache.verifyAskedFor(recipient->getIp(), block.getHeader()->getHeaderHash(), block.getHeader()->getBlockHeight())) {
        BanList& banList = BanList::Instance();
        Log(LOG_LEVEL_INFO) << "node:" << recipient->getIp() << "sent an unwanted block";
        banList.appendBan(recipient->getIp(), BAN_INC_FOR_UNWANTED_BLOCK);
        return;
    }

    blockCache.appendBlock(recipient, &block);
}

void NetworkMessageHandler::handleTransmitPeers(TransmitPeers *transmitPeers, PeerInterfacePtr recipient) {
    Peers& peers = Peers::Instance();
    if(transmitPeers->ipList.size() > 10) {
        Log(LOG_LEVEL_WARNING) << "Peer: " << recipient->getIp() << " has transmitted too many peers";
        return;
    }

    for(std::string ip : transmitPeers->ipList) {
        if(ip == Network::myIP) {
            Log(LOG_LEVEL_INFO) << "Cannot add ip: " << ip << " because it is your own IP";
            continue;
        }
        
        auto io_service = std::make_shared<boost::asio::io_service>();
        tcp::resolver resolver(*io_service);

        auto work = std::make_shared<boost::asio::io_service::work>(*io_service);

        Log(LOG_LEVEL_INFO) << "ip: " << ip;
        auto endpoint_iterator = resolver.resolve({ip, NET_PORT});

        auto peer = std::make_shared<PeerClient>(io_service, endpoint_iterator, work);

        peer->setBlockHeight(0);
        peer->setIp(ip);
        if(peers.appendPeer(peer->get())) {
            peer->do_connect();

            auto io_service_run = [io_service]() {
                try{
                    io_service->run();
                    //io_service->stop();
                    Log(LOG_LEVEL_INFO) << "io_service terminated";
                }
                catch (const std::exception& e) {
                    Log(LOG_LEVEL_ERROR) << "io_service.run terminated with: " << e.what();
                }
            };
            std::thread t(io_service_run);
            t.detach();

            Chain& chain = Chain::Instance();

            // transmit our own block height
            TransmitBlockchainHeight *transmitBlockchainHeight = new TransmitBlockchainHeight();
            transmitBlockchainHeight->height = chain.getCurrentBlockchainHeight();
            peer->deliver(NetworkMessageHelper::serializeToNetworkMessage(*transmitBlockchainHeight));

            //ask for blockheight
            AskForBlockchainHeight askForBlockchainHeight;
            peer->deliver(NetworkMessageHelper::serializeToNetworkMessage(askForBlockchainHeight));

            //ask for donation Address
            AskForDonationAddress askForDonationAddress;
            peer->deliver(NetworkMessageHelper::serializeToNetworkMessage(askForDonationAddress));
        } else {
            peer->close();
        }
    }
}

void NetworkMessageHandler::handleTransmitBlockchainHeight(TransmitBlockchainHeight *transmitBlockchainHeight, PeerInterfacePtr recipient) {

    Log(LOG_LEVEL_INFO) << "NetworkMessageHandler::handleTransmitBlockchainHeight()";
    Chain &chain = Chain::Instance();
    uint32_t myHeight = chain.getCurrentBlockchainHeight();

    recipient->setBlockHeight((uint32_t)transmitBlockchainHeight->height);
    Log(LOG_LEVEL_INFO) << "recipient: " << recipient->getIp() << ", height:" << recipient->getBlockHeight();

    // if we don't have the block(s) ask for it/them

    // if there are less than 10 blocks difference just ask for each of them
    if(transmitBlockchainHeight->height > myHeight && (myHeight + 10 > transmitBlockchainHeight->height)) {

        std::vector<uint32_t> askedBlockHeights;
        for(uint32_t i = myHeight; i <= transmitBlockchainHeight->height; i++) {
            askedBlockHeights.emplace_back(i);
        }
        BlockCache& blockCache = BlockCache::Instance();
        blockCache.insertInBlockHeightAskedMap(recipient->getIp(), askedBlockHeights);

        AskForBlocks askForBlocks;
        askForBlocks.startBlockHeight = myHeight;
        askForBlocks.count = transmitBlockchainHeight->height - myHeight + 1;

        recipient->deliver(
                NetworkMessageHelper::serializeToNetworkMessage(askForBlocks)
        );
    // if the difference is to big start new sync procedure
    } else if(transmitBlockchainHeight->height > myHeight) {
        Network::synced = false;
        Network &network = Network::Instance();
        network.syncBlockchain();
    }
}

void NetworkMessageHandler::handleTransmitBestBlockHeader(TransmitBestBlockHeader *transmitBestBlockHeader, PeerInterfacePtr recipient) {

}

void NetworkMessageHandler::handleTransmitVersion(TransmitVersion *transmitVersion, PeerInterfacePtr recipient) {
    recipient->setVersion(transmitVersion->version);
}

void NetworkMessageHandler::handleTransmitStatus(TransmitStatus *transmitStatus, PeerInterfacePtr recipient) {
    std::string ip = recipient->getIp();
}

void NetworkMessageHandler::handleTransmitLeave(PeerInterfacePtr recipient) {
    Peers &peers = Peers::Instance();
    peers.disconnect(recipient.get()->getIp());
}

void NetworkMessageHandler::handleTransmitDonationAddress(TransmitDonationAddress* transmitDonationAddress, PeerInterfacePtr recipient) {
    if(!transmitDonationAddress->donationAddress.empty()) {
        recipient->setDonationAddress(transmitDonationAddress->donationAddress);
    }
}
