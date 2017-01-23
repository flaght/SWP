// Copyright (c) 2016 The tourism Authors. All rights reserved.
// login_mysql.cc

#include "login/login_mysql.h"

#include <mysql/mysql.h>

#include "pub/storage/data_engine.h"
#include "pub/comm/comm_head.h"

#include "glog/logging.h"

namespace login {

LoginMysql::LoginMysql(config::FileConfig* config) {
  mysql_engine_ = base_logic::DataEngine::Create(MYSQL_TYPE);
  mysql_engine_->InitParam(config->mysql_db_list_);
}

LoginMysql::~LoginMysql() {
  if (mysql_engine_) {
    delete mysql_engine_;
  }
  mysql_engine_ = NULL;
}

int32 LoginMysql::RegisterInsertAndSelect(std::string phone_num, std::string passwd, DicValue* dic) {
  int32 err = 0;
  bool r = false;
  do {
  	std::stringstream ss;
  	ss << "call proc_RegisterInsertAndSelect('" << phone_num << "','" << passwd << "')";
  	LOG(INFO)<< "sql:" << ss.str();
  	r = mysql_engine_->ReadData(ss.str(), dic, CallRegisterInsertAndSelect);
  	//注册一定有结果返回
  	if (!r || dic->empty()) {
  		err = SQL_EXEC_ERR;
  		break;
  	}
  	//    dic->GetBigInteger(L"result")
  } while (0);
  return err;
}

int32 LoginMysql::UserLoginSelect(std::string phone, const char* client_ip,  DicValue* dic) {
  int32 err = 0;
  bool r = false;
  do {
    std::stringstream ss;
    ss << "call proc_UserLoginSelect('" << phone << "','" << client_ip << "')";
    LOG(INFO)<< "sql:" << ss.str();
    r = mysql_engine_->ReadData(ss.str(), dic, CallUserLoginSelect);
    if (!r) {
      err = SQL_EXEC_ERR;
      break;
    }
    if (dic->empty()) {
      err = PHONE_OR_PASSWD_ERR;
      break;
    }
  } while (0);
  return err;
}

int32 LoginMysql::ChangePasswdUpdate(std::string phone, std::string passwd) {
  int32 err = 0;
  bool r = false;
  do {
    std::stringstream ss;
    ss << "call proc_ChangePasswdUpdate(" << phone << ",'" << passwd << "');";
    LOG(INFO)<< "sql:" << ss.str();
    r = mysql_engine_->WriteData(ss.str());
    if (!r)
      err = SQL_EXEC_ERR;
  } while (0);
  return err;
}


int32 LoginMysql::CheckPasswdSelect(int64 uid, std::string pass, int64 type,
                                   DicValue* dic) {
  int32 err = 0;
  bool r = false;
  do {
    std::stringstream ss;
    ss << "call proc_CheckPasswdSelect(" << uid << ",'" << pass << "'," << type
       << ")";
    LOG(INFO)<< "sql:" << ss.str();
	/*    r = mysql_engine_->ReadData(ss.str(), dic, CallCheckPasswdSelect);
    if (!r) {
      err = SQL_EXEC_ERROR;
      break;
	  }*/
  } while (0);
  return err;
}

	/*int32 LoginMysql::ChangePasswdSelect(int64 uid, std::string oldpass,
                                    std::string newpass, int64 ctype,
                                    int64 ptype, DicValue* dic) {
  int32 err = 0;
  bool r = false;
  do {
    std::stringstream ss;
    ss << "call proc_ChangePasswdSelect(" << uid << ",'" << oldpass << "','"
       << newpass << "'," << ctype << "," << ptype << ")";
    LOG(INFO)<< "sql:" << ss.str();
    r = mysql_engine_->ReadData(ss.str(), dic, CallChangePasswdSelect);
    if (!r) {
      err = SQL_EXEC_ERROR;
      break;
    }
  } while (0);
  return err;
  }*/

void LoginMysql::CallRegisterInsertAndSelect(void* param, Value* value) {
  base_storage::DBStorageEngine* engine =
      (base_storage::DBStorageEngine*) (param);
  MYSQL_ROW rows;
  int32 num = engine->RecordCount();
  DicValue* dict = reinterpret_cast<DicValue*>(value);
  if (num > 0) {
    while (rows = (*(MYSQL_ROW*) (engine->FetchRows())->proc)) {
      if (rows[0] != NULL) {
        dict->SetBigInteger(L"result", atoll(rows[0]));
        //用户已注册过
        if (atoi(rows[0]) == 0)
          break;
      }
      if (rows[1] != NULL)
        dict->SetBigInteger(L"uid", atoll(rows[1]));
    }
  } else {
    LOG(WARNING)<< "CallRegisterInsertAndSelect count < 0";
  }
}

void LoginMysql::CallUserLoginSelect(void* param, Value* value) {
  base_storage::DBStorageEngine* engine =
      (base_storage::DBStorageEngine*) (param);
  MYSQL_ROW rows;
  int32 num = engine->RecordCount();
  DicValue* userinfo = new DicValue();
  DicValue* dict = reinterpret_cast<DicValue*>(value);
  if (num > 0) {
    while (rows = (*(MYSQL_ROW*) (engine->FetchRows())->proc)) {
      if (rows[0] != NULL)
        userinfo->SetBigInteger(L"uid", atoll(rows[0]));
      if (rows[1] != NULL)
        userinfo->SetString(L"screenName", rows[1]);
      if (rows[2] != NULL)
        userinfo->SetCharInteger(L"gender", atoi(rows[2]));
	  dict->Set(L"userinfo", userinfo);
    }
  } else {
    LOG(WARNING)<< "CallUserLoginSelect count < 0";
  }
}


void LoginMysql::CallCheckPasswdSelect(void* param, Value* value) {
  base_storage::DBStorageEngine* engine =
      (base_storage::DBStorageEngine*) (param);
  MYSQL_ROW rows;
  int32 num = engine->RecordCount();
  DicValue* dict = reinterpret_cast<DicValue*>(value);
  if (num > 0) {
    while (rows = (*(MYSQL_ROW*) (engine->FetchRows())->proc)) {
      if (rows[0] != NULL) {
        dict->SetBigInteger(L"result_", atoll(rows[0]));
      }
    }
  } else {
    LOG(WARNING)<<"CallCheckPasswdSelect count < 0";
  }
}

void LoginMysql::CallChangePasswdSelect(void* param, Value* value) {
  base_storage::DBStorageEngine* engine =
      (base_storage::DBStorageEngine*) (param);
  MYSQL_ROW rows;
  int32 num = engine->RecordCount();
  DicValue* dict = reinterpret_cast<DicValue*>(value);
  if (num > 0) {
    while (rows = (*(MYSQL_ROW*) (engine->FetchRows())->proc)) {
      if (rows[0] != NULL) {
        dict->SetBigInteger(L"result_", atoll(rows[0]));
      }
    }
  } else {
    LOG(WARNING)<<"CallChangePasswdSelect count < 0";
  }
}

} // namespace login

