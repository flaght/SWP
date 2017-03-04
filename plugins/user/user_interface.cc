// Copyright (c) 2016 The tourism Authors. All rights reserved.
// user_interface.cc
// Created on: 2016年8月10日.
// Author: Paco.
#include "user/user_interface.h"

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <time.h>
#include <sys/socket.h>
#include <sstream>

#include "basic/md5sum.h"

#include "user/user_proto.h"
#include "user/user_opcode.h"
#include "util/util.h"
#include "pay/wxpay/wx_order.h"
#include "pay/unionpay/unionpay_order.h"
#include "net/packet_processing.h"
#include "logic/logic_unit.h"

#define SHELL_SMS "./verify_code_sms.sh"
#define SMS_KEY "yd1742653sd"
namespace user {
UserInterface* UserInterface::instance_;

UserInterface* UserInterface::GetInstance() {
  if (instance_ == NULL)
    instance_ = new UserInterface();
  return instance_;
}

void UserInterface::FreeInstance() {
  if (instance_ == NULL) {
    delete instance_;
    instance_ = NULL;
  }
}

UserInterface::UserInterface() {
  user_mysql_ = NULL;
  data_share_mgr_ = NULL;
}

UserInterface::~UserInterface() {

}

void UserInterface::InitConfig(config::FileConfig* config) {
  user_mysql_ = new UserMysql(config);
//  data_share_mgr_ = share::OnDataShareMgr::GetInstance();
}

void UserInterface::InitShareDataMgr(share::DataShareMgr* data) {
  data_share_mgr_ = data;

}

int32 UserInterface::OnUserInfo(const int32 socket, PacketHead* packet) {
  int32 err = 0;
  do {
    net_request::UserInfo user_info;
    struct PacketControl* packet_recv = (struct PacketControl*) (packet);
    user_info.set_http_packet(packet_recv->body_);
    DicValue dic;
    //    UserInfo* u = data_share_mgr_->GetUser(user_info.uid());
    //    if (u != NULL) {
    //      dict.SetBigInteger(L"uid_", u->uid());
    //      dict.SetString(L"phone_num_", u->phone_num());
    //      dict.SetString(L"nickname_", u->nickname());
    //      dict.SetBigInteger(L"credit_lv_", u->credit_lv());
    //      dict.SetBigInteger(L"praise_lv_", u->praise_lv());
    //      dict.SetBigInteger(L"cash_lv_", u->cash_lv());
    //      dict.SetString(L"head_url_", u->head_url());
    //      dict.SetString(L"address_", u->usual_addr());
    //      dict.SetReal(L"longitude_", u->usual_lon());
    //      dict.SetReal(L"latitude_", u->usual_lat());
    //      //SendMsg(socket, packet, &dict, USER_INFO_RLY);
    //      break;
    //    }
    err = user_mysql_->UserInfoSelect(user_info.uid_str(), &dic);
    if (err < 0)
      break;
    struct PacketControl packet_control;
    MAKE_HEAD(packet_control, USER_INFO_RLY, USER_TYPE, 0, 0, 0);
    packet_control.body_ = &dic;
    send_message(socket, &packet_control);
    //SendMsg(socket, packet, &dict, USER_INFO_RLY);
  } while (0);
  if (err < 0)
    send_error(socket, USER_TYPE, USER_INFO_RLY, err);
  //SendError(socket, packet, err, USER_INFO_RLY);
  return err;
}

int32 UserInterface::OnAccountInfo(const int32 socket, PacketHead* packet) {
  int32 err = 0;
  do {
    net_request::AccountInfo account_info;
    struct PacketControl* packet_recv = (struct PacketControl*) (packet);
    account_info.set_http_packet(packet_recv->body_);
    DicValue dic;
    err = user_mysql_->AccountInfoSelect(account_info.uid(), &dic);
    if (err < 0)
      break;
    struct PacketControl packet_control;
    MAKE_HEAD(packet_control, ACCOUNT_INFO_RLY, USER_TYPE, 0,
              packet->session_id, 0);
    packet_control.body_ = &dic;
    send_message(socket, &packet_control);
    //SendMsg(socket, packet, &dic, ACCOUNT_INFO_RLY);
  } while (0);
  if (err < 0)
    send_error(socket, ERROR_TYPE, err, packet->session_id);
  //SendError(socket, packet, err, ACCOUNT_INFO_RLY);
  return err;
}

int32 UserInterface::OnOrderList(const int32 socket, PacketHead* packet) {
  int32 err = 0;
  do {
    net_request::OrderList order_list;
    struct PacketControl* packet_recv = (struct PacketControl*) (packet);
    order_list.set_http_packet(packet_recv->body_);
    DicValue dic;
    err = user_mysql_->OrderListSelect(order_list.uid(), order_list.flow_type(),
                                       order_list.start_pos(),
                                       order_list.count(), &dic);
    if (err < 0)
      break;
    struct PacketControl packet_control;
    MAKE_HEAD(packet_control, ORDER_LIST_RLY, USER_TYPE, 0, 0, 0);
    packet_control.body_ = &dic;
    send_message(socket, &packet_control);
    //SendMsg(socket, packet, &dic, ORDER_LIST_RLY);
  } while (0);
  if (err < 0)
    send_error(socket, ERROR_TYPE, err, packet->session_id);
  //SendError(socket, packet, err, ORDER_LIST_RLY);
  return err;
}

int32 UserInterface::OnOrderDetail(const int32 socket, PacketHead* packet) {
  int32 err = 0;
  do {
    net_request::OrderDetail order_detail;
    struct PacketControl* packet_recv = (struct PacketControl*) (packet);
    order_detail.set_http_packet(packet_recv->body_);
    DicValue dic;
    err = user_mysql_->OrderDetailSelect(order_detail.uid(),
                                         order_detail.flow_id(),
                                         order_detail.flow_type(), &dic);
    if (err < 0)
      break;
    struct PacketControl packet_control;
    MAKE_HEAD(packet_control, ORDER_DETAIL_RLY, USER_TYPE, 0, 0, 0);
    packet_control.body_ = &dic;
    send_message(socket, &packet_control);
    //SendMsg(socket, packet, &dic, ORDER_DETAIL_RLY);
  } while (0);
  if (err < 0)
    send_error(socket, ERROR_TYPE, err, packet->session_id);
  //SendError(socket, packet, err, ORDER_DETAIL_RLY);
  return err;
}

int32 UserInterface::OnBankcardList(const int32 socket, PacketHead* packet) {
  int32 err = 0;
  do {
    net_request::BankcardList bankcard_list;
    struct PacketControl* packet_recv = (struct PacketControl*) (packet);
    bankcard_list.set_http_packet(packet_recv->body_);
    DicValue dic;
    err = user_mysql_->BankcardListSelect(bankcard_list.uid(), &dic);
    if (err < 0)
      break;
    struct PacketControl packet_control;
    MAKE_HEAD(packet_control, BANKCARD_LIST_RLY, USER_TYPE, 0, 0, 0);
    packet_control.body_ = &dic;
    send_message(socket, &packet_control);
    //SendMsg(socket, packet, &dic, BANKCARD_LIST_RLY);
  } while (0);
  if (err < 0)
    send_error(socket, ERROR_TYPE, err, packet->session_id);
  //SendError(socket, packet, err, BANKCARD_LIST_RLY);
  return err;
}

int32 UserInterface::OnBindBankcard(const int32 socket, PacketHead* packet) {
  int32 err = 0;
  do {
    net_request::BindBankcard bind_bankcard;
    struct PacketControl* packet_recv = (struct PacketControl*) (packet);
    bind_bankcard.set_http_packet(packet_recv->body_);
    DicValue dic;
    err = user_mysql_->BindBankcardInsertAndSelect(
        bind_bankcard.uid(), bind_bankcard.bank_id(),
        bind_bankcard.branch_bank(), bind_bankcard.bankcard_num(),
        bind_bankcard.bank_username(), &dic);
    if (err < 0)
      break;
    struct PacketControl packet_control;
    MAKE_HEAD(packet_control, BIND_BANKCARD_RLY, USER_TYPE, 0, 0, 0);
    packet_control.body_ = &dic;
    send_message(socket, &packet_control);
    //SendMsg(socket, packet, &dic, BIND_BANKCARD_RLY);
  } while (0);
  if (err < 0)
    send_error(socket, ERROR_TYPE, err, packet->session_id);
  //SendError(socket, packet, err, BIND_BANKCARD_RLY);
  return err;
}

int32 UserInterface::OnUnbindBankcard(const int32 socket, PacketHead* packet) {
  int32 err = 0;
  do {
    net_request::UnbindBankcard unbind_bankcard;
    struct PacketControl* packet_recv = (struct PacketControl*) (packet);
    unbind_bankcard.set_http_packet(packet_recv->body_);
    /*if (time(NULL) - unbind_bankcard.timestamp() > 15 * 60) {
     err = VERIFY_CODE_OVERDUE;
     break;
     }
     std::stringstream ss;
     ss << SMS_KEY << unbind_bankcard.timestamp() << unbind_bankcard.verify_code() << unbind_bankcard.phone_num();
     base::MD5Sum md5(ss.str());
     if (md5.GetHash() != unbind_bankcard.verify_token()) {
     err = VERIFY_CODE_ERR;
     break;
     }*/
    err = user_mysql_->UnbindBankcardDelete(unbind_bankcard.phone_num(),
                                            unbind_bankcard.bankcard_id());
    LOG_MSG("unbind before send");
    if (err < 0)
      break;
    struct PacketControl packet_control;
    MAKE_HEAD(packet_control, UNBIND_BANKCARD_RLY, USER_TYPE, 0, 0, 0);
    packet_control.body_ = NULL;
    LOG_MSG("unbind before send");
    send_message(socket, &packet_control);
    //SendMsg(socket, packet, NULL, UNBIND_BANKCARD_RLY);
  } while (0);
  if (err < 0)
    send_error(socket, ERROR_TYPE, err, packet->session_id);
  //SendError(socket, packet, err, UNBIND_BANKCARD_RLY);
  return err;
}

int32 UserInterface::OnChangeDefaultBankcard(const int32 socket,
                                             PacketHead* packet) {
  int32 err = 0;
  do {
    net_request::ChangeDefaultBankcard change_default_bankcard;
    struct PacketControl* packet_recv = (struct PacketControl*) (packet);
    change_default_bankcard.set_http_packet(packet_recv->body_);
    err = user_mysql_->ChangeDefaultBankcard(
        change_default_bankcard.uid(), change_default_bankcard.bankcard_id());
    if (err < 0)
      break;
    struct PacketControl packet_control;
    MAKE_HEAD(packet_control, CHANGE_DEFAULT_BANKCARD_RLY, USER_TYPE, 0, 0, 0);
    packet_control.body_ = NULL;
    send_message(socket, &packet_control);
    //SendMsg(socket, packet, NULL, CHANGE_DEFAULT_BANKCARD_RLY);
  } while (0);
  if (err < 0)
    send_error(socket, ERROR_TYPE, err, packet->session_id);
  //SendError(socket, packet, err, CHANGE_DEFAULT_BANKCARD_RLY);
  return err;
}

int32 UserInterface::OnBankAccountInfo(const int32 socket, PacketHead* packet) {
  int32 err = 0;
  do {
    net_request::BankAccountInfo bank_account_info;
    struct PacketControl* packet_recv = (struct PacketControl*) (packet);
    bank_account_info.set_http_packet(packet_recv->body_);
    DicValue dic;
    err = user_mysql_->BankAccountInfoSelect(bank_account_info.bankcard_num(),
                                             &dic);
    if (err < 0)
      break;
    struct PacketControl packet_control;
    MAKE_HEAD(packet_control, BANK_ACCOUNT_INFO_RLY, USER_TYPE, 0, 0, 0);
    packet_control.body_ = &dic;
    send_message(socket, &packet_control);
    //SendMsg(socket, packet, &dic, BANK_ACCOUNT_INFO_RLY);
  } while (0);
  if (err < 0)
    send_error(socket, ERROR_TYPE, err, packet->session_id);
  //SendError(socket, packet, err, BANK_ACCOUNT_INFO_RLY);
  return err;
}

int UserInterface::SplitString(const std::string &in, const char ch,
                               std::vector<std::string> &out) {  //last is ,??????????
  if (in.size() == 0)
    return 0;
  out.clear();

  size_t old_pos = 0, new_pos = 0;
  std::string tmp;
  while (true) {
    new_pos = in.find(ch, old_pos);
    if (new_pos != std::string::npos) {
      tmp = in.substr(old_pos, new_pos - old_pos);
      if (tmp != "")
        out.push_back(tmp);
      old_pos = ++new_pos;
    } else if (old_pos <= in.size()) {
      tmp = in.substr(old_pos);
      if (tmp != "")
        out.push_back(tmp);
      break;
    } else
      break;
  }

  return 0;
}

int32 UserInterface::OnCreditList(const int32 socket, PacketHead* packet) {
  int32 err = 0;
  do {
    net_request::CreditList credit_list;
    struct PacketControl* packet_recv = (struct PacketControl*) (packet);
    credit_list.set_http_packet(packet_recv->body_);

    std::string db_status;
    std::vector < std::string > split_status;
    SplitString(credit_list.status(), ',', split_status);
    for (std::vector<std::string>::iterator it = split_status.begin();
        it != split_status.end(); ++it) {
      if (*it == CREDIT_INPROCESS) {
        db_status += DB_CREDIT_INPROCESS_STR;
        db_status += DB_CREDIT_CLIENT_SUCCESS_STR;
      } else if (*it == CREDIT_SUCCESS)
        db_status += DB_CREDIT_SERVER_SUCCESS_STR;
      else if (*it == CREDIT_FAIL) {
        db_status += DB_CREDIT_CLIENT_FAIL_STR;
        db_status += DB_CREDIT_SERVER_FAIL_STR;
      } else {
        err = CREDIT_STATUS_ERR;
        break;
      }
    }
    if (err < 0)
      break;

    DicValue dic;
    err = user_mysql_->CreditListSelect(
        credit_list.uid(), db_status.substr(0, db_status.size() - 1),
        credit_list.start_pos(), credit_list.count(), &dic);
    if (err < 0)
      break;
    struct PacketControl packet_control;
    MAKE_HEAD(packet_control, CREDIT_LIST_RLY, USER_TYPE, 0, 0, 0);
    packet_control.body_ = &dic;
    send_message(socket, &packet_control);
    //SendMsg(socket, packet, &dic, CREDIT_LIST_RLY);
  } while (0);
  if (err < 0)
    send_error(socket, ERROR_TYPE, err, packet->session_id);
  //SendError(socket, packet, err, CREDIT_LIST_RLY);
  return err;
}

/*int32 UserInterface::OnCreditDetail(const int32 socket, PacketHead* packet) {
 int32 err = 0;
 do {
 net_request::CreditDetail credit_detail;
 struct PacketControl* packet_recv = (struct PacketControl*) (packet);
 credit_detail.set_http_packet(packet_recv->body_);
 DicValue dic;
 err = user_mysql_->CreditDetailSelect(credit_detail.uid(), credit_detail.recharge_id(), &dic);
 if (err < 0)
 break;
 //SendMsg(socket, packet, &dic, CREDIT_DETAIL_RLY);
 } while (0);
 if (err < 0)
 //SendError(socket, packet, err, CREDIT_DETAIL_RLY);
 return err;
 }*/

int32 UserInterface::OnUserWithdraw(const int32 socket, PacketHead* packet) {
  int32 err = 0;
  do {
    net_request::UserWithdraw user_withdraw;
    struct PacketControl* packet_recv = (struct PacketControl*) (packet);
    user_withdraw.set_http_packet(packet_recv->body_);
    DicValue dic;
    err = user_mysql_->UserWithdrawInsertAndSelect(user_withdraw.uid(),
                                                   user_withdraw.money(),
                                                   user_withdraw.bankcard_id(),
                                                   user_withdraw.passwd(),
                                                   &dic);
    if (err < 0)
      break;
    struct PacketControl packet_control;
    MAKE_HEAD(packet_control, USER_WITHDRAW_RLY, USER_TYPE, 0, 0, 0);
    packet_control.body_ = &dic;
    send_message(socket, &packet_control);
    //SendMsg(socket, packet, &dic, USER_WITHDRAW_RLY);
  } while (0);
  if (err < 0)
    send_error(socket, ERROR_TYPE, err, packet->session_id);
  //SendError(socket, packet, err, USER_WITHDRAW_RLY);
  return err;
}

int32 UserInterface::OnUserWithdrawList(const int32 socket,
                                        PacketHead* packet) {
  int32 err = 0;
  do {
    net_request::UserWithdrawList user_withdraw_list;
    struct PacketControl* packet_recv = (struct PacketControl*) (packet);
    user_withdraw_list.set_http_packet(packet_recv->body_);
    DicValue dic;
    err = user_mysql_->UserWithdrawListSelect(user_withdraw_list.uid(),
                                              user_withdraw_list.status(),
                                              user_withdraw_list.start_pos(),
                                              user_withdraw_list.count(), &dic);
    if (err < 0)
      break;
    struct PacketControl packet_control;
    MAKE_HEAD(packet_control, USER_WITHDRAW_LIST_RLY, USER_TYPE, 0, 0, 0);
    packet_control.body_ = &dic;
    send_message(socket, &packet_control);
    //SendMsg(socket, packet, &dic, USER_WITHDRAW_LIST_RLY);
  } while (0);
  if (err < 0)
    send_error(socket, ERROR_TYPE, err, packet->session_id);
  //SendError(socket, packet, err, USER_WITHDRAW_LIST_RLY);
  return err;
}

/*int32 UserInterface::OnUserWithdrawDetail(const int32 socket, PacketHead* packet) {
 int32 err = 0;
 do {
 UserWithdrawDetail recv;
 err = recv.Deserialize();
 if (err < 0)
 break;
 DicValue dic;
 err = user_mysql_->UserWithdrawDetailSelect(recv.uid(), recv.withdraw_id(), &dic);
 if (err < 0)
 break;
 //SendMsg(socket, packet, &dic, USER_WITHDRAW_DETAIL_RLY);
 } while (0);
 if (err < 0)
 //SendError(socket, packet, err, USER_WITHDRAW_DETAIL_RLY);
 return err;
 }*/

int32 UserInterface::OnObtainVerifyCode(const int32 socket,
                                        PacketHead* packet) {
  int32 err = 0;
  do {
    net_request::ObtainVerifyCode obtain_verify_code;
    struct PacketControl* packet_recv = (struct PacketControl*) (packet);
    obtain_verify_code.set_http_packet(packet_recv->body_);
    std::stringstream ss;
    int64 timestamp_ = time(NULL);
    int64 rand_code_ = util::Random(100000, 999999);

    DicValue dic;
    dic.SetBigInteger(L"timestamp", timestamp_);

    ss << SMS_KEY << timestamp_ << rand_code_ << obtain_verify_code.phone_num();
    base::MD5Sum md5(ss.str());
    dic.SetString(L"vToken", md5.GetHash().c_str());
    LOG_MSG2("md5 token: %s", md5.GetHash().c_str());
    ss.str("");
    ss.clear();
    ss << obtain_verify_code.phone_num() << " "
        <<rand_code_<<" "
        << obtain_verify_code.verify_type();
    data_share_mgr_->UpdateSMSToken(ss.str(), md5.GetHash());
    struct PacketControl packet_control;
    MAKE_HEAD(packet_control, OBTAIN_VERIFY_CODE_RLY, USER_TYPE, 0, packet->session_id, 0);
    packet_control.body_ = &dic;
    send_message(socket, &packet_control);
    //SendMsg(socket, packet, &dic, OBTAIN_VERIFY_CODE_RLY);

    ss.str("");
    ss.clear();
    ss << SHELL_SMS << " " << obtain_verify_code.phone_num() << " "
        << rand_code_ << " " << obtain_verify_code.verify_type();
    std::string sysc = ss.str();
    LOG_DEBUG2("%s",sysc.c_str());
    system(ss.str().c_str());
  } while (0);
  if (err < 0) {
    send_error(socket, ERROR_TYPE, err, packet->session_id);
    //SendError(socket, packet, err, OBTAIN_VERIFY_CODE_RLY);
  }
  return err;
}

int32 UserInterface::OnChangeUserInfo(const int32 socket, PacketHead* packet) {
  int32 err = 0;
  do {
    net_request::ChangeUserInfo change_user_info;
    struct PacketControl* packet_recv = (struct PacketControl*) (packet);
    change_user_info.set_http_packet(packet_recv->body_);
    err = user_mysql_->ChangeUserInfoUpdate(change_user_info.uid(),
                                            change_user_info.nickname(),
                                            change_user_info.head_url(),
                                            change_user_info.gender());
    if (err < 0)
      break;
    struct PacketControl packet_control;
    MAKE_HEAD(packet_control, CHANGE_USER_INFO_RLY, USER_TYPE, 0, 0, 0);
    packet_control.body_ = NULL;
    send_message(socket, &packet_control);
    //SendMsg(socket, packet, NULL, CHANGE_USER_INFO_RLY);
  } while (0);
  if (err < 0) {
    send_error(socket, ERROR_TYPE, err, packet->session_id);
    //SendError(socket, packet, err, CHANGE_USER_INFO_RLY);
  }
  return err;
}

int32 UserInterface::OnWXPlaceOrder(const int32 socket, PacketHead* packet) {
  int32 err = 0;
  do {
    net_request::WXPlaceOrder wx_place_order;
    struct PacketControl* packet_recv = (struct PacketControl*) (packet);
    wx_place_order.set_http_packet(packet_recv->body_);
    //记录订单信息
    DicValue recharge_dic;
    err = user_mysql_->RechargeInfoInsertAndSelect(wx_place_order.uid(),
                                                   wx_place_order.price(),
                                                   &recharge_dic);
    if (err < 0)
      break;
    std::string recharge_id;
    recharge_dic.GetString(L"rid", &recharge_id);
    std::string ip;
    //访问微信下单接口
    WXOrder wx_order;
    if (util::GetIPAddress(socket, &ip, NULL))
      wx_order.set_spbill_create_ip(ip);
    wx_order.set_body(wx_place_order.title());
    wx_order.set_out_trade_no(recharge_id);
    wx_order.set_total_fee(wx_place_order.price() * 100);
    std::string wx_result = wx_order.PlaceOrder();
    base_logic::ValueSerializer* deserializer =
        base_logic::ValueSerializer::Create(base_logic::IMPL_XML, &wx_result);
    std::string err_str;
    int32 err = 0;
    DicValue* dic = (DicValue*) deserializer->Deserialize(&err, &err_str);
    do {
      if (dic != NULL) {
        std::string return_code;
        dic->GetString(L"return_code", &return_code);
        LOG_MSG2("return_code:", return_code);
        //下单成功
        if (return_code.find("SUCCESS") != std::string::npos) {
          std::string result_code;
          dic->GetString(L"result_code", &result_code);
          LOG_MSG2("result_code:", result_code);
          //业务逻辑成功
          if (result_code.find("SUCCESS") != std::string::npos) {
            std::string prepay_id;
            dic->GetString(L"prepay_id", &prepay_id);
            LOG_MSG2("prepay_id:", prepay_id);
            int npos1 = prepay_id.find("<![CDATA[");
            int npos2 = prepay_id.find("]]>");
            prepay_id = prepay_id.substr(npos1 + 9, npos2 - npos1 - 9);
            wx_order.set_prepayid(prepay_id);
            wx_order.PreSign();
            wx_order.PreSerialize(&recharge_dic);
            struct PacketControl packet_control;
            MAKE_HEAD(packet_control, WX_PLACE_ORDER_RLY, USER_TYPE, 0, 0, 0);
            packet_control.body_ = &recharge_dic;
            send_message(socket, &packet_control);
            //SendMsg(socket, packet, &recharge_dic, WX_PLACE_ORDER_RLY);
            // todo 下单成功 ，记录微信订单信息

          } else {
            err = WX_PLACE_ORDER_ERR;
            break;
          }
        } else {
          err = WX_PLACE_ORDER_ERR;
          break;
        }
      }
    } while (0);
    base_logic::ValueSerializer::DeleteSerializer(base_logic::IMPL_XML,
                                                  deserializer);

  } while (0);
  if (err < 0) {
    send_error(socket, ERROR_TYPE, err, packet->session_id);
    //SendError(socket, packet, err, WX_PLACE_ORDER_RLY);
    //记录订单信息
    //访问微信下单接口
    /*
     if (util::GetIPAddress(socket, &ip, NULL))
     unionpayorder.set_spbill_create_ip(ip);
     unionpayorder.set_body(unionpayplace_order.title());
     unionpayorder.set_out_trade_no(recharge_id);
     unionpayorder.set_total_fee(unionpayplace_order.price() * 100);*/
    //basic_logic::DictionaryValue dic;
    //SendMsg(socket, packet, &recharge_dic, UNIONPAY_PLACE_ORDER_RLY);
    // todo 下单成功 ，记录微信订单信息
    /*          } else {
     err = UNIONPAY_PLACE_ORDER_ERR;
     break;
     }
     } else {
     err = UNIONPAY_PLACE_ORDER_ERR;
     break;
     }
     }*/
    //base_logic::ValueSerializer::DeleteSerializer(base_logic::IMPL_XML,
    //                                               deserializer);
    //} while (0);
    //SendError(socket, packet, err, UNIONPAY_PLACE_ORDER_RLY);
  }
  return err;
}

int32 UserInterface::OnWXPayClientResponse(const int32 socket,
                                           PacketHead* packet) {
  int32 err = 0;
  do {
    net_request::WXPayClient wx_pay_client;
    struct PacketControl* packet_recv = (struct PacketControl*) (packet);
    wx_pay_client.set_http_packet(packet_recv->body_);
    DicValue dic;
    /*err = user_mysql_->ChangeRechargeStatusAndSelect(wx_pay_client.recharge_id(),
     wx_pay_client.pay_result(), &dic);
     if (err < 0)
     break;
     struct PacketControl packet_control;
     MAKE_HEAD(packet_control, WXPAY_CLIENT_RLY, USER_TYPE, 0, 0, 0);
     packet_control.body_ = &dic;
     send_message(socket, &packet_control);*/
    //SendMsg(socket, packet, &dic, WXPAY_CLIENT_RLY);
    if (wx_pay_client.pay_result() == 1)
      user_mysql_->ChangeRechargeStatusAndSelect(wx_pay_client.recharge_id(), 1,
                                                 &dic);
    else
      user_mysql_->ChangeRechargeStatusAndSelect(wx_pay_client.recharge_id(), 2,
                                                 &dic);
    struct PacketControl packet_control;
    MAKE_HEAD(packet_control, WXPAY_CLIENT_RLY, USER_TYPE, 0, 0, 0);
    packet_control.body_ = &dic;
    send_message(socket, &packet_control);
    //SendMsg(socket, packet, &dic, WXPAY_SERVER_RLY);
  } while (0);
  if (err < 0) {
    send_error(socket, ERROR_TYPE, err, packet->session_id);
    //SendError(socket, packet, err, WXPAY_CLIENT_RLY);
  }
  return err;
}

#define WX_RESPONSE \
"<xml>\
  <return_code><![CDATA[SUCCESS]]></return_code>\
  <return_msg><![CDATA[OK]]></return_msg>\
</xml>"
int32 UserInterface::OnWXPayServerResponse(const int32 socket,
                                           PacketHead* packet) {
  int32 err = 0;
  do {
    net_request::WXPayServer wx_pay_server;
    struct PacketControl* packet_recv = (struct PacketControl*) (packet);
    wx_pay_server.set_http_packet(packet_recv->body_);
    //支付成功
    DicValue dic;
    if (wx_pay_server.appid() != APPID && wx_pay_server.mch_id() != MCH_ID) {
      LOG_ERROR2("WXPAY SERVER RESULT appid:[%d]", wx_pay_server.appid()); LOG_ERROR2("WXPAY SERVER RESULT mch_id:[%d]", wx_pay_server.mch_id());
      break;
    }
    if (wx_pay_server.pay_result() == 1) {
      user_mysql_->ChangeRechargeStatusAndSelect(wx_pay_server.recharge_id(), 3,
                                                 &dic);
    } else {
      user_mysql_->ChangeRechargeStatusAndSelect(wx_pay_server.recharge_id(), 4,
                                                 &dic);
    }
    int64 user_id = 0;
    send(socket, WX_RESPONSE, sizeof(WX_RESPONSE), 0);
    /*dic.GetBigInteger(L"uid_", &user_id);
     UserInfo* user = data_share_mgr_->GetUser(user_id);
     if (user != NULL && user->is_login()) {
     struct PacketControl packet_control;
     MAKE_HEAD(packet_control, WXPAY_SERVER_RLY, USER_TYPE, 0, 0, 0);
     packet_control.body_ = &dic;
     send_message(socket, &packet_control);
     //SendMsg(user->socket_fd(), packet, &dic, WXPAY_SERVER_RLY);
     }*/
  } while (0);
  return err;
}

int32 UserInterface::OnUnionpayPlaceOrder(const int32 socket,
                                          PacketHead* packet) {
  int32 err = 0;
  do {
    net_request::UnionpayPlaceOrder unionpay_place_order;
    struct PacketControl* packet_recv = (struct PacketControl*) (packet);
    err = unionpay_place_order.set_http_packet(packet_recv->body_);
    if (err < 0)
      break;
    DicValue recharge_dic;
    err = user_mysql_->RechargeInfoInsertAndSelect(unionpay_place_order.uid(),
                                                   unionpay_place_order.price(),
                                                   &recharge_dic);
    if (err < 0)
      break;
    int64 recharge_id;
    recharge_dic.GetBigInteger(L"rid", &recharge_id);
    std::string ip;
    DicValue dic;
    dic.SetBigInteger(L"rid", recharge_id);
    UnionpayOrder unionpay_order;
    unionpay_order.set_price(unionpay_place_order.price());
    std::string unionpay_result = unionpay_order.PlaceOrder();
    dic.SetString(L"tn", unionpay_result);
    LOG_MSG2("kkkkkk tn:%s", unionpay_result.c_str());
    struct PacketControl packet_control;
    MAKE_HEAD(packet_control, UNIONPAY_PLACE_ORDER_RLY, USER_TYPE, 0, 0, 0);
    packet_control.body_ = &dic;
    send_message(socket, &packet_control);
  } while (0);
  if (err < 0) {
    send_error(socket, ERROR_TYPE, err, packet->session_id);
  }
  return err;
}
int32 UserInterface::CloseSocket(const int fd) {
  data_share_mgr_->UserOffline(fd);
  return 0;
}

int32 UserInterface::OnAlipayServer(const int32 socket, PacketHead* packet) {
  LOG_MSG("alipay server req");
  return 0;
}

int32 UserInterface::OnAlipayClient(const int32 socket, PacketHead* packet) {
  LOG_MSG("alipay client req");
  return 0;
}

/*int32 UserInterface::OnDeviceToken(const int32 socket, PacketHead* packet) {
 int32 err = 0;
 LOG(INFO) << "DeviceToken";
 do {
 DeviceToken rev;
 LOG(INFO) << "DeviceToken Deserialize err:" << err;
 if (err < 0)
 break;
 int result = data_share_mgr_->AddDeviceToken(rev.uid(), rev.device_token());
 LOG(INFO) << "AddDeviceToken result:" << result;
 if (result >= 0)
 err = user_mysql_->DeviceTokenUpdate(rev.uid(), rev.device_token());
 if (err < 0)
 break;
 //SendMsg(socket, packet, NULL, DEVICE_TOKEN_RLY);
 } while (0);
 if (err < 0) {
 LOG(INFO) << "DeviceToken SendError err:" << err;
 //SendError(socket, packet, err, DEVICE_TOKEN_RLY);
 }
 return err;
 }*/

/*int32 UserInterface::OnCheckSMSCode(const int32 socket, PacketHead* packet) {
 int32 err = 0;
 do {
 CheckSMSCode rev;
 if (err < 0)
 break;
 if (time(NULL) - rev.timestamp() > 15 * 60) {
 err = VERIFY_CODE_OVERDUE;
 break;
 }
 std::stringstream ss;
 ss << SMS_KEY << rev.timestamp() << rev.verify_code() << rev.phone_num();
 base::MD5Sum md5(ss.str());
 if (md5.GetHash() != rev.token()) {
 err = VERIFY_CODE_ERR;
 break;
 }
 ss.str("");
 ss.clear();
 ss << rev.phone_num() << ":" << rev.verify_type();
 if (!data_share_mgr_->CheckSMSToken(ss.str(), rev.token())) {
 err = VERIFY_CODE_NOT_NEW;
 break;
 }
 //SendMsg(socket, packet, NULL, CHECK_SMS_CODE_RLY);
 } while (0);
 if (err < 0)
 //SendError(socket, packet, err, CHECK_SMS_CODE_RLY);
 return err;
 }*/

/*void UserInterface::SendPacket(const int socket, PacketHead* packet) {

 char* s = new char[packet->packet_length];
 LOG(INFO) << "packet body:" << packet->body_str();
 memset(s, 0, packet->packet_length());
 memcpy(s, &packet->head(), HEAD_LENGTH);
 memcpy(s + HEAD_LENGTH, packet->body_str().c_str(),
 packet->body_str().length());
 int total = util::OnSendFull(socket, s, packet->packet_length());
 delete[] s;
 s = NULL;
 LOG_IF(ERROR, total != packet->packet_length())
 << "send packet wrong:opcode[" << packet->operate_code() << "]"
 << total << "," << packet->packet_length();
 }

 void UserInterface:://SendError(const int socket, PacketHead* packet, int32 err,
 int16 opcode) {
 PacketErr p_err;
 p_err.set_head(packet->head());
 p_err.set_type(ERROR_TYPE);
 p_err.set_error(err);
 p_err.Serialize();
 p_err.AdapterLen();
 p_err.set_operate_code(opcode);
 SendPacket(socket, &p_err);
 }

 void UserInterface:://SendMsg(const int socket, PacketHead* packet, DicValue* dic,
 int16 opcode) {
 PacketHead send;
 send.set_head(packet->head());
 send.Serialize(dic);
 send.AdapterLen();
 send.set_operate_code(opcode);
 SendPacket(socket, &send);
 }*/

}  // namespace user

