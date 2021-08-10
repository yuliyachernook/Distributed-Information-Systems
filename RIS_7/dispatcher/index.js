const dgram = require('dgram');
const serviceHelpers = require('./../service-helpers');
const server = dgram.createSocket('udp4');
const config = require('./../config');

const clients = {};
const requestExceptions = [
    'init-coordinator',
    're-init-coordinator'
];


server.on('message', (message, client) => {
    if (requestExceptions.find(exception => exception === message.toString())) {
        return;
    }
    console.log(`Dispatcher got: "${message}" from ${client.address}:${client.port}`);
    const coordinator = serviceHelpers.getCoordinator();
    if (coordinator.host === client.address) {
        console.log('Got a server\'s response');
        const sourceClient = clients[coordinator.host];
        server.send(message, sourceClient.port, sourceClient.ip);
        delete clients[coordinator.host];
    } else {
        console.log('Got a client\'s request');
        clients[coordinator.host] = {ip: client.address, port: client.port, command: message};
        server.send(message, coordinator.port, coordinator.host);
    }
});

server.on('listening', () => {
    const address = server.address();
    console.log(`Dispatcher is listening to ${address.address}:${address.port}`);
});

server.on('error', err => {
    console.log(`Dispatcher error:\n${err.stack}`);
    server.close();
});

server.bind(config.dispatcher.port, config.dispatcher.host);
