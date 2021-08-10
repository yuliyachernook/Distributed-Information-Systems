const dgram = require('dgram');
const fs = require('fs');
const config = require('./../config');

const timeServiceHandlers = {
    "time": getTimeHandler,
    "ready": checkServerReady,
    "ready-ok": coordinatorIsReady,
    "init-coordinator": initCoordinator,
    "re-init-coordinator": reInitCoordinator
};
const coordinatorFilePath = './../coordinator.json';

const self = module.exports = {

    initTimeServer: count => {
        const server = dgram.createSocket('udp4');
        server.ready = false;
        const timeServerConfig = config.timeService[`127.0.0.${count + 1}`];

        server.on('message', (message, client) => {
            console.log(`Server-${count} got: "${message}" from ${client.address}:${client.port}`);
            const handler = timeServiceHandlers[message];
            if (handler) {
                handler(server, client);
            }
        });

        server.on('error', err => {
            console.log(`Server-${count} error:\n${err.stack}`);
            server.close();
        });

        server.on('listening', () => {
            server.setBroadcast(true);
            const address = server.address();
            console.log(`Server-${count} is listening to ${address.address}:${address.port}`);

            // 3 is a highest rang for now
            if (timeServerConfig.rang === 3) {
                server.ready = true;
                self.setCoordinator(address.address, address.port);
                setTimeout(() => {
                    server.send('init-coordinator', config.timeService.port, config.timeService.broadcastHost);
                }, 1000);
            }
        });

        server.bind(config.timeService.port, timeServerConfig.host);

        setInterval(self.checkCoordinatorAvailable, config.timeService.checkCoordinatorInterval, server);

        return server;
    },

    getCoordinator: () => {
        return JSON.parse(fs.readFileSync(coordinatorFilePath).toString());
    },

    setCoordinator: (host, port) => {
        fs.writeFile(coordinatorFilePath, JSON.stringify({host, port}, null, '  '), err => {
            if (err) {
                console.log(`Error while saving current coordinator`);
            }
        })
    },

    checkCoordinatorAvailable: server => {
        server.coordinatorReady = false;
        server.coordinator = {
            host: self.getCoordinator().host,
            port: self.getCoordinator().port
        }
        console.log('Check coordinator is available');
        if (server.address().address !== server.coordinator.host) {
            server.send('ready', server.coordinator.port, server.coordinator.host);
            self.recheckCoordinatorAvailable(server, 1);
        }
    },

    recheckCoordinatorAvailable: (server, attempt) => {
        setTimeout(() => {
            if (!server.coordinatorReady && attempt < config.timeService.checkCoordinatorAttempts) {
                console.log('Recheck coordinator is available');
                server.send('ready', server.coordinator.port, server.coordinator.host);
                self.recheckCoordinatorAvailable(server, ++attempt);
            } else if (server.coordinatorReady) {
                console.log('Coordinator is available');
            } else {
                console.log('Coordinator is not available');
                server.send('re-init-coordinator', config.timeService.port, config.timeService.broadcastHost);
            }
        }, 300);
    }
};

function getTimeHandler(server, client) {
    if (server.ready) {
        server.send(new Date().toISOString(), client.port, client.address);
    }
}

function checkServerReady(server, client) {
    if (server.ready) {
        server.send('ready-ok', client.port, client.address);
    }
}

function coordinatorIsReady(server, client) {
    server.coordinatorReady = true;
}

function initCoordinator(server, client) {
    server.ready = true;
    server.coordinatorReady = true;
    server.coordinator = {
        host: client.address,
        port: client.port
    };
    console.log(`Initial coordinator: ${server.coordinator.host}:${server.coordinator.port}`);
}

function reInitCoordinator(server, client) {
    if (!server.coordinatorReady) {
        console.log('New coordinator suggestion');
        const myRang = config.timeService[server.address().address].rang;
        const clientRang = config.timeService[client.address].rang;
        if (myRang > clientRang) {
            console.log('My rang is higher');
            server.send('re-init-coordinator', config.timeService.port, config.timeService.broadcastHost);
        } else {
            console.log('Agree with a new coordinator');
            server.coordinatorReady = true;
            self.setCoordinator(client.address, client.port);
        }
    }
}
