/*
 * Copyright (C) 2006-2012 SIProp Project http://www.siprop.org/
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include "TcpServer.h"


TcpServer::TcpServer() {
    listener = epfd = 0;
}

TcpServer::~TcpServer() {
    if (listener != 0) stop();
}

void TcpServer::stop() {
    // close client
    map<int,int>::iterator itr;
    pthread_mutex_lock(&access_lock);
    for(itr = clientMap.begin(); itr != clientMap.end(); itr++ ) {
        close(itr->second);
    }
    clientMap.clear();
    pthread_mutex_unlock(&access_lock);

    // close server
    close(listener);
    close(epfd);

    listener = epfd = 0;
}

void TcpServer::die(const char* msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void TcpServer::deleteClient(int client) {
    pthread_mutex_lock(&access_lock);
    map<int, int>::iterator itr = clientMap.find(client);
    clientMap.erase(itr);
    pthread_mutex_unlock(&access_lock);
    printf("deleteClient:%d\n", client);
};


void TcpServer::setupSocket() {
    int one = 1;
    struct sockaddr_in sin;
    if ((this->listener = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        die("socket");
    }
    memset(&sin, 0, sizeof sin);

    sin.sin_family = AF_INET;
    // sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_addr.s_addr = inet_addr("127.0.0.1");
    sin.sin_port = htons(port);

    setsockopt(this->listener, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int));
    if (bind(this->listener, (struct sockaddr *) &sin, sizeof sin) < 0) {
        close(listener);
        die("bind");
    }
    if (listen(this->listener, MAX_BACKLOG) < 0) {
        close(listener);
        die("listen");
    }
}

void TcpServer::eventClient(int client, struct epoll_event ev) {
    char buffer[1024];
    int n = read(client, buffer, sizeof buffer);
    if (n < 0) {
        epoll_ctl(epfd, EPOLL_CTL_DEL, client, &ev);
        close(client);
        deleteClient(client);
    } else if (n == 0) {
        epoll_ctl(epfd, EPOLL_CTL_DEL, client, &ev);
        close(client);
        deleteClient(client);
    } else {
        // write(client, buffer, n);
    }
}

void TcpServer::eventServer(struct epoll_event ev) {
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof client_addr;

    int client = accept(this->listener, (struct sockaddr *) &client_addr, &client_addr_len);
    if (client < 0) {
        die("accept");
    }
    int flag = fcntl(this->listener, F_GETFL, 0);
    fcntl(this->listener, F_SETFL, flag | O_NONBLOCK);
    memset(&ev, 0, sizeof ev);
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = client;
    epoll_ctl(epfd, EPOLL_CTL_ADD, client, &ev);

    pthread_mutex_lock(&access_lock);
    clientMap.insert(map<int, int>::value_type(client, client));
    pthread_mutex_unlock(&access_lock);
    printf("accept new client:%d\n", client);
}

void TcpServer::writeToClient(int len, void *data) {
    map<int,int>::iterator itr;
    pthread_mutex_lock(&access_lock);

    for(itr = clientMap.begin(); itr != clientMap.end(); itr++ ) {
        int n = write(itr->second, data, len);
    }
    pthread_mutex_unlock(&access_lock);
}

void TcpServer::setupEpoll() {
    if ((epfd = epoll_create(MAX_EVENTS)) < 0) {
        die("epoll_create");
    }
    memset(&ev, 0, sizeof ev);
    ev.events = EPOLLIN;
    ev.data.fd = this->listener;
    epoll_ctl(epfd, EPOLL_CTL_ADD, this->listener, &ev);
}



//---------------------------------------------------------------------------
// main loop
//---------------------------------------------------------------------------
void TcpServer::serverLoop() {
    while(1) {
        int i;
        int nfd = epoll_wait(epfd, events, MAX_EVENTS, -1);
        for (i = 0; i < nfd; i++) {
            if (events[i].data.fd == this->listener) {
                eventServer(ev);
            } else {
                eventClient(events[i].data.fd, ev);
            }
        }
    }

}


//---------------------------------------------------------------------------
// thread operation
//---------------------------------------------------------------------------
void *TcpServer::mainThread(void *arg) {
    thread_args* tf_args = static_cast<thread_args*>(arg) ;
    TcpServer *tcpServer = tf_args->This;
    tcpServer->setupSocket();
    tcpServer->setupEpoll();
    tcpServer->serverLoop();
}

void TcpServer::start(int port) {
  this->port = port;
  pthread_mutex_init(&this->access_lock, NULL);
  pthread_create(&thread, NULL, &TcpServer::mainThread, new thread_args(this, 0));
}

