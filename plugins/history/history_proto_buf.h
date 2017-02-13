//  Copyright (c) 2015-2015 The SWP Authors. All rights reserved.
//  Created on: 2017年1月8日 Author: kerry

#ifndef SWP_HISTORY_PROTO_BUF_H_
#define SWP_HISTORY_PROTO_BUF_H_

#include "basic/basictypes.h"
#include "logic/base_values.h"
#include <list>
#include <string>

namespace history_logic {

namespace net_request {

class HistoryPosition {
 public:
  HistoryPosition()
      : id_(NULL),
        token_(NULL) {
  }

  ~HistoryPosition() {
    if (id_) {
      delete id_;
      id_ = NULL;
    }
    if (token_) {
      delete token_;
      token_ = NULL;
    }
  }

  void set_http_packet(base_logic::DictionaryValue* value);

  void set_id(const int64 id) {
    id_ = new base_logic::FundamentalValue(id);
  }

  void set_token(const std::string& token) {
    token_ = new base_logic::StringValue(token);
  }

  const int64 id() const {
    int64 id = 0;
    id_->GetAsBigInteger(&id);
    return id;
  }

  const std::string token() const {
    std::string token;
    token_->GetAsString(&token);
    return token;
  }

 public:
  base_logic::FundamentalValue* id_;
  base_logic::StringValue* token_;
};

}

namespace net_reply {

class TradesPosition {
 public:
  TradesPosition()
      : position_id_(NULL),
        id_(NULL),
        code_id_(NULL),
        symbol_(NULL),
        name_(NULL),
        buy_sell_(NULL),
        amount_(NULL),
        open_price_(NULL),
        position_time_(NULL),
        open_cost_(NULL),
        open_charge_(NULL),
        close_time_(NULL),
        close_price_(NULL),
        gross_profit_(NULL),
        limit_(NULL),
        stop_(NULL),
        close_type_(NULL),
        is_deferred_(NULL),
        deferred_(NULL),
        interval_(NULL),
        result_(NULL),
        value_(NULL) {
  }

  ~TradesPosition() {
    if (value_) {
      delete value_;
      value_ = NULL;
    }
  }

  void set_position_id(const int64 position_id) {
    position_id_ = new base_logic::FundamentalValue(position_id);
  }

  void set_id(const int64 id) {
    id_ = new base_logic::FundamentalValue(id);
  }

  void set_code_id(const int64 code_id) {
    code_id_ = new base_logic::FundamentalValue(code_id);
  }

  void set_symbol(const std::string& symbol) {
    symbol_ = new base_logic::StringValue(symbol);
  }

  void set_name(const std::string& name) {
    name_ = new base_logic::StringValue(name);
  }

  void set_buy_sell(const int32 buy_sell) {
    buy_sell_ = new base_logic::FundamentalValue(buy_sell);
  }

  void set_amount(const int64 amount) {
    amount_ = new base_logic::FundamentalValue(amount);
  }

  void set_open_price(const double open_price) {
    open_price_ = new base_logic::FundamentalValue(open_price);
  }

  void set_position_time(const int64 position_time) {
    position_time_ = new base_logic::FundamentalValue(position_time);
  }

  void set_open_cost(const double open_cost) {
    open_cost_ = new base_logic::FundamentalValue(open_cost);
  }

  void set_open_charge(const double open_charge) {
    open_charge_ = new base_logic::FundamentalValue(open_charge);
  }

  void set_result(const bool result) {
    result_ = new base_logic::FundamentalValue(result);
  }

  void set_close_time(const int64 close_time) {
    close_time_ = new base_logic::FundamentalValue(close_time);
  }

  void set_close_price(const double close_price) {
    close_price_ = new base_logic::FundamentalValue(close_price);
  }

  void set_gross_profit(const double gross_profit) {
    gross_profit_ = new base_logic::FundamentalValue(gross_profit);
  }

  void set_limit(const double limit) {
    limit_ = new base_logic::FundamentalValue(limit);
  }

  void set_stop(const double stop) {
    stop_ = new base_logic::FundamentalValue(stop);
  }

  void set_close_type(const int32 close_type) {
    close_type_ = new base_logic::FundamentalValue(close_type);
  }

  void set_is_deferred(const bool is_deferred) {
    is_deferred_ = new base_logic::FundamentalValue(is_deferred);
  }

  void set_deferred(const double deferred) {
    deferred_ = new base_logic::FundamentalValue(deferred);
  }

  void set_interval(const double interval) {
    interval_ = new base_logic::FundamentalValue(interval);
  }

  base_logic::DictionaryValue* get() {
    value_ = new base_logic::DictionaryValue();
    if (position_id_ != NULL)
      value_->Set(L"positionId", position_id_);
    if (id_ != NULL)
      value_->Set(L"id", id_);
    if (code_id_ != NULL)
      value_->Set(L"codeId", code_id_);
    if (symbol_ != NULL)
      value_->Set(L"symbol", symbol_);
    if (name_ != NULL)
      value_->Set(L"name", name_);
    if (buy_sell_ != NULL)
      value_->Set(L"buySell", buy_sell_);
    if (amount_ != NULL)
      value_->Set(L"amount", amount_);
    if (open_price_ != NULL)
      value_->Set(L"openPrice", open_price_);
    if (position_time_ != NULL)
      value_->Set(L"positionTime", position_time_);
    if (open_cost_ != NULL)
      value_->Set(L"openCost", open_cost_);
    if (open_charge_ != NULL)
      value_->Set(L"openCharge", open_charge_);
    if (close_time_ != NULL)
      value_->Set(L"closeTime", close_time_);
    if (close_price_ != NULL)
      value_->Set(L"closePrice", close_price_);
    if (gross_profit_ != NULL)
      value_->Set(L"grossProfit", gross_profit_);
    if (limit_ != NULL)
      value_->Set(L"limit", limit_);
    if (stop_ != NULL)
      value_->Set(L"stop", stop_);
    if (close_type_ != NULL)
      value_->Set(L"closeType", close_type_);
    if (is_deferred_ != NULL)
      value_->Set(L"isDeferred", is_deferred_);
    if (deferred_ != NULL)
      value_->Set(L"deferred", deferred_);
    if (result_ != NULL)
      value_->Set(L"result", result_);
    if (interval_ != NULL)
      value_->Set(L"interval", interval_);
    return value_;
  }

 private:
  base_logic::FundamentalValue* position_id_;
  base_logic::FundamentalValue* id_;
  base_logic::FundamentalValue* code_id_;
  base_logic::StringValue* symbol_;
  base_logic::StringValue* name_;
  base_logic::FundamentalValue* buy_sell_;
  base_logic::FundamentalValue* amount_;
  base_logic::FundamentalValue* open_price_;
  base_logic::FundamentalValue* position_time_;
  base_logic::FundamentalValue* open_cost_;
  base_logic::FundamentalValue* open_charge_;
  base_logic::FundamentalValue* close_time_;
  base_logic::FundamentalValue* close_price_;
  base_logic::FundamentalValue* gross_profit_;
  base_logic::FundamentalValue* limit_;
  base_logic::FundamentalValue* stop_;
  base_logic::FundamentalValue* close_type_;
  base_logic::FundamentalValue* is_deferred_;
  base_logic::FundamentalValue* deferred_;
  base_logic::FundamentalValue* interval_;
  base_logic::FundamentalValue* result_;
  base_logic::DictionaryValue* value_;
};

class AllTradesPosition {
 public:
  AllTradesPosition()
      : positions_info_(NULL),
        value_(NULL) {
    positions_info_ = new base_logic::ListValue;
  }

  ~AllTradesPosition() {
    if (value_) {
      delete value_;
      value_ = NULL;
    }
  }

  void set_unit(base_logic::DictionaryValue* value) {
    positions_info_->Append((base_logic::Value*) (value));
  }

  base_logic::DictionaryValue* get() {
    value_ = new base_logic::DictionaryValue();
    if (!positions_info_->empty()) {
      value_->Set(L"positioninfo", positions_info_);
    } else {
      delete positions_info_;
      positions_info_ = NULL;
    }
    return value_;
  }

  const int32 Size() {
    return positions_info_->GetSize();
  }

  void Reset() {
    if (value_) {
      delete value_;
      value_ = NULL;
    }
    positions_info_ = new base_logic::ListValue;
  }
 private:
  base_logic::ListValue* positions_info_;
  base_logic::DictionaryValue* value_;
};

}

}

#endif
