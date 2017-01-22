//  Copyright (c) 2015-2015 The swp Authors. All rights reserved.
//  Created on: 2016年12月30日 Author: kerry

#include "quotations/quotations_logic.h"
#include "quotations/quotations_proto_buf.h"
#include "quotations/operator_code.h"
#include "quotations/schduler_engine.h"
#include "logic/swp_infos.h"
#include "config/config.h"
#include "core/common.h"
#include "logic/logic_comm.h"
#include "logic/logic_unit.h"
#include "net/errno.h"
#include <string>

#define DEFAULT_CONFIG_PATH "./plugins/quotations/quotations_config.xml"

namespace quatations_logic {

Quotationslogic *Quotationslogic::instance_ = NULL;

Quotationslogic::Quotationslogic() {
  if (!Init())
    assert(0);
}

Quotationslogic::~Quotationslogic() {
}

bool Quotationslogic::Init() {
  bool r = false;
  std::string path = DEFAULT_CONFIG_PATH;
  config::FileConfig *config = config::FileConfig::GetFileConfig();
  if (config == NULL)
    return false;
  r = config->LoadConfig(path);

  quotations_logic::QuotationsEngine::GetSchdulerManager();
  quotations_redis_ = new quotations_logic::QuotationsRedis(config);
  quotations_logic::QuotationsEngine::GetSchdulerManager()->InitRedis(
      quotations_redis_);
  quotations_logic::QuotationsEngine::GetSchdulerManager()->InitGoodsData();
  return true;
}

Quotationslogic *Quotationslogic::GetInstance() {
  if (instance_ == NULL)
    instance_ = new Quotationslogic();
  return instance_;
}

void Quotationslogic::FreeInstance() {
  delete instance_;
  instance_ = NULL;
}

bool Quotationslogic::OnQuotationsConnect(struct server *srv,
                                          const int socket) {
  std::string ip;
  int port;
  logic::SomeUtils::GetIPAddress(socket, ip, port);
  LOG_MSG2("ip {%s} prot {%d}", ip.c_str(), port);
  return true;
}

bool Quotationslogic::OnQuotationsMessage(struct server *srv, const int socket,
                                          const void *msg, const int len) {
  bool r = false;
  struct PacketHead *packet = NULL;
  if (srv == NULL || socket < 0 || msg == NULL || len < PACKET_HEAD_LENGTH)
    return false;

  if (!net::PacketProsess::UnpackStream(msg, len, &packet)) {
    LOG_ERROR2("UnpackStream Error socket %d", socket);
    return false;
  }

  switch (packet->operate_code) {
    case R_QUOTATIONS_REAL_TIME_DATA: {
      OnRealTime(srv, socket, packet);
      break;
    }
    case R_QUOTATIONS_TIME_LINE_DATA: {
      OnTimeLine(srv, socket, packet);
      break;
    }
    default:
      break;
  }
  return true;
}

bool Quotationslogic::OnQuotationsClose(struct server *srv, const int socket) {
  return true;
}

bool Quotationslogic::OnBroadcastConnect(struct server *srv, const int socket,
                                         const void *msg, const int len) {
  //像行情数据中心注册
  quotations_logic::QuotationsEngine::GetSchdulerManager()
      ->LoginQuotationsCenter(socket);
  return true;
}

bool Quotationslogic::OnBroadcastMessage(struct server *srv, const int socket,
                                         const void *msg, const int len) {
  bool r = false;
  struct PacketHead *packet = NULL;
  if (srv == NULL || socket < 0 || msg == NULL || len < PACKET_HEAD_LENGTH)
    return false;

  if (!net::PacketProsess::UnpackStream(msg, len, &packet)) {
    LOG_ERROR2("UnpackStream Error socket %d", socket);
    return false;
  }

  switch (packet->operate_code) {
    case 1001: {
      OnQutations(srv, socket, packet);
      break;
    }
    default:
      break;
  }
  return true;
}

bool Quotationslogic::OnBroadcastClose(struct server *srv, const int socket) {
  return true;
}

bool Quotationslogic::OnIniTimer(struct server *srv) {
  if (srv->add_time_task != NULL) {
  }
  return true;
}

bool Quotationslogic::OnTimeout(struct server *srv, char *id, int opcode,
                                int time) {
  switch (opcode) {
    default:
      break;
  }
  return true;
}

bool Quotationslogic::OnRealTime(struct server* srv, int socket,
                                 struct PacketHead *packet) {
  quotations_logic::net_request::RealTime real_time;
  struct PacketControl* packet_control = (struct PacketControl*) (packet);
  real_time.set_htt_packet(packet_control->body_);
  if (real_time.goods_infos_ != NULL)
    quotations_logic::QuotationsEngine::GetSchdulerManager()->SendRealTime(
        socket, real_time.goods_infos_);
  return true;
}

bool Quotationslogic::OnTimeLine(struct server* srv, int socket,
                                 struct PacketHead *packet) {
  quotations_logic::net_request::TimeLine time_line;
  struct PacketControl* packet_control = (struct PacketControl*) (packet);
  time_line.set_htt_packet(packet_control->body_);

  quotations_logic::QuotationsEngine::GetSchdulerManager()->SendTimeLine(
      socket, time_line.exchange_name(), time_line.platform_name(),
      time_line.good_type());
  return true;
}

bool Quotationslogic::OnQutations(struct server* srv, int socket,
                                  struct PacketHead *packet) {
  quotations_logic::net_other::RealTime real_time;
  swp_logic::Quotations quotations;
  struct PacketControl* packet_control = (struct PacketControl*) (packet);
  real_time.set_http_packet(packet_control->body_);
  quotations.set_change(real_time.change());
  quotations.set_closed_yesterday_price(real_time.closed_yesterday_price());
  quotations.set_current_price(real_time.current_price());
  quotations.set_current_unix_time(real_time.current_unix_time());
  quotations.set_exchange_name(real_time.exchange_name());
  quotations.set_high_price(real_time.high_price());
  quotations.set_low_price(real_time.low_price());
  quotations.set_opening_today_price(real_time.opening_today_price());
  quotations.set_pchg(real_time.pchg());
  quotations.set_platform_name(real_time.platform_name());
  quotations.set_symbol(real_time.symbol());
  quotations.set_type(real_time.type());
  quotations_logic::QuotationsEngine::GetSchdulerManager()->SetQuotations(
      quotations);
  return true;
}

void Quotationslogic::Test() {

  base_logic::ListValue* goodsinfos = new base_logic::ListValue;
  base_logic::DictionaryValue* unit = new base_logic::DictionaryValue;
  unit->SetString(L"platformName", "JH");
  unit->SetString(L"exchangeName", "DEFAULT");
  unit->SetString(L"goodType", "AG");
  goodsinfos->Append(unit);

  base_logic::ValueSerializer *engine = base_logic::ValueSerializer::Create(
      base_logic::IMPL_JSON);
  if (engine == NULL) {
    LOG_ERROR("engine create null");
    return;
  }
  std::string body_stream;
  engine->Serialize((*goodsinfos), &body_stream);

  quotations_logic::QuotationsEngine::GetSchdulerManager()->SendRealTime(
      13, goodsinfos);
  if (goodsinfos) {
    delete goodsinfos;
    goodsinfos = NULL;
  }

}

}  // namespace trades_logic