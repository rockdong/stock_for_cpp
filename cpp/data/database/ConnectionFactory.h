#ifndef DATA_CONNECTION_FACTORY_H
#define DATA_CONNECTION_FACTORY_H

#include "IConnection.h"

namespace data {

class ConnectionFactory {
public:
    static IConnection& createConnection();
};

} // namespace data

#endif // DATA_CONNECTION_FACTORY_H