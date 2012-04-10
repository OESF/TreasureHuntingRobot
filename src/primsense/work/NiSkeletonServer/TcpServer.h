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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#include <map>

#define MAX_BACKLOG 10
#define MAX_EVENTS 100


using namespace std;

class TcpServer {
    public:
        TcpServer();
        ~TcpServer();

        void start(int port);
        void writeToClient(int len, void *data);
        void stop();

    private:
        int port, listener, epfd;
        map<int, int> clientMap;
        struct epoll_event ev;
        struct epoll_event events[MAX_EVENTS];

        pthread_t thread;
        pthread_mutex_t access_lock;
        pthread_cond_t access_threshold;

        static void *mainThread(void *arg);
        void die(const char* msg);
        void setupSocket();
        void eventServer(struct epoll_event ev);
        void eventClient(int client, struct epoll_event ev);
        void setupEpoll();
        void serverLoop();
        void deleteClient(int client);
        struct thread_args {
            TcpServer *This;
            void* actual_arg ;
            thread_args(TcpServer* t, void* p ) : This(t), actual_arg(p) {};
        };
};
