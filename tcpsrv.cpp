#include "tcp_server.h"
#include "tcp_client.h"
#include "network_utils.h"
#include <arpa/inet.h>

#include "CommandParser/libcli.h"
#include "CommandParser/cmdtlv.h"

#define TCP_SRV_CREATE 1
#define TCP_SRV_START 2

static void PrintClient(const TcpClient* cli) {
  printf("[%s, %d]\n", network_convert_ip_n_to_p(htonl(cli->ip), 0),
         htons(cli->port));
}

static void PrintServer(const TcpServer* srv) {
  printf("[%s, %d]\n", network_convert_ip_n_to_p(srv->ip, 0), htons(srv->port));
}

static void ConnectedListener(const TcpServer* srv, const TcpClient* cli) {
  PrintServer(srv);
  printf("client connected: ");
  PrintClient(cli);
}

static void DisconnectedListener(const TcpServer* srv, const TcpClient* cli) {}

static void ReceivedListener(const TcpServer*, const TcpClient*,
                             unsigned char* msg, uint16_t len) {
  printf("%s() bytes received: %d, msg: %s\n", __FUNCTION__, len, msg);
}

int CfgTcpSrvHandler(param_t* param, ser_buff_t* s_buf, op_mode enable) {
  // printf("%s\n", __FUNCTION__);

  int cmd;

  cmd = EXTRACT_CMD_CODE(s_buf);

  switch (cmd) {
    case TCP_SRV_CREATE:
      printf("create invoked\n");
      break;

    case TCP_SRV_START:
      printf("start invoked\n");
      break;
    default:
      break;   
  }

  return 0;
}

static void CliBuildConfigTree(void) {
  /* config tcp-srv <name> */
  param_t* cfg_hook = libcli_get_config_hook();

  {
    /* config tcp-srv ... */
    static param_t tcp_srv;
    init_param(&tcp_srv, CMD, "tcp-srv", NULL, NULL, INVALID, NULL,
               "config tcp-srv");
    libcli_register_param(cfg_hook, &tcp_srv);
    {
      /* config tcp-srv <name> */
      static param_t name;
      init_param(&name, LEAF, NULL, CfgTcpSrvHandler, NULL, STRING, "name",
                 "Name");
      libcli_register_param(&tcp_srv, &name);
      set_param_cmd_code(&name, TCP_SRV_CREATE);
      {
        /* config tcp-srv <name> start */
        static param_t start;
        init_param(&start, CMD, "start", CfgTcpSrvHandler, NULL, INVALID, NULL,
                   "Start");
        libcli_register_param(&name, &start);
        set_param_cmd_code(&start, TCP_SRV_START);
      }
    }
  }
}

static void CliBuildShowTree(void) {}

static void CliBuild(void) {
  CliBuildConfigTree();
  CliBuildShowTree();
}

int main(int argc, char** argv) {
#if 0  
  TcpServer* srv1 = new TcpServer("127.0.0.1", 40000, "tcp server1");

  srv1->RegisterListener(ConnectedListener, DisconnectedListener,
                         ReceivedListener);

  srv1->Start();
  scanf("\n");

  srv1->Display();
#endif

  init_libcli();

  CliBuild();

  start_shell();

  return 0;
}