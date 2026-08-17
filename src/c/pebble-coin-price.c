#include <pebble.h>
#include <pebble-coin-price.h>
#include <pebble-events/pebble-events.h>

#define CP_RESULT_MAX_LEN 64
#define CP_PERSIST_KEY 1234

static CoinPriceHandler s_handler;
static int s_next_id = 1;
static char s_price[CP_RESULT_MAX_LEN];
static CoinPriceCoin s_last_coin = 0;
static CoinPriceCurrency s_last_currency = 0;

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
  Tuple *result_t = dict_find(iter, MESSAGE_KEY_CP_RESULT);
  if (!result_t) {
    return;
  }

  int id = 0;
  Tuple *id_t = dict_find(iter, MESSAGE_KEY_CP_ID);
  if (id_t) {
    id = id_t->value->int32;
  }

  strncpy(s_price, result_t->value->cstring, sizeof(s_price) - 1);
  s_price[sizeof(s_price) - 1] = '\0';
  persist_write_string(CP_PERSIST_KEY, s_price);

  if (s_handler) {
    s_handler(s_price, CoinPriceStatusAvailable, id);
  }
}

void coin_price_init(void) {
  events_app_message_request_inbox_size(256);
  events_app_message_request_outbox_size(256);
  events_app_message_register_inbox_received(inbox_received_handler, NULL);
}

void coin_price_deinit(void) {
  s_handler = NULL;
}

void coin_price_set_handler(CoinPriceHandler handler) {
  s_handler = handler;
}

int coin_price_fetch(CoinPriceCoin coin, CoinPriceCurrency currency, const char *api_key) {
  if (s_last_coin != 0 && (coin != s_last_coin || currency != s_last_currency)) {
    s_price[0] = '\0';
    persist_delete(CP_PERSIST_KEY);
  }
  s_last_coin = coin;
  s_last_currency = currency;

  int id = s_next_id++;

  DictionaryIterator *iter;
  if (app_message_outbox_begin(&iter) != APP_MSG_OK) {
    return -1;
  }
  dict_write_int32(iter, MESSAGE_KEY_CP_COIN, (int)coin);
  dict_write_int32(iter, MESSAGE_KEY_CP_CURRENCY, (int)currency);
  if (api_key) {
    dict_write_cstring(iter, MESSAGE_KEY_CP_API_KEY, api_key);
  }
  dict_write_int32(iter, MESSAGE_KEY_CP_ID, id);
  if (app_message_outbox_send() != APP_MSG_OK) {
    return -1;
  }
  return id;
}

const char *coin_price_get_last(void) {
  if (s_price[0] != '\0') {
    return s_price;
  }
  if (persist_exists(CP_PERSIST_KEY)) {
    persist_read_string(CP_PERSIST_KEY, s_price, sizeof(s_price));
    return s_price;
  }
  return NULL;
}
