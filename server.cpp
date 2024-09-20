// This autogenerated skeleton file illustrates how to build a server.
// You should copy it to another filename to avoid overwriting it.

#include "storage.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include "skip_list.h"

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using namespace ::storage_service;

class StorageHandler : virtual public StorageIf {
public:
    SkipList *skipList;

    StorageHandler() {
        skipList = new SkipList();
    }

    void insert(const std::string &key, const std::string &val) {
        printf("insert\n");
        skipList->insert(key, val);
    }

    void update_by_key(const std::string &key, const std::string &val) {
        skipList->update_by_key(key, val);
        printf("update_by_key\n");
    }

    void delete_by_key(const std::string &key) {
        skipList->delete_by_key(key);
        printf("delete_by_key\n");
    }

    void find_by_key(std::string &_return, const std::string &key) {
        Node *node = skipList->find_by_key(key);
        _return = node->val;
        printf("find_by_key\n");
    }

    void dump(const std::string &file_name) {
        skipList->dump(file_name);
        printf("dump\n");
    }

    void load(const std::string &file_name) {
        skipList->load(file_name);
        printf("load\n");
    }

};

int main(int argc, char **argv) {
    int port = 9090;
    ::std::shared_ptr<StorageHandler> handler(new StorageHandler());
    ::std::shared_ptr<TProcessor> processor(new StorageProcessor(handler));
    ::std::shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
    ::std::shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
    ::std::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

    TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
    server.serve();
    return 0;
}

