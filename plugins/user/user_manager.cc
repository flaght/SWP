// Copyright (c) 2016 The tourism Authors. All rights reserved.
// user_manager.cc
// Created on: 2016年8月10日.
// Author: Paco.

#include "user/user_manager.h"

#include <stdio.h>

//#include "glog/logging.h"

#include "user/user_opcode.h"
#include "user/user_interface.h"
#include "logic/logic_comm.h"

namespace user {
UserManager* UserManager::instance_ = NULL;

UserManager* UserManager::GetInstance() {
  if (instance_ == NULL)
    instance_ = new UserManager();
  return instance_;
}

void UserManager::FreeInstance() {
  if (instance_ == NULL) {
    delete instance_;
    instance_ = NULL;
  }
}

UserManager::UserManager() {

}

UserManager::~UserManager() {

}

int32 UserManager::AssignPacket(const int32 socket, PacketHead* packet) {
  int32 err = 0;
  UserInterface* interface = UserInterface::GetInstance();
  LOG_MSG2("AssignPacket opcode", (int)packet->operate_code);
  switch (packet->operate_code) {
    case HEARTBEAT_REQ: {
      interface->OnHeartbeat(socket, packet);
      break;
    }
    case USER_INFO_REQ: {
      interface->OnUserInfo(socket, packet);
      break;
    }
    case ACCOUNT_INFO_REQ: {
	  interface->OnAccountInfo(socket, packet);
	  break;
    }
    case ORDER_LIST_REQ: {
	  interface->OnOrderList(socket, packet);
	  break;
    }
    case ORDER_DETAIL_REQ: {
	  interface->OnOrderDetail(socket, packet);
	  break;
    }
    case BANKCARD_LIST_REQ: {
	  interface->OnBankcardList(socket, packet);
	  break;
    }
    case BIND_BANKCARD_REQ: {
	  interface->OnBindBankcard(socket, packet);
	  break;
    }
    case UNBIND_BANKCARD_REQ: {
	  interface->OnUnbindBankcard(socket, packet);
	  break;
    }
    case CHANGE_DEFAULT_BANKCARD_REQ: {
      interface->OnChangeDefaultBankcard(socket, packet);
      break;
    }
    case BANK_ACCOUNT_INFO_REQ: {
      interface->OnBankAccountInfo(socket, packet);
      break;
    }
    case CREDIT_LIST_REQ: {
      interface->OnCreditList(socket, packet);
      break;
    }
    case CREDIT_DETAIL_REQ: {
      interface->OnCreditDetail(socket, packet);
      break;
    }
    case USER_WITHDRAW_REQ: {
      interface->OnUserWithdraw(socket, packet);
      break;
    }
    case USER_WITHDRAW_LIST_REQ: {
      interface->OnUserWithdrawList(socket, packet);
      break;
    }
		
		/*case USER_WITHDRAW_DETAIL_REQ: {
	  interface->OnUserWithdrawDetail(socket, packet);
	  break;
	  }*/

    case OBTAIN_VERIFY_CODE_REQ: {
      interface->OnObtainVerifyCode(socket, packet);
      break;
    }
		
    case CHANGE_USER_INFO_REQ: {
      interface->OnChangeUserInfo(socket, packet);
      break;
    }
	case WX_PLACE_ORDER_REQ: {
      interface->OnWXPlaceOrder(socket, packet);
      break;
    }
    case WXPAY_CLIENT_REQ: {
      interface->OnWXPayClientResponse(socket, packet);
      break;
    }
    case WXPAY_SERVER_REQ: {
      interface->OnWXPayServerResponse(socket, packet);
      break;
    }
    case UNIONPAY_PLACE_ORDER_REQ: {
      interface->OnUnionpayPlaceOrder(socket, packet);
      break;
    }
    case UNIONPAY_CLIENT_REQ: {
      interface->OnWXPayClientResponse(socket, packet);
      break;
    }
    case UNIONPAY_SERVER_REQ: {
      interface->OnWXPayServerResponse(socket, packet);
      break;
    }
		/*case CHANGE_USER_INFO_REQ: {
	  interface->OnChangeUserInfo(socket, packet);
	  break;
	  }*/
		/*case DEVICE_TOKEN_REQ: {
      interface->OnDeviceToken(socket, packet);
      break;
	  }*/
		/*    case ALIPAY_SEVER_REQ: {
      interface->OnAlipayServer(socket, packet);
      break;
    }
    case ALIPAY_CLIENT_REQ: {
      interface->OnAlipayClient(socket, packet);
      break;
    }
    case CHECK_SMS_CODE_REQ: {
      interface->OnCheckSMSCode(socket, packet);
      break;
	  }*/
  }
  return err;
}

int32 UserManager::CheckHeartLoss() {
  UserInterface* interface = UserInterface::GetInstance();
  interface->CheckHeartLoss();
  return 0;
}

int32 UserManager::InitShareGuideData() {
  UserInterface* interface = UserInterface::GetInstance();
  //interface->InitShareGuide();
  return 0;
}

int32 UserManager::OnSockClose(const int fd) {
  UserInterface* interface = UserInterface::GetInstance();
  //  interface->CloseSocket(fd);
  return 0;
}

int32 UserManager::OrderStatusCheck() {
  UserInterface* interface = UserInterface::GetInstance();
  //interface->NopayOrderStatusCheck();
  return 0;
}

}  // namespace user

