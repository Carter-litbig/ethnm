#include "tcp_server.h"
#include "tcp_client.h"
#include "network_utils.h"
#include <arpa/inet.h>

#include "CommandParser/libcli.h"
#include "CommandParser/cmdtlv.h"

#define TCP_SRV_CREATE  1
#define TCP_SRV_START   2
#define TCP_SRV_SHOW    3

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

std::list<TcpServer*> srv_list;
uint16_t default_port = 40000;
const char* default_ip = "127.0.0.1";

static TcpServer* CliLookupServer(std::string name) {
  TcpServer* srv;

  std::list<TcpServer*>::iterator it;

  for (it = srv_list.begin(); it != srv_list.end(); ++it) {
    srv = *it;
    if (srv->name == name) {
      return srv;
    }
  }

  return NULL;
}

int CliCfgTcpSrvHandler(param_t* param, ser_buff_t* s_buf, op_mode mode) {
  // printf("%s\n", __FUNCTION__);

  int cmd;
  const char* srv_name = NULL;
  tlv_struct_t* tlv = NULL;
  TcpServer* srv = NULL;
  char* ip = (char*)default_ip;
  uint16_t port = default_port;

  cmd = EXTRACT_CMD_CODE(s_buf);

  TLV_LOOP_BEGIN(s_buf, tlv) {
    if (strncmp(tlv->leaf_id, "srv-name", strlen("srv-name")) == 0) {
      srv_name = tlv->value;
    } else if (strncmp(tlv->leaf_id, "srv-ip", strlen("srv-ip")) == 0) {
      ip = tlv->value;
    } else if (strncmp(tlv->leaf_id, "srv-port", strlen("srv-port")) == 0) {
      port = atoi(tlv->value);
    }
  }
  TLV_LOOP_END;

  switch (cmd) {
    case TCP_SRV_CREATE:
      /* config tcp-srv <name> */
      srv = CliLookupServer(std::string(srv_name));
      if (srv) {
        printf("error: tcp server already exist\n");
        return -1;
      }
      srv = new TcpServer(std::string(ip), port, std::string(srv_name));
      srv_list.push_back(srv);
      srv->RegisterListener(ConnectedListener, DisconnectedListener,
                            ReceivedListener);
      // printf("create invoked\n");
      break;

    case TCP_SRV_START:
      /* config tcp-srv <name> start */
      srv = CliLookupServer(std::string(srv_name));
      if (!srv) {
        printf("error: tcp server do not exist\n");
        return -1;
      }
      srv->Start();
      // printf("start invoked\n");
      break;
    default:
      break;
  }

  return 0;
}

static void CliBuildConfigTree() {
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
      init_param(&name, LEAF, NULL, CliCfgTcpSrvHandler, NULL, STRING,
                 "srv-name", "Server name");
      libcli_register_param(&tcp_srv, &name);
      set_param_cmd_code(&name, TCP_SRV_CREATE);
      {
        /* config tcp-srv <ip> ... */
        static param_t ip;
        init_param(&ip, LEAF, 0, NULL, NULL, IPV4, "srv-ip", "Server ip");
        libcli_register_param(&name, &ip);
        {
          /* config tcp-srv <ip> <port> */
          static param_t port;
          init_param(&port, LEAF, 0, CliCfgTcpSrvHandler, 0, INT, "srv-port",
                     "Server port");
          libcli_register_param(&ip, &port);
          set_param_cmd_code(&port, TCP_SRV_CREATE);
        }
      }
      {
        /* config tcp-srv <name> start */
        static param_t start;
        init_param(&start, CMD, "start", CliCfgTcpSrvHandler, NULL, INVALID,
                   NULL, "Start");
        libcli_register_param(&name, &start);
        set_param_cmd_code(&start, TCP_SRV_START);
      }
    }
  }
}

static int CliShowTcpSrvHandler(param_t* param, ser_buff_t* s_buf,
                                op_mode mode) {
  int cmd;
  char* srv_name = NULL;
  tlv_struct_t* tlv = NULL;
  TcpServer* srv = NULL;

  cmd = EXTRACT_CMD_CODE(s_buf);

  TLV_LOOP_BEGIN(s_buf, tlv) {
    if (strncmp(tlv->leaf_id, "srv-name", strlen("srv-name")) == 0) {
      srv_name = tlv->value;
    }
  }
  TLV_LOOP_END;

  switch (cmd) {
    case TCP_SRV_SHOW:
      srv = CliLookupServer(std::string(srv_name));
      if (!srv) {
        printf("error: tcp server do not exist\n");
        return -1;
      }
      srv->Display();
    default:;
      break;
  }
  return 0;
}

static void CliBuildShowTree() {
  param_t* show_hook = libcli_get_show_hook();

  /* show tcp-server ...*/
  static param_t tcp_srv;
  init_param(&tcp_srv, CMD, "tcp-srv", NULL, NULL, INVALID, NULL,
             "show tcp-srv");
  libcli_register_param(show_hook, &tcp_srv);
  {
    /* show tcp-server <name> */
    static param_t name;
    init_param(&name, LEAF, NULL, CliShowTcpSrvHandler, NULL, STRING,
               "srv-name", "Server name");
    libcli_register_param(&tcp_srv, &name);
    set_param_cmd_code(&name, TCP_SRV_SHOW);
  }
}

static void CliBuild() {
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